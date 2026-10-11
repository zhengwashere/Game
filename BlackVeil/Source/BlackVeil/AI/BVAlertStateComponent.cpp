// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVAlertStateComponent.h"
#include "BVGuardCharacter.h"
#include "BVAIController.h"
#include "BVSquadManager.h"
#include "Engine/World.h"
#include "BlackVeil.h"

UBVAlertStateComponent::UBVAlertStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UBVAlertStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

FString UBVAlertStateComponent::GetAlertStateName(EBVAlertState State)
{
	switch (State)
	{
	case EBVAlertState::Unaware:       return TEXT("UNAWARE");
	case EBVAlertState::Suspicious:    return TEXT("SUSPICIOUS");
	case EBVAlertState::Investigating: return TEXT("INVESTIGATING");
	case EBVAlertState::Searching:     return TEXT("SEARCHING");
	case EBVAlertState::Alerted:       return TEXT("ALERTED");
	case EBVAlertState::Combat:        return TEXT("COMBAT");
	case EBVAlertState::Returning:     return TEXT("RETURNING");
	default:                           return TEXT("UNKNOWN");
	}
}

bool UBVAlertStateComponent::IsHunting() const
{
	return AlertState == EBVAlertState::Investigating
		|| AlertState == EBVAlertState::Searching
		|| AlertState == EBVAlertState::Alerted
		|| AlertState == EBVAlertState::Combat;
}

void UBVAlertStateComponent::SetAlertState(EBVAlertState NewState)
{
	if (NewState == AlertState) { return; }

	const EBVAlertState Old = AlertState;
	AlertState = NewState;
	StateTimer = 0.f;

	OnAlertStateChanged.Broadcast(Old, NewState);
	UE_LOG(LogBlackVeil, Verbose, TEXT("%s alert state %s -> %s"),
		*GetNameSafe(GetOwner()), *GetAlertStateName(Old), *GetAlertStateName(NewState));
}

void UBVAlertStateComponent::ReportSighting(const FVector& ObservedLocation, float Confidence)
{
	ABVGuardCharacter* Guard = Cast<ABVGuardCharacter>(GetOwner());
	const float Vigilance = Guard ? FMath::Lerp(0.6f, 1.5f, Guard->Personality.Vigilance) : 1.f;

	LastKnownPlayerLocation = ObservedLocation;
	bHasLastKnownLocation = true;
	TimeSinceLastSighting = 0.f;

	Suspicion = FMath::Clamp(Suspicion + SightSuspicionGain * Confidence * Vigilance, 0.f, SuspicionThreshold);
	OnSuspicionChanged.Broadcast(Suspicion);

	EvaluateEscalation();
}

void UBVAlertStateComponent::ReportNoise(const FVector& NoiseLocation, float Loudness)
{
	// A loud noise the guard cannot see still raises suspicion, but less than a
	// sighting, and only sets a last-known location if we have nothing better.
	if (!bHasLastKnownLocation || AlertState == EBVAlertState::Unaware)
	{
		LastKnownPlayerLocation = NoiseLocation;
		bHasLastKnownLocation = true;
	}

	Suspicion = FMath::Clamp(Suspicion + NoiseSuspicionGain * Loudness, 0.f, SuspicionThreshold);
	OnSuspicionChanged.Broadcast(Suspicion);

	EvaluateEscalation();
}

void UBVAlertStateComponent::ReceiveCommunicatedContact(const FVector& Location, EBVAlertState SuggestedState)
{
	LastKnownPlayerLocation = Location;
	bHasLastKnownLocation = true;

	// Communication can only raise us to the suggested state, never beyond it.
	if (static_cast<uint8>(SuggestedState) > static_cast<uint8>(AlertState))
	{
		SetAlertState(SuggestedState);
	}
	else if (AlertState == EBVAlertState::Unaware)
	{
		Suspicion = FMath::Max(Suspicion, SuspicionThreshold * 0.5f);
		SetAlertState(EBVAlertState::Suspicious);
	}
}

void UBVAlertStateComponent::EvaluateEscalation()
{
	switch (AlertState)
	{
	case EBVAlertState::Unaware:
		if (Suspicion >= SuspicionThreshold * 0.35f) { SetAlertState(EBVAlertState::Suspicious); }
		break;

	case EBVAlertState::Suspicious:
		if (Suspicion >= SuspicionThreshold) { SetAlertState(EBVAlertState::Investigating); }
		break;

	case EBVAlertState::Investigating:
		if (Suspicion >= SuspicionThreshold) { SetAlertState(EBVAlertState::Searching); }
		break;

	case EBVAlertState::Searching:
		if (Suspicion >= SuspicionThreshold) { SetAlertState(EBVAlertState::Alerted); }
		break;

	case EBVAlertState::Alerted:
		// Full awareness: only a confirmed sighting pushes to combat, handled by
		// the AI controller calling SetAlertState(Combat) directly.
		break;

	default:
		break;
	}
}

void UBVAlertStateComponent::EvaluateDeescalation(float DeltaTime)
{
	// Suspicion always decays when nothing new arrives.
	if (Suspicion > 0.f)
	{
		const ABVGuardCharacter* Guard = Cast<ABVGuardCharacter>(GetOwner());
		const float Patience = Guard ? FMath::Lerp(2.0f, 0.5f, Guard->Personality.SearchPatience) : 1.f;
		Suspicion = FMath::Max(0.f, Suspicion - SuspicionDecayRate * Patience * DeltaTime);
		OnSuspicionChanged.Broadcast(Suspicion);
	}

	StateTimer += DeltaTime;

	// Timed states eventually lapse if nothing sustains them.
	switch (AlertState)
	{
	case EBVAlertState::Suspicious:
		if (StateTimer > InvestigatingDuration && Suspicion < SuspicionThreshold * 0.2f)
		{
			ReturnToPatrol();
		}
		break;

	case EBVAlertState::Investigating:
		if (StateTimer > InvestigatingDuration && Suspicion < SuspicionThreshold * 0.4f)
		{
			ReturnToPatrol();
		}
		break;

	case EBVAlertState::Searching:
		if (StateTimer > SearchingDuration && Suspicion < SuspicionThreshold * 0.5f)
		{
			ReturnToPatrol();
		}
		break;

	case EBVAlertState::Alerted:
		if (StateTimer > AlertedDuration && Suspicion < SuspicionThreshold * 0.6f)
		{
			ReturnToPatrol();
		}
		break;

	case EBVAlertState::Combat:
		if (StateTimer > CombatCooldown && Suspicion < SuspicionThreshold * 0.5f)
		{
			SetAlertState(EBVAlertState::Searching);
		}
		break;

	case EBVAlertState::Returning:
		if (StateTimer > 2.f && Suspicion <= 0.f)
		{
			SetAlertState(EBVAlertState::Unaware);
		}
		break;

	default:
		break;
	}
}

void UBVAlertStateComponent::ReturnToPatrol()
{
	SetAlertState(EBVAlertState::Returning);
	Suspicion = 0.f;
	OnSuspicionChanged.Broadcast(Suspicion);
}

void UBVAlertStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceLastSighting += DeltaTime;

	// Sightings go stale: after the guard's memory duration we drop the contact.
	if (const ABVGuardCharacter* Guard = Cast<ABVGuardCharacter>(GetOwner()))
	{
		if (bHasLastKnownLocation && TimeSinceLastSighting > Guard->Personality.MemoryDuration)
		{
			bHasLastKnownLocation = false;
		}
	}

	EvaluateDeescalation(DeltaTime);
}
