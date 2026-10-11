// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVPlayerCharacter.h"
#include "BVStealthComponent.h"
#include "BVInteractionComponent.h"
#include "BVTakedownComponent.h"
#include "BVPlayerController.h"
#include "Gadgets/BVEquipmentInventory.h"
#include "World/BVNoiseSubsystem.h"
#include "Core/BVGameMode.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVPlayerCharacter::ABVPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Third-person capsule: slightly shorter than default so the operative
	// reads as a human silhouette in cover.
	GetCapsuleComponent()->InitCapsuleSize(38.f, 90.f);

	// Character rotates toward movement, camera drives facing.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 540.f, 0.f);
	Move->JumpZVelocity = 420.f;
	Move->AirControl = 0.25f;
	Move->MaxWalkSpeed = WalkSpeed;
	Move->MaxWalkSpeedCrouched = CrouchSpeed;
	Move->bCanWalkOffLedges = true;
	Move->bCanWalkOffLedgesWhenCrouching = true;
	Move->NavAgentProps.bCanCrouch = true;
	Move->SetWalkableFloorAngle(50.f);
	Move->BrakingDecelerationWalking = 2000.f;
	Move->GroundFriction = 8.f;

	// Camera boom sits behind and above the shoulder.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 260.f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 60.f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 14.f;
	CameraBoom->ProbeSize = 12.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->FieldOfView = 90.f;

	StealthComponent = CreateDefaultSubobject<UBVStealthComponent>(TEXT("StealthComponent"));
	InteractionComponent = CreateDefaultSubobject<UBVInteractionComponent>(TEXT("InteractionComponent"));
	TakedownComponent = CreateDefaultSubobject<UBVTakedownComponent>(TEXT("TakedownComponent"));
	EquipmentInventory = CreateDefaultSubobject<UBVEquipmentInventory>(TEXT("EquipmentInventory"));

	Health = MaxHealth;
}

void ABVPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ABVPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateStealthInputs();
}

float ABVPlayerCharacter::GetSpeedForTier(EBVMovementSpeed Tier) const
{
	switch (Tier)
	{
	case EBVMovementSpeed::Prone:	return ProneSpeed;
	case EBVMovementSpeed::Crouch:	return CrouchSpeed;
	case EBVMovementSpeed::Silent:	return SilentSpeed;
	case EBVMovementSpeed::Walk:	return WalkSpeed;
	case EBVMovementSpeed::Jog:		return JogSpeed;
	case EBVMovementSpeed::Sprint:	return SprintSpeed;
	default:						return WalkSpeed;
	}
}

float ABVPlayerCharacter::GetCurrentPlanarSpeed() const
{
	return GetVelocity().Size2D();
}


void ABVPlayerCharacter::SetMovementSpeed(EBVMovementSpeed NewSpeed)
{
	// Posture gates which speed tiers are legal.
	if (CurrentPosture == EBVPosture::Prone && NewSpeed != EBVMovementSpeed::Prone)
	{
		return;
	}
	if (CurrentPosture == EBVPosture::Crouching &&
		(NewSpeed == EBVMovementSpeed::Sprint || NewSpeed == EBVMovementSpeed::Jog))
	{
		return;
	}

	CurrentSpeed = NewSpeed;

	UCharacterMovementComponent* Move = GetCharacterMovement();
	const float Target = GetSpeedForTier(NewSpeed);
	if (CurrentPosture == EBVPosture::Crouching)
	{
		Move->MaxWalkSpeedCrouched = Target;
	}
	else
	{
		Move->MaxWalkSpeed = Target;
	}

	// Silent move forces the silent tier regardless of requested speed.
	if (bSilentMove && NewSpeed != EBVMovementSpeed::Prone)
	{
		Move->MaxWalkSpeed = SilentSpeed;
		CurrentSpeed = EBVMovementSpeed::Silent;
	}

	const EBVNoiseLevel Noise = bSilentMove
		? EBVNoiseLevel::Silent
		: UBVNoiseSubsystem::GetNoiseLevelForSpeed(CurrentSpeed, CurrentSurface);
	OnSpeedChanged.Broadcast(CurrentSpeed, Noise);
}

void ABVPlayerCharacter::ToggleCrouch()
{
	UCharacterMovementComponent* Move = GetCharacterMovement();

	if (CurrentPosture == EBVPosture::Prone)
	{
		Move->SetMovementMode(MOVE_Walking);
		UnCrouch();
		Crouch();
		CurrentPosture = EBVPosture::Crouching;
		SetMovementSpeed(EBVMovementSpeed::Crouch);
	}
	else if (CurrentPosture == EBVPosture::Crouching)
	{
		UnCrouch();
		CurrentPosture = EBVPosture::Standing;
		SetMovementSpeed(EBVMovementSpeed::Walk);
	}
	else
	{
		Crouch();
		CurrentPosture = EBVPosture::Crouching;
		SetMovementSpeed(EBVMovementSpeed::Crouch);
	}

	OnPostureChanged.Broadcast(CurrentPosture);
}

void ABVPlayerCharacter::ToggleProne()
{
	UCharacterMovementComponent* Move = GetCharacterMovement();

	if (CurrentPosture == EBVPosture::Prone)
	{
		UnCrouch();
		CurrentPosture = EBVPosture::Crouching;
		SetMovementSpeed(EBVMovementSpeed::Crouch);
	}
	else
	{
		UnCrouch();
		// Prone reuses the crouched movement mode with a lowered capsule; the
		// animation blueprint drives the visual pose.
		Crouch();
		CurrentPosture = EBVPosture::Prone;
		SetMovementSpeed(EBVMovementSpeed::Prone);
	}

	OnPostureChanged.Broadcast(CurrentPosture);
}

void ABVPlayerCharacter::StartSprint()
{
	if (CurrentPosture != EBVPosture::Standing || bSilentMove || bInCover)
	{
		return;
	}

bool ABVPlayerCharacter::TryEnterCover()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector Start = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector Right = GetActorRightVector();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVCoverTrace), false, this);
	FHitResult Hit;

	// Probe forward, left and right to classify the cover type.
	auto Probe = [&](const FVector& Dir, EBVCoverStance Stance) -> bool
	{
		if (World->LineTraceSingleByChannel(Hit, Start, Start + Dir * CoverTraceDistance, ECC_Visibility, Params))
		{
			CoverStance = Stance;
			bInCover = true;
			bWallHugging = (Stance == EBVCoverStance::Low || Stance == EBVCoverStance::High);
			// Snap flush to the surface with a small standoff.
			const FVector Offset = -Dir * 40.f;
			SetActorLocation(Hit.ImpactPoint + Offset + FVector(0.f, 0.f, 0.f), true);
			OnCoverChanged.Broadcast(CoverStance);
			return true;
		}
		return false;
	};

	if (Probe(Forward, EBVCoverStance::High))
	{
		SetMovementSpeed(EBVMovementSpeed::Walk);
		return true;
	}
	if (Probe(-Right, EBVCoverStance::CornerLeft))
	{
		SetMovementSpeed(EBVMovementSpeed::Walk);
		return true;
	}
	if (Probe(Right, EBVCoverStance::CornerRight))
	{
		SetMovementSpeed(EBVMovementSpeed::Walk);
		return true;
	}
	return false;
}

void ABVPlayerCharacter::ExitCover()
{
	bInCover = false;
	bWallHugging = false;
	CoverStance = EBVCoverStance::None;
	StopLeaning();
	OnCoverChanged.Broadcast(CoverStance);
}

bool ABVPlayerCharacter::TryToggleWallHug()
{
	if (bWallHugging)
	{
		ExitCover();
		return false;
	}
	return TryEnterCover();
}

void ABVPlayerCharacter::SetLeaning(bool bLeanLeft)
{
	// Leaning is only meaningful against cover.
	if (!bInCover)
	{
		TryEnterCover();
	}
	if (!bInCover)
	{
		return;
	}
	bLeaning = true;
	const float Target = bLeanLeft ? -1.f : 1.f;
	LeanAmount = FMath::FInterpTo(LeanAmount, Target, GetWorld()->GetDeltaSeconds(), 8.f);

	// Lean shifts the camera laterally so the player can peek the corner.
	if (CameraBoom)
	{
		const float Lateral = bLeanLeft ? -45.f : 45.f;
		CameraBoom->SocketOffset.Y = FMath::FInterpTo(CameraBoom->SocketOffset.Y, 55.f + Lateral,
			GetWorld()->GetDeltaSeconds(), 6.f);
	}
}

void ABVPlayerCharacter::StopLeaning()
{
	bLeaning = false;

bool ABVPlayerCharacter::TryVaultOrClimb()
{
	UWorld* World = GetWorld();
	if (!World || CurrentPosture == EBVPosture::Prone)
	{
		return false;
	}

	const FVector Forward = GetActorForwardVector();
	const FVector Start = GetActorLocation();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVVaultTrace), false, this);

	// Low obstacle in front -> vault. Trace at knee height.
	FHitResult LowHit;
	const FVector LowStart = Start + FVector(0.f, 0.f, -20.f);
	if (World->LineTraceSingleByChannel(LowHit, LowStart, LowStart + Forward * VaultTraceForward,
		ECC_Visibility, Params))
	{
		// Ensure there is clear space on the far side and above.
		const FVector OverStart = LowHit.ImpactPoint + Forward * 40.f + FVector(0.f, 0.f, VaultTraceHeight);
		FHitResult ClearHit;
		const bool bBlocked = World->LineTraceSingleByChannel(ClearHit, OverStart,
			OverStart - FVector(0.f, 0.f, VaultTraceHeight + 40.f), ECC_Visibility, Params);

		const FVector Destination = bBlocked
			? ClearHit.ImpactPoint + FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.f)
			: LowHit.ImpactPoint + Forward * 120.f +
			  FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.f);

		// Launch the character toward the destination; animation warp matches it.
		SetActorLocation(Destination, true);
		SetMovementSpeed(EBVMovementSpeed::Walk);
		return true;
	}

	// High ledge in front -> climb / mantle.
	FHitResult HighHit;
	const FVector HighStart = Start + FVector(0.f, 0.f, LedgeTraceHeight * 0.5f);
	if (World->LineTraceSingleByChannel(HighHit, HighStart, HighStart + Forward * VaultTraceForward,
		ECC_Visibility, Params))
	{
		FHitResult TopHit;
		if (World->LineTraceSingleByChannel(TopHit, HighHit.ImpactPoint + FVector(0.f, 0.f, 40.f),
			HighHit.ImpactPoint - FVector(0.f, 0.f, 40.f), ECC_Visibility, Params))
		{
			const FVector Destination = TopHit.ImpactPoint +
				FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.f);
			SetActorLocation(Destination, true);
			return true;
		}
	}

	return false;
}

bool ABVPlayerCharacter::TryLedgeGrab()
{
	UWorld* World = GetWorld();
	if (!World || bLedgeHanging)
	{
		return false;
	}

	const FVector Forward = GetActorForwardVector();
	const FVector Start = GetActorLocation();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVLedgeTrace), false, this);
	FHitResult WallHit;
	if (!World->LineTraceSingleByChannel(WallHit, Start, Start + Forward * 70.f, ECC_Visibility, Params))
	{
		return false;
	}

	FHitResult TopHit;
	if (World->LineTraceSingleByChannel(TopHit, WallHit.ImpactPoint + FVector(0.f, 0.f, 60.f),
		WallHit.ImpactPoint - FVector(0.f, 0.f, 20.f), ECC_Visibility, Params))
	{
		bLedgeHanging = true;
		CurrentPosture = EBVPosture::Hanging;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->StopMovementImmediately();
		// Hang just below the ledge lip.
		SetActorLocation(TopHit.ImpactPoint - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 30.f));
		OnPostureChanged.Broadcast(CurrentPosture);
		return true;
	}
	return false;
}

void ABVPlayerCharacter::ReleaseLedge()
{
	if (!bLedgeHanging)
	{
		return;
	}
	bLedgeHanging = false;
	CurrentPosture = EBVPosture::Standing;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	OnPostureChanged.Broadcast(CurrentPosture);
}

void ABVPlayerCharacter::ApplyDamageToPlayer(float Amount)
{
	Health = FMath::Clamp(Health - Amount, 0.f, MaxHealth);
	if (Health <= 0.f)
	{
		OnIncapacitated();
	}
}

void ABVPlayerCharacter::OnIncapacitated()
{
	// Delegate to the game mode so the mission framework handles failure.
	if (ABVGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ABVGameMode>() : nullptr)
	{
		GM->OnMissionFailed(EBVFailureReason::PlayerKilled);
	}
}

EBVSurfaceType ABVPlayerCharacter::ResolveCurrentSurface() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return EBVSurfaceType::Concrete;
	}

	const FVector Start = GetActorLocation();
	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVSurfaceTrace), false, this);
	Params.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, Start, Start - FVector(0.f, 0.f, 120.f), ECC_Visibility, Params))
	{
		if (const UPhysicalMaterial* Phys = Hit.PhysMaterial.Get())
		{
			// Map by surface type name convention used in the project's materials.
			const FString Name = Phys->GetName().ToLower();
			if (Name.Contains(TEXT("metal")))	return EBVSurfaceType::Metal;
			if (Name.Contains(TEXT("wood")))		return EBVSurfaceType::Wood;
			if (Name.Contains(TEXT("carpet")))	return EBVSurfaceType::Carpet;
			if (Name.Contains(TEXT("grass")))	return EBVSurfaceType::Grass;
			if (Name.Contains(TEXT("water")))	return EBVSurfaceType::Water;
			if (Name.Contains(TEXT("gravel")))	return EBVSurfaceType::Gravel;
			if (Name.Contains(TEXT("glass")))	return EBVSurfaceType::Glass;
			if (Name.Contains(TEXT("mud")))		return EBVSurfaceType::Mud;
		}
	}
	return EBVSurfaceType::Concrete;
}

void ABVPlayerCharacter::UpdateStealthInputs()
{
	CurrentSurface = ResolveCurrentSurface();

	if (StealthComponent)
	{
		StealthComponent->SetPlayerPosture(CurrentPosture);
		StealthComponent->SetMovementSpeed(bSilentMove ? EBVMovementSpeed::Silent : CurrentSpeed);
		StealthComponent->SetLeaning(bLeaning);
		StealthComponent->SetCurrentSurface(CurrentSurface);
	}
}

	if (GetWorld())
	{
		LeanAmount = FMath::FInterpTo(LeanAmount, 0.f, GetWorld()->GetDeltaSeconds(), 8.f);
	}
	if (CameraBoom)
	{
		CameraBoom->SocketOffset.Y = FMath::FInterpTo(CameraBoom->SocketOffset.Y, 55.f,
			GetWorld()->GetDeltaSeconds(), 6.f);
	}
}

	bSprinting = true;
	SetMovementSpeed(EBVMovementSpeed::Sprint);
}

void ABVPlayerCharacter::StopSprint()
{
	if (!bSprinting)
	{
		return;
	}
	bSprinting = false;

void ABVPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		UE_LOG(LogBlackVeil, Error, TEXT("PlayerCharacter requires an EnhancedInputComponent."));
		return;
	}

	if (MoveAction)			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABVPlayerCharacter::Input_Move);
	if (LookAction)			EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABVPlayerCharacter::Input_Look);
	if (SprintAction)
	{
		EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_SprintStart);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABVPlayerCharacter::Input_SprintStop);
	}
	if (CrouchAction)		EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_Crouch);
	if (ProneAction)		EIC->BindAction(ProneAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_Prone);
	if (JumpVaultAction)	EIC->BindAction(JumpVaultAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_JumpVault);
	if (InteractAction)
	{
		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_InteractStart);
		EIC->BindAction(InteractAction, ETriggerEvent::Completed, this, &ABVPlayerCharacter::Input_InteractStop);
	}
	if (FireAction)
	{
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_FireStart);
		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &ABVPlayerCharacter::Input_FireStop);
	}
	if (AimAction)
	{
		EIC->BindAction(AimAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_AimStart);
		EIC->BindAction(AimAction, ETriggerEvent::Completed, this, &ABVPlayerCharacter::Input_AimStop);
	}
	if (TakedownAction)		EIC->BindAction(TakedownAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_Takedown);
	if (LeanLeftAction)		EIC->BindAction(LeanLeftAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_LeanLeft);
	if (LeanRightAction)	EIC->BindAction(LeanRightAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_LeanRight);
	if (WallHugAction)		EIC->BindAction(WallHugAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_WallHug);
	if (SilentMoveAction)	EIC->BindAction(SilentMoveAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_SilentMoveToggle);
	if (GadgetSlot1Action)	EIC->BindAction(GadgetSlot1Action, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_GadgetSlot1);
	if (GadgetSlot2Action)	EIC->BindAction(GadgetSlot2Action, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_GadgetSlot2);
	if (GadgetSlot3Action)	EIC->BindAction(GadgetSlot3Action, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_GadgetSlot3);
	if (GadgetSlot4Action)	EIC->BindAction(GadgetSlot4Action, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_GadgetSlot4);
	if (SwitchWeaponAction)	EIC->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_SwitchWeapon);
	if (ReloadAction)		EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_Reload);
	if (PauseAction)		EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &ABVPlayerCharacter::Input_Pause);
}

void ABVPlayerCharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	LastMoveInput = Axis;

	if (!Controller || Axis.IsNearlyZero())
	{
		return;
	}

	// Camera-relative movement so W always goes "into the screen".
	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);

	// Jog is the default when moving; walking is selected by silent mode or cover.
	if (CurrentPosture == EBVPosture::Standing && !bSilentMove && !bInCover && !bSprinting)
	{
		SetMovementSpeed(EBVMovementSpeed::Jog);
	}
	else if (CurrentPosture == EBVPosture::Standing && bSilentMove)
	{
		SetMovementSpeed(EBVMovementSpeed::Silent);
	}
}

void ABVPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ABVPlayerCharacter::Input_SprintStart(const FInputActionValue&) { StartSprint(); }
void ABVPlayerCharacter::Input_SprintStop(const FInputActionValue&) { StopSprint(); }
void ABVPlayerCharacter::Input_Crouch(const FInputActionValue&) { ToggleCrouch(); }
void ABVPlayerCharacter::Input_Prone(const FInputActionValue&) { ToggleProne(); }

void ABVPlayerCharacter::Input_JumpVault(const FInputActionValue&)
{
	// Contextual: vault a low obstacle, mantle a ledge, else normal jump.
	if (TryVaultOrClimb())
	{
		return;
	}
	Jump();
}

void ABVPlayerCharacter::Input_InteractStart(const FInputActionValue&)
{
	if (InteractionComponent)
	{
		InteractionComponent->BeginInteract();
	}
}

void ABVPlayerCharacter::Input_InteractStop(const FInputActionValue&)
{
	if (InteractionComponent)
	{
		InteractionComponent->EndInteract();
	}
}

	SetMovementSpeed(CurrentPosture == EBVPosture::Standing
		? EBVMovementSpeed::Jog : EBVMovementSpeed::Crouch);
}

void ABVPlayerCharacter::SetSilentMove(bool bEnabled)
{
	bSilentMove = bEnabled;
	SetMovementSpeed(bEnabled ? EBVMovementSpeed::Silent : EBVMovementSpeed::Walk);
}


void ABVPlayerCharacter::Input_FireStart(const FInputActionValue&)
{
	if (EquipmentInventory)
	{
		EquipmentInventory->StartFire();
	}
}

void ABVPlayerCharacter::Input_FireStop(const FInputActionValue&)
{
	if (EquipmentInventory)
	{
		EquipmentInventory->StopFire();
	}
}

void ABVPlayerCharacter::Input_AimStart(const FInputActionValue&)
{
	bIsAiming = true;
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = 170.f;
		CameraBoom->SocketOffset = FVector(0.f, 65.f, 55.f);
	}
}

void ABVPlayerCharacter::Input_AimStop(const FInputActionValue&)
{
	bIsAiming = false;
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = 260.f;
		CameraBoom->SocketOffset = FVector(0.f, 55.f, 60.f);
	}
}

void ABVPlayerCharacter::Input_Takedown(const FInputActionValue&)
{
	if (TakedownComponent)
	{
		// Non-lethal by default; the takedown component decides lethality from
		// the current context (equipped weapon, target state, player choice).
		TakedownComponent->TryTakedown(EBVLethality::NonLethal);
	}
}

void ABVPlayerCharacter::Input_LeanLeft(const FInputActionValue&) { SetLeaning(true); }
void ABVPlayerCharacter::Input_LeanRight(const FInputActionValue&) { SetLeaning(false); }
void ABVPlayerCharacter::Input_WallHug(const FInputActionValue&) { TryToggleWallHug(); }
void ABVPlayerCharacter::Input_SilentMoveToggle(const FInputActionValue&) { SetSilentMove(!bSilentMove); }

void ABVPlayerCharacter::Input_GadgetSlot1(const FInputActionValue&)
{
	if (EquipmentInventory) { EquipmentInventory->SelectGadgetSlot(0); }
}
void ABVPlayerCharacter::Input_GadgetSlot2(const FInputActionValue&)
{
	if (EquipmentInventory) { EquipmentInventory->SelectGadgetSlot(1); }
}
void ABVPlayerCharacter::Input_GadgetSlot3(const FInputActionValue&)
{
	if (EquipmentInventory) { EquipmentInventory->SelectGadgetSlot(2); }
}
void ABVPlayerCharacter::Input_GadgetSlot4(const FInputActionValue&)
{
	if (EquipmentInventory) { EquipmentInventory->SelectGadgetSlot(3); }
}
void ABVPlayerCharacter::Input_SwitchWeapon(const FInputActionValue&)
{
	if (EquipmentInventory) { EquipmentInventory->CycleWeapon(); }
}
void ABVPlayerCharacter::Input_Reload(const FInputActionValue&)
{
	if (EquipmentInventory) { EquipmentInventory->Reload(); }
}
void ABVPlayerCharacter::Input_Pause(const FInputActionValue&)
{
	// Routed through the player controller which owns the pause menu widget.
	if (ABVPlayerController* PC = Cast<ABVPlayerController>(GetController()))
	{
		PC->TogglePauseMenu();
	}
}
