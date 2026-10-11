// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/BVTypes.h"
#include "BVMissionData.generated.h"

/** A single objective authored for a mission. */
USTRUCT(BlueprintType)
struct FBVObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") FName ObjectiveID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") EBVObjectiveType Type = EBVObjectiveType::Custom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") bool bIsOptional = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") int32 RequiredCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") bool bHasLocationMarker = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") FVector WorldLocation = FVector::ZeroVector;
	/** Objectives that must be complete before this one becomes active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective") TArray<FName> Prerequisites;
};

/** Collectible intelligence discovered in a level. */
USTRUCT(BlueprintType)
struct FBVIntelItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intel") FName IntelID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intel") FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intel") FText Body;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intel") FName Category;
};

/** Spawn instruction for one guard in a mission. */
USTRUCT(BlueprintType)
struct FBVEnemySpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") FName SpawnID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") uint8 GuardTypeOrdinal = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") FName PatrolRouteID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") FName SquadID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy") FBVGuardPersonality Personality;
};

/** Named respawn point used by the checkpoint system. */
USTRUCT(BlueprintType)
struct FBVCheckpointConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint") FName CheckpointID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint") FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint") int32 Order = 0;
};

/** Difficulty knobs applied when a mission starts. */
USTRUCT(BlueprintType)
struct FBVMissionDifficultySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty") float EnemyCountMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty") float PerceptionMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty") float AlarmEscalationSpeed = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty") bool bNoCheckpoints = false;
};

/**
 * Authoring container for a single campaign mission.
 *
 * Designers fill these in the editor; the mission manager reads them at runtime
 * so adding a mission never requires touching gameplay code.
 */
UCLASS(BlueprintType)
class BLACKVEIL_API UBVMissionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") int32 MissionIndex = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") FText MissionName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") FText LocationName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") FText BriefingText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") FText DebriefingText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TSoftObjectPtr<UWorld> MapAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FBVObjectiveData> Objectives;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FBVIntelItem> IntelItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FBVEnemySpawnConfig> EnemyComposition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FBVCheckpointConfig> Checkpoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FName> StartingGadgets;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FName> StartingWeapons;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") TArray<FName> RewardGadgets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") FBVMissionDifficultySettings Difficulty;

	/** Target par time in seconds, used by the score screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission") float ParTimeSeconds = 900.f;
};
