// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVThrowableEffects.h"
#include "AI/BVGuardCharacter.h"
#include "World/BVNoiseSubsystem.h"
#include "World/BVElectronicDeviceInterface.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "CollisionQueryParams.h"
#include "BlackVeil.h"

// ---------------------------------------------------------------- NoiseMaker

ABVProjectile_NoiseMaker::ABVProjectile_NoiseMaker()
{
	FuseTime = 0.8f;
	NoiseRadius = 1800.f;
	EffectRadius = 0.f;
}

void ABVProjectile_NoiseMaker::Detonate()
{
	if (bDetonated) { return; }
	bDetonated = true;

	// A noise maker does not destroy itself: it rattles for a while so guards
	// commit to investigating it rather than losing the thread immediately.
	PulsesRemaining = FMath::Max(1, FMath::FloorToInt(LingerDuration / PulseInterval));
	GetWorldTimerManager().SetTimer(PulseTimer, this, &ABVProjectile_NoiseMaker::PulseNoise, PulseInterval, true);
}

void ABVProjectile_NoiseMaker::PulseNoise()
{
	if (PulsesRemaining-- <= 0)
	{
		GetWorldTimerManager().ClearTimer(PulseTimer);
		Destroy();
		return;
	}

	if (UBVNoiseSubsystem* Noise = GetWorld() ? GetWorld()->GetSubsystem<UBVNoiseSubsystem>() : nullptr)
	{
		Noise->ReportNoise(GetActorLocation(), EBVNoiseLevel::Medium, this, false, TEXT("NoiseMaker"));
	}
}

// ---------------------------------------------------------------- Flashbang

ABVProjectile_Flashbang::ABVProjectile_Flashbang()
{
	FuseTime = 2.2f;
	EffectRadius = 900.f;
	NoiseRadius = 2600.f;
}

void ABVProjectile_Flashbang::Detonate()
{
	if (bDetonated) { return; }
	bDetonated = true;

	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ABVGuardCharacter> It(World); It; ++It)
		{
			ABVGuardCharacter* Guard = *It;
			if (!Guard || Guard->IsIncapacitated()) { continue; }

			const float Dist = FVector::Dist(GetActorLocation(), Guard->GetActorLocation());
			if (Dist > EffectRadius) { continue; }

			// Facing matters: guards looking at the burst are blinded, others are
			// only deafened by the bang.
			const FVector ToBurst = (GetActorLocation() - Guard->GetActorLocation()).GetSafeNormal();
			const float Facing = FVector::DotProduct(Guard->GetActorForwardVector(), ToBurst);

			if (Facing > 0.2f)
			{
				Guard->ApplyStatusEffect(EBVStatusEffect::Blinded, BlindDuration);
			}
			Guard->ApplyStatusEffect(EBVStatusEffect::Deafened, DeafenedDuration);
		}

		if (UBVNoiseSubsystem* Noise = World->GetSubsystem<UBVNoiseSubsystem>())
		{
			Noise->ReportNoise(GetActorLocation(), EBVNoiseLevel::Deafening, this, false, TEXT("Flashbang"));
		}
	}

	Destroy();
}

// -------------------------------------------------------------------- Smoke

ABVProjectile_Smoke::ABVProjectile_Smoke()
{
	FuseTime = 1.6f;
	EffectRadius = 700.f;
	NoiseRadius = 900.f;
}

void ABVProjectile_Smoke::Detonate()
{
	if (bDetonated) { return; }
	bDetonated = true;

	// Register a smoke volume with the world so AI sight checks inside it are
	// heavily penalised. The volume removes itself when the smoke clears.
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// The Niagara smoke effect and volume actor are Blueprint subclasses of
		// ABVThrowableProjectile; spawn a generic volume here and let the BP add VFX.
		if (AActor* Volume = World->SpawnActor<AActor>(AActor::StaticClass(), GetActorLocation(), FRotator::ZeroRotator, Params))
		{
			// Tag the actor so AI sight queries can find it.
			Volume->Tags.Add(TEXT("BVSightBlocker"));
			Volume->SetLifeSpan(SmokeDuration);
		}
	}

	Destroy();
}

// ---------------------------------------------------------------------- EMP

ABVProjectile_EMP::ABVProjectile_EMP()
{
	FuseTime = 1.2f;
	EffectRadius = 1200.f;
	NoiseRadius = 800.f;
}

void ABVProjectile_EMP::Detonate()
{
	if (bDetonated) { return; }
	bDetonated = true;

	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor || Actor == this) { continue; }
			if (!Actor->GetClass()->ImplementsInterface(UBVElectronicDeviceInterface::StaticClass())) { continue; }

			const float Dist = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
			if (Dist > EffectRadius) { continue; }

			IBVElectronicDeviceInterface::Execute_DisableFor(Actor, DisableDuration);
		}
	}

	Destroy();
}

// -------------------------------------------------------------- StickyCamera

ABVProjectile_StickyCamera::ABVProjectile_StickyCamera()
{
	FuseTime = 0.f;
	EffectRadius = 0.f;
	NoiseRadius = 0.f;
	ImpactSpeedThreshold = 0.f;

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	CaptureComponent->SetupAttachment(MeshComponent);
	CaptureComponent->FOVAngle = 95.f;
	CaptureComponent->bCaptureEveryFrame = false;
}

void ABVProjectile_StickyCamera::Detonate()
{
	// A sticky camera is permanent until destroyed: it just stops moving.
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}
	bDetonated = true;
}

void ABVProjectile_StickyCamera::EmitDistractionNoise()
{
	if (UBVNoiseSubsystem* Noise = GetWorld() ? GetWorld()->GetSubsystem<UBVNoiseSubsystem>() : nullptr)
	{
		Noise->ReportNoise(GetActorLocation(), EBVNoiseLevel::Medium, this, false, TEXT("RemoteCamera"));
	}
}
