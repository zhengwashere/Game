// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BlackVeilGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/BlackVeilSaveGame.h"

const FString UBlackVeilGameInstance::SaveSlotName = TEXT("BlackVeilCampaign");

UBlackVeilGameInstance::UBlackVeilGameInstance()
{
	// Default equipment available from mission 1
	CampaignData.UnlockedEquipment.UnlockedGadgets = {
		TEXT("Lockpick"),
		TEXT("NoiseMaker")
	};
}

void UBlackVeilGameInstance::Init()
{
	Super::Init();
	LoadCampaign();
}

void UBlackVeilGameInstance::Shutdown()
{
	SaveCampaign();
	Super::Shutdown();
}

void UBlackVeilGameInstance::SaveCampaign()
{
	UBlackVeilSaveGame* SaveGame = Cast<UBlackVeilSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UBlackVeilSaveGame::StaticClass()));
	if (SaveGame)
	{
		SaveGame->CampaignData = CampaignData;
		UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, SaveUserIndex);
	}
}

bool UBlackVeilGameInstance::LoadCampaign()
{
	UBlackVeilSaveGame* SaveGame = Cast<UBlackVeilSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	if (SaveGame)
	{
		CampaignData = SaveGame->CampaignData;
		return true;
	}
	// Initialize fresh save with starting equipment
	CampaignData.HighestMissionUnlocked = 1;
	CampaignData.UnlockedEquipment.UnlockedGadgets = {
		TEXT("Lockpick"),
		TEXT("NoiseMaker")
	};
	return false;
}

void UBlackVeilGameInstance::ResetCampaign()
{
	UGameplayStatics::DeleteGameInSlot(SaveSlotName, SaveUserIndex);
	CampaignData = FCampaignSaveData();
	CampaignData.UnlockedEquipment.UnlockedGadgets = {
		TEXT("Lockpick"),
		TEXT("NoiseMaker")
	};
}

void UBlackVeilGameInstance::SetCurrentMission(int32 MissionIndex)
{
	CurrentMissionIndex = MissionIndex;
	// Reset current session score
	CurrentMissionScore = FMissionScore();
	CurrentMissionScore.MissionIndex = MissionIndex;
}

void UBlackVeilGameInstance::RecordMissionScore(const FMissionScore& Score)
{
	// Update or add score
	bool bFound = false;
	for (FMissionScore& Existing : CampaignData.MissionScores)
	{
		if (Existing.MissionIndex == Score.MissionIndex)
		{
			// Keep best grade
			if (Score.Grade < Existing.Grade) // 'S' < 'A' in ASCII - handle properly
			{
				Existing = Score;
			}
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		CampaignData.MissionScores.Add(Score);
	}

	// Unlock next mission
	if (Score.MissionIndex >= CampaignData.HighestMissionUnlocked)
	{
		CampaignData.HighestMissionUnlocked = FMath::Min(Score.MissionIndex + 1, 12);
	}

	SaveCampaign();
}

FMissionScore UBlackVeilGameInstance::GetMissionScore(int32 MissionIndex) const
{
	for (const FMissionScore& Score : CampaignData.MissionScores)
	{
		if (Score.MissionIndex == MissionIndex)
			return Score;
	}
	return FMissionScore();
}

FString UBlackVeilGameInstance::CalculateGrade(const FMissionScore& Score) const
{
	// Grade calculation based on performance
	int32 Points = 100;
	Points -= Score.Detections * 15;
	Points -= Score.Alarms * 25;
	Points -= Score.Kills * 10;
	Points += Score.IntelCollected * 5;
	Points += Score.OptionalObjectivesCompleted * 10;

	if (Points >= 95) return TEXT("S");
	if (Points >= 80) return TEXT("A");
	if (Points >= 65) return TEXT("B");
	if (Points >= 50) return TEXT("C");
	return TEXT("D");
}

void UBlackVeilGameInstance::UnlockGadget(FName GadgetID)
{
	if (!CampaignData.UnlockedEquipment.UnlockedGadgets.Contains(GadgetID))
	{
		CampaignData.UnlockedEquipment.UnlockedGadgets.Add(GadgetID);
		SaveCampaign();
	}
}

bool UBlackVeilGameInstance::IsGadgetUnlocked(FName GadgetID) const
{
	return CampaignData.UnlockedEquipment.UnlockedGadgets.Contains(GadgetID);
}

void UBlackVeilGameInstance::CollectIntel(FName IntelID)
{
	if (!CampaignData.CollectedIntelItems.Contains(IntelID))
	{
		CampaignData.CollectedIntelItems.Add(IntelID);
		CurrentMissionScore.IntelCollected++;
		SaveCampaign();
	}
}

bool UBlackVeilGameInstance::IsIntelCollected(FName IntelID) const
{
	return CampaignData.CollectedIntelItems.Contains(IntelID);
}

void UBlackVeilGameInstance::RecordDetection()
{
	CurrentMissionScore.Detections++;
}

void UBlackVeilGameInstance::RecordAlarm()
{
	CurrentMissionScore.Alarms++;
}

void UBlackVeilGameInstance::RecordTakedown(bool bLethal)
{
	CurrentMissionScore.Takedowns++;
	if (bLethal) CurrentMissionScore.Kills++;
}
