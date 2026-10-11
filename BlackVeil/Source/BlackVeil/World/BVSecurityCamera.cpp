// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVSecurityCamera.h"
#include "BVAlarmPanel.h"
#include "World/BVNoiseSubsystem.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVStealthComponent.h"
#include "Core/BlackVeilGameInstance.h"
#include "Components/SpotLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "CollisionQueryParams.h"
#include "BlackVeil.h"

ABVSecurityCamera::ABVSecurityCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionType = EBVInteractionType::Camera;
	PromptText = NSLOCTEXT("BlackVeil", "CameraHack", "Access Camera");
	IconTag = TEXT("Camera");
	AvailableActions = { TEXT("Camera.Hack"), TEXT("Camera.Disable") };

	SweepPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SweepPivot"));
	SweepPivot->SetupAttachment(MeshComponent);

	CameraBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBody"));
	CameraBody->SetupAttachment(SweepPivot);

	VisionCone = CreateDefaultSubobject<USpotLightComponent>(TEXT("VisionCone"));
	VisionCone->SetupAttachment(SweepPivot);
	VisionCone->SetIntensity(300.f);
	VisionCone->SetAttenuationRadius(1800.f);
	VisionCone->SetOuterConeAngle(60.f);
	VisionCone->SetLightColor(FLinearColor(1.f, 0.15f, 0.1f));
}

void ABVSecurityCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDestroyed) { return; }

	// Sweep the head back and forth across its arc.
	SweepAlpha += DeltaSeconds * (SweepSpeed / 90.f);
	const float Yaw = InitialYaw + FMath::Sin(SweepAlpha) * SweepHalfAngle;
	if (SweepPivot)
	{
		SweepPivot->SetRelativeRotation(FRotator(0.f, Yaw, 0.f));
	}

	if (bDisabled || !bPowered) { return; }

	EvaluateDetection(DeltaSeconds);
}

void ABVSecurityCamera::EvaluateDetection(float DeltaSeconds)
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	APlayerController* PC = World->GetFirstPlayerController();
	ABVPlayerCharacter* Player = PC ? Cast<ABVPlayerCharacter>(PC->GetPawn()) : nullptr;
	if (!Player) { return; }

	const FVector CamLoc = VisionCone ? VisionCone->GetComponentLocation() : GetActorLocation();
	const FVector CamDir = VisionCone ? VisionCone->GetForwardVector() : GetActorForwardVector();
	const FVector PlayerLoc = Player->GetActorLocation();

	const FVector ToPlayer = PlayerLoc - CamLoc;
	const float Dist = ToPlayer.Size();

	bool bVisible = false;
	if (Dist <= DetectionRange)
	{
		const float Angle = FMath::RadiansToDegrees(
			FMath::Acos(FMath::Clamp(FVector::DotProduct(CamDir, ToPlayer.GetSafeNormal()), -1.f, 1.f)));

		if (Angle <= FieldOfViewDegrees * 0.5f)
		{
			// Line of sight check - cameras cannot see through walls either.
			FCollisionQueryParams Params(SCENE_QUERY_STAT(BVCameraLOS), false, this);
			FHitResult Hit;
			const bool bBlocked = World->LineTraceSingleByChannel(Hit, CamLoc, PlayerLoc, ECC_Visibility, Params);
			if (!bBlocked || Hit.GetActor() == Player)
			{
				// Visibility scales detection speed: a player in shadow is much
				// harder for a camera to resolve.
				float VisFactor = 1.f;
				if (const UBVStealthComponent* Stealth = Player->GetStealthComponent())
				{
					VisFactor = FMath::Clamp(Stealth->GetVisibilityLevel(), 0.08f, 1.f);
				}
				const float DistFactor = FMath::Clamp(1.f - (Dist / DetectionRange), 0.15f, 1.f);

				DetectionProgress += DetectionRate * VisFactor * DistFactor * DeltaSeconds;
				bVisible = true;
			}
		}
	}

	if (!bVisible)
	{
		DetectionProgress = FMath::Max(0.f, DetectionProgress - 25.f * DeltaSeconds);
	}

	if (DetectionProgress >= 100.f)
	{
		DetectionProgress = 0.f;
		TriggerAlarm(Player);
	}
}

void ABVSecurityCamera::TriggerAlarm(AActor* Detected)
{
	UE_LOG(LogBlackVeil, Warning, TEXT("Camera %s detected an intruder."), *GetName());
	OnCameraDetected.Broadcast(this, Detected);

	// Raise the alarm on every panel in the same group.
	for (TActorIterator<ABVAlarmPanel> It(GetWorld()); It; ++It)
	{
		ABVAlarmPanel* Panel = *It;
		if (Panel && Panel->AlarmGroupID == AlarmGroupID)
		{
			Panel->TriggerAlarm(Detected, this);
		}
	}

	if (UBlackVeilGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UBlackVeilGameInstance>() : nullptr)
	{
		GI->RecordAlarm();
	}
}

void ABVSecurityCamera::DisableFor_Implementation(float Duration)
{
	bDisabled = true;
	if (VisionCone) { VisionCone->SetVisibility(false); }

	if (Duration > 0.f)
	{
		GetWorldTimerManager().SetTimer(ReenableTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			RestorePower_Implementation();
		}), Duration, false);
	}
}

void ABVSecurityCamera::RestorePower_Implementation()
{
	if (bDestroyed) { return; }
	bDisabled = false;
	bPowered = true;
	if (VisionCone) { VisionCone->SetVisibility(true); }
}

void ABVSecurityCamera::SetDetectionEnabled(bool bEnabled)
{
	bDisabled = !bEnabled;
	if (VisionCone) { VisionCone->SetVisibility(bEnabled && !bDestroyed); }
}

void ABVSecurityCamera::TakeCameraDamage(float Amount)
{
	if (!bDestroyable || bDestroyed) { return; }

	Health -= Amount;
	if (Health <= 0.f)
	{
		bDestroyed = true;
		bPowered = false;
		if (VisionCone) { VisionCone->SetVisibility(false); }
		SetActorTickEnabled(false);

		// Shooting a camera out is noisy and might draw attention anyway.
		if (UBVNoiseSubsystem* Noise = GetWorld() ? GetWorld()->GetSubsystem<UBVNoiseSubsystem>() : nullptr)
		{
			Noise->ReportNoise(GetActorLocation(), EBVNoiseLevel::Medium, this, false, TEXT("CameraDestroyed"));
		}
	}
}
