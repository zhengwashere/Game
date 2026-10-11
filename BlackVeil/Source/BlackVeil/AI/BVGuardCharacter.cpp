// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGuardCharacter.h"
#include "BVPatrolComponent.h"
#include "BVAlertStateComponent.h"
#include "BVAIController.h"
#include "World/BVHidingSpot.h"
#include "Components/SpotLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BVPlayerCharacter.h"
#include "BlackVeil.h"

ABVGuardCharacter::ABVGuardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ABVAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Guards stand a little shorter than the player and never crouch automatically.
	GetCapsuleComponent()->InitCapsuleSize(40.f, 92.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 320.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 220.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(GetMesh(), TEXT("head"));
	Flashlight->SetIntensity(4500.f);
	Flashlight->SetAttenuationRadius(2200.f);
	Flashlight->SetInnerConeAngle(14.f);
	Flashlight->SetOuterConeAngle(30.f);
	Flashlight->SetVisibility(false);

	PatrolComponent = CreateDefaultSubobject<UBVPatrolComponent>(TEXT("PatrolComponent"));
	AlertStateComponent = CreateDefaultSubobject<UBVAlertStateComponent>(TEXT("AlertStateComponent"));

	Health = MaxHealth;
}

void ABVGuardCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	// Armored and elite guards soak more damage.
	switch (GuardType)
	{
	case EBVGuardType::Armored: ArmorMitigation = 0.35f; break;
	case EBVGuardType::Elite:   ArmorMitigation = 0.25f; MaxHealth = 140.f; Health = MaxHealth; break;
	case EBVGuardType::Commander: ArmorMitigation = 0.15f; break;
	default: ArmorMitigation = 0.f; break;
	}
}

void ABVGuardCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool ABVGuardCharacter::IsAlertedToPlayer() const
{
	if (bIncapacitated) { return false; }
	if (!AlertStateComponent) { return false; }
	const EBVAlertState State = AlertStateComponent->GetAlertState();
	return State == EBVAlertState::Investigating
		|| State == EBVAlertState::Searching
		|| State == EBVAlertState::Alerted
		|| State == EBVAlertState::Combat;
}

void ABVGuardCharacter::ApplyDamageToGuard(float Amount, AActor* Causer)
{
	if (bIncapacitated) { return; }

	const float Mitigated = Amount * (1.f - FMath::Clamp(ArmorMitigation, 0.f, 0.9f));
	Health = FMath::Max(0.f, Health - Mitigated);

	if (Health <= 0.f)
	{
		EnterIncapacitatedState(EBVLethality::Lethal);
	}
	else if (Causer)
	{
		// Being shot at without dying still alerts the guard.
		if (AlertStateComponent)
		{
			AlertStateComponent->SetAlertState(EBVAlertState::Combat);
		}
	}
}

void ABVGuardCharacter::ReceiveTakedown(EBVTakedownType Type, EBVLethality Lethality)
{
	if (bIncapacitated) { return; }
	EnterIncapacitatedState(Lethality);
}

void ABVGuardCharacter::EnterIncapacitatedState(EBVLethality Lethality)
{
	bIncapacitated = true;
	bDead = (Lethality == EBVLethality::Lethal);
	Health = 0.f;

	// Stop the brain and movement; the body stays in the world for concealment.
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		if (AI->GetBrainComponent())
		{
			AI->GetBrainComponent()->StopLogic(TEXT("Incapacitated"));
		}
		AI->UnPossess();
	}

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Ragdoll or slump animation is driven by the animation blueprint reacting
	// to bIncapacitated / bDead.
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	SetFlashlightEnabled(false);
	OnGuardIncapacitated.Broadcast(this);
}

void ABVGuardCharacter::SetBeingCarried(bool bCarried)
{
	bBeingCarried = bCarried;

	if (bCarried)
	{
		// Attach to the carrier's shoulder and disable physics so it rides along.
		if (AActor* Carrier = GetAttachParentActor())
		{
			// already attached
		}
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void ABVGuardCharacter::ConcealIn(ABVHidingSpot* Spot)
{
	if (!Spot) { return; }

	bConcealed = true;
	ConcealmentSpot = Spot;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorLocation(Spot->GetConcealmentLocation());

	UE_LOG(LogBlackVeil, Log, TEXT("Body %s concealed in %s"), *GetName(), *Spot->GetName());
}

void ABVGuardCharacter::OnDiscoveredBy(ABVGuardCharacter* Discoverer)
{
	if (!Discoverer || !AlertStateComponent) { return; }

	// A found body puts the discoverer straight into a search, and they will
	// radio it in through the squad manager.
	AlertStateComponent->SetAlertState(EBVAlertState::Searching);
	AlertStateComponent->ReportSighting(GetActorLocation(), 0.6f);
}

void ABVGuardCharacter::ApplyStatusEffect(EBVStatusEffect Effect, float Duration)
{
	if (Effect == EBVStatusEffect::None) { return; }
	const float Expiry = GetWorld() ? GetWorld()->GetTimeSeconds() + Duration : Duration;
	StatusEffectEndTimes.Add(Effect, Expiry);
}

bool ABVGuardCharacter::HasStatusEffect(EBVStatusEffect Effect) const
{
	if (const float* Expiry = StatusEffectEndTimes.Find(Effect))
	{
		return GetWorld() && GetWorld()->GetTimeSeconds() < *Expiry;
	}
	return false;
}

float ABVGuardCharacter::GetSightMultiplier() const
{
	float Mult = 1.f;
	if (HasStatusEffect(EBVStatusEffect::Blinded)) { Mult *= 0.08f; }
	if (HasStatusEffect(EBVStatusEffect::Stunned)) { Mult *= 0.35f; }
	if (HasStatusEffect(EBVStatusEffect::Disrupted)) { Mult *= 0.6f; }
	return Mult;
}

float ABVGuardCharacter::GetHearingMultiplier() const
{
	return HasStatusEffect(EBVStatusEffect::Deafened) ? 0.15f : 1.f;
}

void ABVGuardCharacter::SetFlashlightEnabled(bool bEnabled)
{
	if (Flashlight)
	{
		Flashlight->SetVisibility(bEnabled && !bIncapacitated);
	}
}

bool ABVGuardCharacter::CanFire() const
{
	return !bIncapacitated && (GetWorld()->GetTimeSeconds() - LastFireTime) >= FireInterval;
}

void ABVGuardCharacter::FireAtTarget(AActor* Target)
{
	if (!Target || !CanFire()) { return; }

	LastFireTime = GetWorld()->GetTimeSeconds();

	// Hitscan with a spread that widens with distance, then damage if it lands
	// on the player. Full ballistics/tracers are handled by the animation + VFX.
	const FVector Muzzle = GetActorLocation() + FVector(0.f, 0.f, 60.f);
	const FVector TargetPoint = Target->GetActorLocation();
	const float Dist = FVector::Dist(Muzzle, TargetPoint);

	if (Dist > WeaponRange) { return; }

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVGuardFire), false, this);
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Muzzle, TargetPoint, ECC_Pawn, Params))
	{
		if (ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(Hit.GetActor()))
		{
			Player->ApplyDamageToPlayer(WeaponDamage);
		}
	}
}
