// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVSquadManager.h"
#include "BVAIController.h"
#include "BVGuardCharacter.h"
#include "BVAlertStateComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVSquadManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogBlackVeil, Log, TEXT("Squad manager online."));
}

void UBVSquadManager::Deinitialize()
{
	Squads.Empty();
	AlarmedSquads.Empty();
	Super::Deinitialize();
}

void UBVSquadManager::RegisterGuard(ABVAIController* Controller)
{
	if (!Controller) { return; }
	const ABVGuardCharacter* Guard = Controller->GetGuard();
	if (!Guard) { return; }

	const FName SquadID = Guard->SquadID;
	TArray<TWeakObjectPtr<ABVAIController>>& Members = Squads.FindOrAdd(SquadID);
	Members.AddUnique(Controller);

	UE_LOG(LogBlackVeil, Verbose, TEXT("Guard registered to squad %s (%d members)"),
		*SquadID.ToString(), Members.Num());
}

void UBVSquadManager::UnregisterGuard(ABVAIController* Controller)
{
	if (!Controller) { return; }
	for (TPair<FName, TArray<TWeakObjectPtr<ABVAIController>>>& Pair : Squads)
	{
		Pair.Value.Remove(Controller);
	}
}

TArray<ABVAIController*> UBVSquadManager::GetSquadMembers(FName SquadID) const
{
	TArray<ABVAIController*> Out;
	if (const TArray<TWeakObjectPtr<ABVAIController>>* Members = Squads.Find(SquadID))
	{
		for (const TWeakObjectPtr<ABVAIController>& Weak : *Members)
		{
			if (ABVAIController* C = Weak.Get()) { Out.Add(C); }
		}
	}
	return Out;
}

bool UBVSquadManager::IsSquadAlarmed(FName SquadID) const
{
	return AlarmedSquads.Contains(SquadID);
}

void UBVSquadManager::BroadcastContact(FName SquadID, const FVector& ApproxLocation, uint8 SuggestedAlertState, AActor* Spotter)
{
	// Teamwork governs how far a guard's report travels: low-teamwork guards
	// effectively shout less far, so distant members may miss it.
	const float BaseShareRadius = 4000.f;

	for (ABVAIController* Member : GetSquadMembers(SquadID))
	{
		if (!Member || Member == Spotter) { continue; }

		const ABVGuardCharacter* MemberGuard = Member->GetGuard();
		if (!MemberGuard || MemberGuard->IsIncapacitated()) { continue; }

		const float Dist = FVector::Dist(MemberGuard->GetActorLocation(), ApproxLocation);
		const float ShareRadius = BaseShareRadius * FMath::Lerp(0.5f, 1.5f, MemberGuard->Personality.Teamwork);
		if (Dist > ShareRadius) { continue; }

		Member->ReceiveSquadContact(ApproxLocation, SuggestedAlertState, Spotter);
	}
}

void UBVSquadManager::ReportGuardDown(ABVGuardCharacter* DownedGuard, const FVector& Location)
{
	if (!DownedGuard) { return; }

	const FName SquadID = DownedGuard->SquadID;
	OnGuardDownReported.Broadcast(SquadID, Location);

	// Everyone nearby becomes suspicious and will eventually notice the absence.
	BroadcastContact(SquadID, Location, static_cast<uint8>(EBVAlertState::Searching), DownedGuard);

	// Missing patrol partner: the squad searches the area around the body.
	CoordinateSearch(SquadID, Location, 1500.f);

	UE_LOG(LogBlackVeil, Warning, TEXT("Guard down in squad %s at %s"),
		*SquadID.ToString(), *Location.ToCompactString());
}

void UBVSquadManager::SetSquadAlarm(FName SquadID, bool bAlarmed, const FVector& AlarmLocation)
{
	if (bAlarmed) { AlarmedSquads.Add(SquadID); }
	else { AlarmedSquads.Remove(SquadID); }

	OnSquadAlarm.Broadcast(SquadID, bAlarmed, AlarmLocation);

	const uint8 TargetState = static_cast<uint8>(bAlarmed ? EBVAlertState::Alerted : EBVAlertState::Returning);
	BroadcastContact(SquadID, AlarmLocation, TargetState, nullptr);
}

void UBVSquadManager::CallReinforcements(FName SquadID, const FVector& Location)
{
	// The mission framework listens for the alarm broadcast and spawns the
	// actual reinforcement wave; this keeps spawning data-driven per level.
	SetSquadAlarm(SquadID, true, Location);

	UE_LOG(LogBlackVeil, Warning, TEXT("Reinforcements requested to %s"), *Location.ToCompactString());
}

void UBVSquadManager::CoordinateSearch(FName SquadID, const FVector& Center, float Radius)
{
	TArray<ABVAIController*> Members = GetSquadMembers(SquadID);
	Members.RemoveAll([](ABVAIController* C)
	{
		const ABVGuardCharacter* G = C ? C->GetGuard() : nullptr;
		return !G || G->IsIncapacitated();
	});

	if (Members.Num() == 0) { return; }

	// Fan the squad out around the contact point in evenly spaced sectors so
	// they cover ground instead of stacking up.
	const float AngleStep = 360.f / static_cast<float>(Members.Num());
	for (int32 i = 0; i < Members.Num(); ++i)
	{
		const float Angle = FMath::DegreesToRadians(AngleStep * i);
		const FVector Offset(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);
		const FVector SectorPoint = Center + Offset;

		Members[i]->ReceiveSquadContact(SectorPoint, static_cast<uint8>(EBVAlertState::Searching), nullptr);
	}
}
