// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVMissionManager.h"
#include "BVCheckpoint.h"
#include "BVObjectiveVolume.h"
#include "Core/BlackVeilGameInstance.h"
#include "Core/BVGameMode.h"
#include "AI/BVGuardCharacter.h"
#include "AI/BVPatrolComponent.h"
#include "AI/BVPatrolRoute.h"
#include "Gadgets/BVEquipmentInventory.h"
#include "Player/BVPlayerCharacter.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "BlackVeil.h"

void UBVMissionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogBlackVeil, Log, TEXT("Mission manager initialized."));
}

void UBVMissionManager::Deinitialize()
{
	ObjectiveStates.Empty();
	ObjectiveCounts.Empty();
	Checkpoints.Empty();
	ObjectiveVolumes.Empty();
	Super::Deinitialize();
}

void UBVMissionManager::LoadMission(int32 MissionIndex)
{
	// Mission assets live under /Game/Data/Missions/DA_Mission_XX.
	const FString Path = FString::Printf(TEXT("/Game/Data/Missions/DA_Mission_%02d.DA_Mission_%02d"), MissionIndex, MissionIndex);
	CurrentMission = LoadObject<UBVMissionData>(nullptr, *Path);

	if (!CurrentMission)
	{
		UE_LOG(LogBlackVeil, Warning, TEXT("No mission data asset found at %s; running with an empty objective set."), *Path);
		return;
	}

	ObjectiveStates.Empty();
	ObjectiveCounts.Empty();

	// Seed runtime state from the authored objectives.
	for (const FBVObjectiveData& Obj : CurrentMission->Objectives)
	{
		FBVObjectiveDisplayData Display;
		Display.ObjectiveID = Obj.ObjectiveID;
		Display.Description = Obj.Description;
		Display.Type = Obj.Type;
		Display.bIsOptional = Obj.bIsOptional;
		Display.RequiredCount = Obj.RequiredCount;
		Display.bHasLocationMarker = Obj.bHasLocationMarker;
		Display.WorldLocation = Obj.WorldLocation;
		Display.State = EBVObjectiveState::Inactive;

		ObjectiveStates.Add(Obj.ObjectiveID, Display);
		ObjectiveCounts.Add(Obj.ObjectiveID, 0);
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Mission %d loaded: %s"), MissionIndex, *CurrentMission->MissionName.ToString());
}

void UBVMissionManager::StartMission()
{
	bMissionActive = true;
	ElapsedTime = 0.f;

	if (!CurrentMission) { return; }

	// Activate every objective with no prerequisites.
	for (const FBVObjectiveData& Obj : CurrentMission->Objectives)
	{
		if (Obj.Prerequisites.Num() == 0)
		{
			ActivateObjective(Obj.ObjectiveID);
		}
	}

	// Build the level's enemy force and hand the player their kit.
	SpawnMissionEnemies();
	ApplyPlayerLoadout();

	RefreshAllObjectives();
}

void UBVMissionManager::ApplyPlayerLoadout()
{
	UWorld* World = GetWorld();
	if (!World || !CurrentMission) { return; }

	APlayerController* PC = World->GetFirstPlayerController();
	ABVPlayerCharacter* Player = PC ? Cast<ABVPlayerCharacter>(PC->GetPawn()) : nullptr;
	UBVEquipmentInventory* Inventory = Player ? Player->GetEquipmentInventory() : nullptr;
	if (!Inventory) { return; }

	Inventory->SetupLoadout(CurrentMission->StartingGadgets, CurrentMission->StartingWeapons);

	UE_LOG(LogBlackVeil, Log, TEXT("Loadout applied: %d gadgets, %d weapons"),
		CurrentMission->StartingGadgets.Num(), CurrentMission->StartingWeapons.Num());
}

void UBVMissionManager::SpawnMissionEnemies()
{
	UWorld* World = GetWorld();
	if (!World || !CurrentMission) { return; }

	// Difficulty scales how many of the authored guards actually spawn.
	const float CountMult = CurrentMission->Difficulty.EnemyCountMultiplier;
	const int32 Total = CurrentMission->EnemyComposition.Num();
	const int32 ToSpawn = FMath::Clamp(FMath::RoundToInt(Total * CountMult), 1, Total);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	int32 Spawned = 0;
	for (int32 i = 0; i < ToSpawn; ++i)
	{
		const FBVEnemySpawnConfig& Config = CurrentMission->EnemyComposition[i];

		ABVGuardCharacter* Guard = World->SpawnActor<ABVGuardCharacter>(
			ABVGuardCharacter::StaticClass(), Config.Location, Config.Rotation, Params);
		if (!Guard) { continue; }

		Guard->GuardType = static_cast<EBVGuardType>(Config.GuardTypeOrdinal);
		Guard->Personality = Config.Personality;
		Guard->SquadID = Config.SquadID.IsNone() ? FName(TEXT("Squad_A")) : Config.SquadID;
		Guard->PatrolRouteID = Config.PatrolRouteID;

		// Wire the authored route onto the guard's patrol component.
		if (!Config.PatrolRouteID.IsNone())
		{
			for (TActorIterator<ABVPatrolRoute> It(World); It; ++It)
			{
				ABVPatrolRoute* Route = *It;
				if (Route && Route->RouteID == Config.PatrolRouteID)
				{
					Route->ApplyToPatrolComponent(Guard->GetPatrolComponent());
					break;
				}
			}
		}

		++Spawned;
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Spawned %d/%d guards for mission %d"), Spawned, Total, GetMissionIndex());
}

void UBVMissionManager::ActivateObjective(FName ObjectiveID)
{
	if (FBVObjectiveDisplayData* State = ObjectiveStates.Find(ObjectiveID))
	{
		if (State->State == EBVObjectiveState::Inactive)
		{
			State->State = EBVObjectiveState::Active;
			OnObjectiveUpdated.Broadcast(*State);
		}
	}
}

void UBVMissionManager::AddObjectiveProgress(FName ObjectiveID, int32 Amount)
{
	FBVObjectiveDisplayData* State = ObjectiveStates.Find(ObjectiveID);
	if (!State || State->State == EBVObjectiveState::Completed || State->State == EBVObjectiveState::Failed)
	{
		return;
	}

	if (State->State == EBVObjectiveState::Inactive)
	{
		ActivateObjective(ObjectiveID);
		State = ObjectiveStates.Find(ObjectiveID);
	}

	int32& Count = ObjectiveCounts.FindOrAdd(ObjectiveID);
	Count += Amount;
	State->CurrentCount = Count;

	OnObjectiveUpdated.Broadcast(*State);

	if (Count >= State->RequiredCount)
	{
		CompleteObjective(ObjectiveID);
	}
}

void UBVMissionManager::CompleteObjective(FName ObjectiveID)
{
	FBVObjectiveDisplayData* State = ObjectiveStates.Find(ObjectiveID);
	if (!State || State->State == EBVObjectiveState::Completed) { return; }

	State->State = EBVObjectiveState::Completed;
	OnObjectiveUpdated.Broadcast(*State);

	UE_LOG(LogBlackVeil, Log, TEXT("Objective complete: %s"), *ObjectiveID.ToString());

	// Unlock any objectives that were waiting on this one.
	if (CurrentMission)
	{
		for (const FBVObjectiveData& Obj : CurrentMission->Objectives)
		{
			if (Obj.Prerequisites.Contains(ObjectiveID))
			{
				bool bAllMet = true;
				for (const FName& Prereq : Obj.Prerequisites)
				{
					const EBVObjectiveState S = GetObjectiveState(Prereq);
					if (S != EBVObjectiveState::Completed) { bAllMet = false; break; }
				}
				if (bAllMet) { ActivateObjective(Obj.ObjectiveID); }
			}
		}
	}

	EvaluateMissionCompletion();
}

void UBVMissionManager::FailObjective(FName ObjectiveID)
{
	if (FBVObjectiveDisplayData* State = ObjectiveStates.Find(ObjectiveID))
	{
		State->State = EBVObjectiveState::Failed;
		OnObjectiveUpdated.Broadcast(*State);

		if (!State->bIsOptional)
		{
			FailMission(FText::FromString(TEXT("A required objective was failed.")));
		}
	}
}

EBVObjectiveState UBVMissionManager::GetObjectiveState(FName ObjectiveID) const
{
	if (const FBVObjectiveDisplayData* State = ObjectiveStates.Find(ObjectiveID))
	{
		return State->State;
	}
	return EBVObjectiveState::Inactive;
}

TArray<FBVObjectiveDisplayData> UBVMissionManager::GetObjectiveDisplayData() const
{
	TArray<FBVObjectiveDisplayData> Out;
	ObjectiveStates.GenerateValueArray(Out);
	return Out;
}

bool UBVMissionManager::AreMandatoryObjectivesComplete() const
{
	for (const TPair<FName, FBVObjectiveDisplayData>& Pair : ObjectiveStates)
	{
		if (!Pair.Value.bIsOptional && Pair.Value.State != EBVObjectiveState::Completed)
		{
			return false;
		}
	}
	return ObjectiveStates.Num() > 0;
}

void UBVMissionManager::EvaluateMissionCompletion()
{
	if (bMissionActive && AreMandatoryObjectivesComplete())
	{
		CompleteMission();
	}
}

void UBVMissionManager::CollectIntel(FName IntelID)
{
	if (UBlackVeilGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UBlackVeilGameInstance>() : nullptr)
	{
		GI->CollectIntel(IntelID);
	}
	OnIntelCollected.Broadcast(IntelID);

	// Intel can also satisfy a retrieve-style objective.
	if (ObjectiveStates.Contains(IntelID))
	{
		CompleteObjective(IntelID);
	}
}

void UBVMissionManager::RegisterCheckpoint(ABVCheckpoint* Checkpoint)
{
	if (Checkpoint && !Checkpoints.Contains(Checkpoint))
	{
		Checkpoints.Add(Checkpoint);
	}
}

void UBVMissionManager::TriggerCheckpoint(FName CheckpointID, ABVCheckpoint* Source)
{
	LastCheckpointID = CheckpointID;
	LastCheckpoint = Source;

	UE_LOG(LogBlackVeil, Log, TEXT("Checkpoint reached: %s"), *CheckpointID.ToString());
}

void UBVMissionManager::RegisterObjectiveVolume(ABVObjectiveVolume* Volume)
{
	if (Volume && !ObjectiveVolumes.Contains(Volume))
	{
		ObjectiveVolumes.Add(Volume);
	}
}

bool UBVMissionManager::RestoreLastCheckpoint()
{
	if (!LastCheckpoint)
	{
		return false;
	}

	LastCheckpoint->RestoreCheckpoint();
	OnCheckpointRestored.Broadcast(LastCheckpointID);

	bMissionActive = true;
	return true;
}

void UBVMissionManager::RefreshAllObjectives()
{
	OnObjectivesRefreshed.Broadcast(GetObjectiveDisplayData());
}

void UBVMissionManager::UnlockRewards(const FMissionScore& Score)
{
	if (!CurrentMission) { return; }

	UBlackVeilGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UBlackVeilGameInstance>() : nullptr;
	if (!GI) { return; }

	// Grade-gated unlocks: strong performances unlock more equipment.
	const bool bGoodGrade = Score.Grade == TEXT("S") || Score.Grade == TEXT("A");

	for (const FName& Reward : CurrentMission->RewardGadgets)
	{
		GI->UnlockGadget(Reward);
	}

	if (bGoodGrade)
	{
		// Extra reward for flawless play is authored per mission as an S-rank item.
		const FName SRankReward(*FString::Printf(TEXT("SRank_Mission%d"), CurrentMission->MissionIndex));
		GI->UnlockGadget(SRankReward);
	}
}

void UBVMissionManager::TickMission(float DeltaSeconds)
{
	if (bMissionActive) { ElapsedTime += DeltaSeconds; }
}

void UBVMissionManager::CompleteMission()
{
	if (!bMissionActive) { return; }
	bMissionActive = false;

	UBlackVeilGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UBlackVeilGameInstance>() : nullptr;

	FMissionScore Score;
	Score.MissionIndex = GetMissionIndex();
	Score.TimeSeconds = ElapsedTime;

	if (GI)
	{
		// Pull the live tallies the game instance has been accumulating.
		Score.Detections = GI->CurrentMissionScore.Detections;
		Score.Alarms = GI->CurrentMissionScore.Alarms;
		Score.Takedowns = GI->CurrentMissionScore.Takedowns;
		Score.Kills = GI->CurrentMissionScore.Kills;
		Score.IntelCollected = GI->CurrentMissionScore.IntelCollected;

		// Optional objective tally.
		for (const TPair<FName, FBVObjectiveDisplayData>& Pair : ObjectiveStates)
		{
			if (Pair.Value.bIsOptional && Pair.Value.State == EBVObjectiveState::Completed)
			{
				Score.OptionalObjectivesCompleted++;
			}
		}

		Score.Grade = GI->CalculateGrade(Score);
		GI->RecordMissionScore(Score);
	}

	UnlockRewards(Score);
	OnMissionCompleted.Broadcast(Score);

	UE_LOG(LogBlackVeil, Log, TEXT("Mission %d complete. Grade %s"), Score.MissionIndex, *Score.Grade);
}

void UBVMissionManager::FailMission(FText Reason)
{
	if (!bMissionActive) { return; }
	bMissionActive = false;

	OnMissionFailed.Broadcast(EBVFailureReason::ObjectiveFailed, Reason);
	UE_LOG(LogBlackVeil, Warning, TEXT("Mission failed: %s"), *Reason.ToString());
}
