// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVMissionSelectWidget.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVMissionSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		SelectedDifficulty = GI->CurrentDifficulty;
	}
}

int32 UBVMissionSelectWidget::GetHighestUnlocked() const
{
	const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>();
	return GI ? GI->CampaignData.HighestMissionUnlocked : 1;
}

bool UBVMissionSelectWidget::IsMissionUnlocked(int32 MissionIndex) const
{
	return MissionIndex <= GetHighestUnlocked();
}

FString UBVMissionSelectWidget::GetMissionGrade(int32 MissionIndex) const
{
	const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>();
	if (!GI) { return TEXT("-"); }

	const FMissionScore Score = GI->GetMissionScore(MissionIndex);
	return Score.Grade.IsEmpty() ? TEXT("-") : Score.Grade;
}

bool UBVMissionSelectWidget::SelectMission(int32 MissionIndex)
{
	if (!IsMissionUnlocked(MissionIndex)) { return false; }

	SelectedMissionIndex = MissionIndex;
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->SetCurrentMission(MissionIndex);
		GI->CurrentDifficulty = SelectedDifficulty;
	}

	OnMissionSelected(MissionIndex, true, GetMissionGrade(MissionIndex));
	return true;
}

void UBVMissionSelectWidget::SetDifficulty(EDifficulty NewDifficulty)
{
	SelectedDifficulty = NewDifficulty;
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->CurrentDifficulty = NewDifficulty;
	}
}
