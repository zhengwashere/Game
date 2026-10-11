// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVPauseMenuWidget.h"
#include "Player/BVPlayerController.h"
#include "Core/BVGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "BlackVeil.h"

void UBVPauseMenuWidget::ResumeGame()
{
	if (ABVPlayerController* PC = Cast<ABVPlayerController>(GetOwningPlayer()))
	{
		PC->ShowPauseMenu(false);
	}
}

void UBVPauseMenuWidget::RestartFromCheckpoint()
{
	if (ABVPlayerController* PC = Cast<ABVPlayerController>(GetOwningPlayer()))
	{
		PC->ShowPauseMenu(false);
	}
	if (ABVGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ABVGameMode>() : nullptr)
	{
		GM->RestartMission(true);
	}
}

void UBVPauseMenuWidget::RestartMission()
{
	if (ABVPlayerController* PC = Cast<ABVPlayerController>(GetOwningPlayer()))
	{
		PC->ShowPauseMenu(false);
	}
	if (ABVGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ABVGameMode>() : nullptr)
	{
		GM->RestartMission(false);
	}
}

void UBVPauseMenuWidget::ReturnToMainMenu()
{
	if (GetWorld())
	{
		GetWorld()->GetFirstPlayerController()->SetPause(false);
	}
	UGameplayStatics::OpenLevel(this, FName(TEXT("MainMenu")));
}

void UBVPauseMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UBVPauseMenuWidget::SetMasterVolume(float Value)
{
	// Volume is applied through the audio manager's sound class hierarchy.
}

void UBVPauseMenuWidget::SetMusicVolume(float Value) {}
void UBVPauseMenuWidget::SetSFXVolume(float Value) {}
