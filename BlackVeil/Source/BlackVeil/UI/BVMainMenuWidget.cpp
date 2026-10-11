// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVMainMenuWidget.h"
#include "Core/BlackVeilGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		// A save "exists" once the player has progressed past mission 1.
		bHasSave = GI->CampaignData.HighestMissionUnlocked > 1
			|| GI->CampaignData.MissionScores.Num() > 0;
	}
	OnSaveStateResolved(bHasSave);
}

FText UBVMainMenuWidget::GetContinueLabel() const
{
	if (!bHasSave)
	{
		return NSLOCTEXT("BlackVeil", "NoSave", "No Saved Campaign");
	}

	if (const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		return FText::FromString(FString::Printf(TEXT("Continue - Mission %d"),
			FMath::Max(1, GI->CampaignData.HighestMissionUnlocked)));
	}
	return NSLOCTEXT("BlackVeil", "Continue", "Continue");
}

void UBVMainMenuWidget::StartNewGame()
{
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->ResetCampaign();
		GI->SetCurrentMission(1);
	}
	// The briefing screen owns equipment selection before launch.
	UGameplayStatics::OpenLevel(this, FName(TEXT("Briefing")), false);
}

void UBVMainMenuWidget::ContinueCampaign()
{
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->SetCurrentMission(FMath::Max(1, GI->CampaignData.HighestMissionUnlocked));
	}
	UGameplayStatics::OpenLevel(this, FName(TEXT("Briefing")), false);
}

void UBVMainMenuWidget::OpenMissionSelect()
{
	// Mission select is a widget swap inside the menu level.
	OnSaveStateResolved(bHasSave);
}

void UBVMainMenuWidget::OpenOptions() {}

void UBVMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
