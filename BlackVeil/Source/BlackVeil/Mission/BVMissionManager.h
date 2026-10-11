// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/BVTypes.h"
#include "Core/BlackVeilGameInstance.h"
#include "BVMissionData.h"
#include "BVMissionManager.generated.h"

class ABVCheckpoint;
class ABVObjectiveVolume;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnObjectiveUpdated, const FBVObjectiveDisplayData&, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnObjectivesRefreshed, const TArray<FBVObjectiveDisplayData>&, AllObjectives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnMissionCompleted, const FMissionScore&, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnMissionFailed, EBVFailureReason, Reason, FText, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnCheckpointRestored, FName, CheckpointID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnIntelCollected, FName, IntelID);

/**
 * Runtime brain of a mission.
 *
 * Loads the mission data asset, activates objectives, tracks their progress,
 * handles checkpoints and failure, and produces the final score. All mission
 * logic lives here and in the objective volumes placed in the level, so a new
 * mission is authored, not coded.
 */
UCLASS()
class BLACKVEIL_API UBVMissionManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ---- Lifecycle ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void LoadMission(int32 MissionIndex);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void StartMission();

	/** Spawns the mission's guards from the data asset and wires up their routes. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void SpawnMissionEnemies();

	/** Applies the mission's starting equipment to the player's inventory. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void ApplyPlayerLoadout();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void CompleteMission();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void FailMission(FText Reason);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") UBVMissionData* GetCurrentMissionData() const { return CurrentMission; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") int32 GetMissionIndex() const { return CurrentMission ? CurrentMission->MissionIndex : 0; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") bool IsMissionActive() const { return bMissionActive; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") float GetElapsedTime() const { return ElapsedTime; }

	/** Advanced by the game mode each frame to keep the mission clock. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void TickMission(float DeltaSeconds);

	// ---- Objectives ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void ActivateObjective(FName ObjectiveID);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void AddObjectiveProgress(FName ObjectiveID, int32 Amount = 1);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void CompleteObjective(FName ObjectiveID);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void FailObjective(FName ObjectiveID);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") EBVObjectiveState GetObjectiveState(FName ObjectiveID) const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") TArray<FBVObjectiveDisplayData> GetObjectiveDisplayData() const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Mission") bool AreMandatoryObjectivesComplete() const;

	// ---- Intel ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void CollectIntel(FName IntelID);

	// ---- Checkpoints ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void RegisterCheckpoint(ABVCheckpoint* Checkpoint);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void TriggerCheckpoint(FName CheckpointID, ABVCheckpoint* Source);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") bool RestoreLastCheckpoint();

	/** Objective volumes register themselves so progress events can find them. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Mission") void RegisterObjectiveVolume(ABVObjectiveVolume* Volume);

	// ---- Events ----
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Mission") FBVOnObjectiveUpdated OnObjectiveUpdated;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Mission") FBVOnObjectivesRefreshed OnObjectivesRefreshed;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Mission") FBVOnMissionCompleted OnMissionCompleted;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Mission") FBVOnMissionFailed OnMissionFailed;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Mission") FBVOnCheckpointRestored OnCheckpointRestored;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Mission") FBVOnIntelCollected OnIntelCollected;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Mission") TObjectPtr<UBVMissionData> CurrentMission;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Mission") bool bMissionActive = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Mission") float ElapsedTime = 0.f;

	/** ObjectiveID -> live state. */
	UPROPERTY() TMap<FName, FBVObjectiveDisplayData> ObjectiveStates;
	UPROPERTY() TMap<FName, int32> ObjectiveCounts;

	UPROPERTY() TArray<TObjectPtr<ABVCheckpoint>> Checkpoints;
	UPROPERTY() TArray<TObjectPtr<ABVObjectiveVolume>> ObjectiveVolumes;

	UPROPERTY() FName LastCheckpointID;
	UPROPERTY() TObjectPtr<ABVCheckpoint> LastCheckpoint;

private:
	void RefreshAllObjectives();
	void EvaluateMissionCompletion();
	void UnlockRewards(const FMissionScore& Score);
};
