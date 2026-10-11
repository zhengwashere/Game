// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "BlackVeilGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlaystyle : uint8
{
	Ghost		UMETA(DisplayName = "Ghost"),
	Panther		UMETA(DisplayName = "Panther"),
	Assault		UMETA(DisplayName = "Assault"),
	Adaptive	UMETA(DisplayName = "Adaptive")
};

UENUM(BlueprintType)
enum class EDifficulty : uint8
{
	Recruit		UMETA(DisplayName = "Recruit"),
	Agent		UMETA(DisplayName = "Agent"),
	Operative	UMETA(DisplayName = "Operative"),
	Perfectionist UMETA(DisplayName = "Perfectionist")
};

USTRUCT(BlueprintType)
struct FMissionScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) int32 MissionIndex = 0;
	UPROPERTY(BlueprintReadWrite) int32 Detections = 0;
	UPROPERTY(BlueprintReadWrite) int32 Alarms = 0;
	UPROPERTY(BlueprintReadWrite) int32 Takedowns = 0;
	UPROPERTY(BlueprintReadWrite) int32 Kills = 0;
	UPROPERTY(BlueprintReadWrite) int32 IntelCollected = 0;
	UPROPERTY(BlueprintReadWrite) int32 OptionalObjectivesCompleted = 0;
	UPROPERTY(BlueprintReadWrite) float TimeSeconds = 0.f;
	UPROPERTY(BlueprintReadWrite) FString Grade; // S, A, B, C, D
	UPROPERTY(BlueprintReadWrite) EPlaystyle DetectedPlaystyle = EPlaystyle::Adaptive;
};

USTRUCT(BlueprintType)
struct FUnlockedEquipment
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) TArray<FName> UnlockedGadgets;
	UPROPERTY(BlueprintReadWrite) TArray<FName> UnlockedWeaponUpgrades;
	UPROPERTY(BlueprintReadWrite) TArray<FName> UnlockedAbilities;
};

USTRUCT(BlueprintType)
struct FCampaignSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) int32 HighestMissionUnlocked = 1;
	UPROPERTY(BlueprintReadWrite) TArray<FMissionScore> MissionScores;
	UPROPERTY(BlueprintReadWrite) FUnlockedEquipment UnlockedEquipment;
	UPROPERTY(BlueprintReadWrite) TArray<FName> CollectedIntelItems;
	UPROPERTY(BlueprintReadWrite) EDifficulty GlobalDifficulty = EDifficulty::Agent;
};

UCLASS()
class BLACKVEIL_API UBlackVeilGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UBlackVeilGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	// Campaign Save/Load
	UFUNCTION(BlueprintCallable, Category = "Campaign")
	void SaveCampaign();

	UFUNCTION(BlueprintCallable, Category = "Campaign")
	bool LoadCampaign();

	UFUNCTION(BlueprintCallable, Category = "Campaign")
	void ResetCampaign();

	// Mission Management
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void SetCurrentMission(int32 MissionIndex);

	UFUNCTION(BlueprintPure, Category = "Mission")
	int32 GetCurrentMissionIndex() const { return CurrentMissionIndex; }

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void RecordMissionScore(const FMissionScore& Score);

	UFUNCTION(BlueprintPure, Category = "Mission")
	FMissionScore GetMissionScore(int32 MissionIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	FString CalculateGrade(const FMissionScore& Score) const;

	// Equipment
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnlockGadget(FName GadgetID);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsGadgetUnlocked(FName GadgetID) const;

	// Intel
	UFUNCTION(BlueprintCallable, Category = "Intel")
	void CollectIntel(FName IntelID);

	UFUNCTION(BlueprintPure, Category = "Intel")
	bool IsIntelCollected(FName IntelID) const;

	// Playstyle Tracking
	UFUNCTION(BlueprintCallable, Category = "Playstyle")
	void RecordDetection();

	UFUNCTION(BlueprintCallable, Category = "Playstyle")
	void RecordAlarm();

	UFUNCTION(BlueprintCallable, Category = "Playstyle")
	void RecordTakedown(bool bLethal);

	UPROPERTY(BlueprintReadWrite, Category = "Campaign")
	FCampaignSaveData CampaignData;

	UPROPERTY(BlueprintReadWrite, Category = "Mission")
	FMissionScore CurrentMissionScore;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	EDifficulty CurrentDifficulty = EDifficulty::Agent;

private:
	int32 CurrentMissionIndex = 1;
	static const FString SaveSlotName;
	static const int32 SaveUserIndex = 0;
};
