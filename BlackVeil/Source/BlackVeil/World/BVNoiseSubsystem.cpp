// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVNoiseSubsystem.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Perception/AISense_Hearing.h"
#include "BlackVeil.h"

void UBVNoiseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogBlackVeil, Log, TEXT("Noise subsystem initialized."));
}

void UBVNoiseSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

float UBVNoiseSubsystem::GetBaseRadiusForLevel(EBVNoiseLevel Level)
{
	switch (Level)
	{
	case EBVNoiseLevel::Silent:		return 0.f;
	case EBVNoiseLevel::Light:		return 600.f;
	case EBVNoiseLevel::Medium:		return 1200.f;
	case EBVNoiseLevel::Heavy:		return 2200.f;
	case EBVNoiseLevel::Deafening:	return 5000.f;
	default:						return 600.f;
	}
}

float UBVNoiseSubsystem::GetSurfaceNoiseMultiplier(EBVSurfaceType Surface)
{
	switch (Surface)
	{
	case EBVSurfaceType::Carpet:	return 0.45f;
	case EBVSurfaceType::Grass:		return 0.6f;
	case EBVSurfaceType::Mud:		return 0.7f;
	case EBVSurfaceType::Wood:		return 1.0f;
	case EBVSurfaceType::Concrete:	return 1.1f;
	case EBVSurfaceType::Gravel:	return 1.35f;
	case EBVSurfaceType::Metal:		return 1.4f;
	case EBVSurfaceType::Glass:		return 1.6f;
	case EBVSurfaceType::Water:		return 1.5f;
	default:						return 1.0f;
	}
}

EBVNoiseLevel UBVNoiseSubsystem::GetNoiseLevelForSpeed(EBVMovementSpeed Speed, EBVSurfaceType Surface)
{
	// Base level from the speed tier, then bias by surface.
	EBVNoiseLevel Base;
	switch (Speed)
	{
	case EBVMovementSpeed::Prone:	Base = EBVNoiseLevel::Silent;	break;
	case EBVMovementSpeed::Crouch:	Base = EBVNoiseLevel::Light;	break;
	case EBVMovementSpeed::Silent:	Base = EBVNoiseLevel::Silent;	break;
	case EBVMovementSpeed::Walk:	Base = EBVNoiseLevel::Light;	break;
	case EBVMovementSpeed::Jog:		Base = EBVNoiseLevel::Medium;	break;
	case EBVMovementSpeed::Sprint:	Base = EBVNoiseLevel::Heavy;	break;
	default:						Base = EBVNoiseLevel::Light;	break;
	}

	// Loud surfaces escalate one tier; soft surfaces de-escalate quiet tiers.
	const float Mult = GetSurfaceNoiseMultiplier(Surface);
	if (Mult >= 1.35f && Base != EBVNoiseLevel::Silent)
	{
		Base = static_cast<EBVNoiseLevel>(FMath::Min(static_cast<int32>(Base) + 1,
			static_cast<int32>(EBVNoiseLevel::Heavy)));
	}
	else if (Mult <= 0.6f)
	{
		Base = static_cast<EBVNoiseLevel>(FMath::Max(static_cast<int32>(Base) - 1,
			static_cast<int32>(EBVNoiseLevel::Silent)));
	}
	return Base;
}

void UBVNoiseSubsystem::ReportNoise(const FVector& Location, EBVNoiseLevel Level, AActor* Instigator,
	bool bIsGunshot, FName Tag)
{
	if (Level == EBVNoiseLevel::Silent)
	{
		return;
	}

	FBVNoiseEvent Event;
	Event.Location = Location;
	Event.Level = Level;
	Event.Instigator = Instigator;
	Event.bIsGunshot = bIsGunshot;
	Event.Tag = Tag;
	Event.Radius = GetBaseRadiusForLevel(Level);
	if (bIsGunshot)
	{
		Event.Radius *= 1.5f;
	}

	OnNoiseReported.Broadcast(Event);

	// Funnel the event into AI hearing so every audible action in the game -
	// footsteps, doors, gadgets, gunshots - reaches the guards through the same
	// path. Nothing else reports noise to the AI, which keeps this honest.
	if (UWorld* World = GetWorld())
	{
		UAISense_Hearing::ReportNoiseEvent(
			World,
			Location,
			/*Loudness*/ FMath::Clamp(Event.Radius / 5000.f, 0.2f, 1.5f),
			Instigator,
			/*MaxRange*/ Event.Radius,
			Tag);
	}
}

bool UBVNoiseSubsystem::CanHearEvent(const FBVNoiseEvent& Event, const FVector& ListenerLocation,
	float ListenerMultiplier) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float EffectiveRadius = Event.Radius * FMath::Max(ListenerMultiplier, 0.1f);
	const float DistSq = FVector::DistSquared(Event.Location, ListenerLocation);
	if (DistSq > FMath::Square(EffectiveRadius))
	{
		return false;
	}

	// Occlusion: count blocking hits between listener and source. Each wall
	// reduces the effective loudness so distant sounds through structure fade.
	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVNoiseOcclusion), false, Event.Instigator.Get());
	Params.bReturnPhysicalMaterial = false;

	TArray<FHitResult> Hits;
	World->LineTraceMultiByChannel(Hits, ListenerLocation, Event.Location, ECC_Visibility, Params);

	// Ignore hits that belong to the listener itself (approximated by distance ~0).
	int32 Blockers = 0;
	for (const FHitResult& Hit : Hits)
	{
		if (FVector::DistSquared(Hit.ImpactPoint, ListenerLocation) < 100.f)
		{
			continue;
		}
		++Blockers;
	}

	// Each blocker halves the remaining audible range.
	const float AttenuatedRadius = EffectiveRadius / FMath::Pow(2.f, static_cast<float>(Blockers));
	return DistSq <= FMath::Square(AttenuatedRadius);
}
