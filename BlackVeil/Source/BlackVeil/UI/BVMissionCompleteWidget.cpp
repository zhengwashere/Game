// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVMissionCompleteWidget.h"
#include "Core/BVGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVMissionCompleteWidget::SetupFromScore(const FMissionScore& InScore)
{
	Score = InScore;
	OnScoreReady(Score);
}

FText UBVMissionCompleteWidget::GetGradeText() const
{
	return FText::FromString(Score.Grade.IsEmpty() ? TEXT("-") : Score.Grade);
}

FText UBVMissionCompleteWidget::GetTimeText() const
{
	const int32 Minutes = FMath::FloorToInt(Score.TimeSeconds / 60.f);
	const int32 Seconds = FMath::FloorToInt(Score.TimeSeconds) % 60;
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}

void UBVMissionCompleteWidget::OnContinueClicked()
{
	// Advance to the next mission if one is unlocked, otherwise return to menu.
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		const int32 Next = Score.MissionIndex + 1;
		if (Next <= 12 && Next <= GI->CampaignData.HighestMissionUnlocked)
		{
			GI->SetCurrentMission(Next);
			const FString LevelName = FString::Printf(TEXT("M%02d"), Next);
			UGameplayStatics::OpenLevel(this, FName(*LevelName));
			return;
		}
	}
	UGameplayStatics::OpenLevel(this, FName(TEXT("MainMenu")));
}

void UBVMissionCompleteWidget::OnReplayClicked()
{
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->SetCurrentMission(Score.MissionIndex);
	}
	const FString LevelName = FString::Printf(TEXT("M%02d"), Score.MissionIndex);
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}

void UBVMissionCompleteWidget::OnMainMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("MainMenu")));
}
