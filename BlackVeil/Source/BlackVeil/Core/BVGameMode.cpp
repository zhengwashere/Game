// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGameMode.h"
#include "BlackVeilGameInstance.h"
#include "Mission/BVMissionManager.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVPlayerController.h"
#include "UI/BVHUDBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVGameMode::ABVGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = ABVPlayerCharacter::StaticClass();
	PlayerControllerClass = ABVPlayerController::StaticClass();
	HUDClass = ABVHUDBase::StaticClass();
}

void ABVGameMode::BeginPlay()
{
	Super::BeginPlay();

	bMissionActive = true;

	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		MissionIndex = GI->GetCurrentMissionIndex();
	}

	// Hand control to the mission framework which loads the data asset,
	// spawns objectives, and arms the objective tracker.
	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MissionManager = World->GetSubsystem<UBVMissionManager>())
		{
			MissionManager->LoadMission(MissionIndex);
			MissionManager->StartMission();
		}
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Black Veil mission %d starting."), MissionIndex);
}

void ABVGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABVGameMode::OnMissionSucceeded()
{
	if (!bMissionActive)
	{
		return;
	}
	bMissionActive = false;

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MissionManager = World->GetSubsystem<UBVMissionManager>())
		{
			MissionManager->CompleteMission();
		}
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Mission %d succeeded."), MissionIndex);
}

void ABVGameMode::OnMissionFailed(EBVFailureReason Reason)
{
	if (!bMissionActive)
	{
		return;
	}
	bMissionActive = false;

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MissionManager = World->GetSubsystem<UBVMissionManager>())
		{
			MissionManager->FailMission(FText::FromString(TEXT("Mission failed.")));
		}
	}

	UE_LOG(LogBlackVeil, Warning, TEXT("Mission %d failed. Reason: %d"), MissionIndex, static_cast<int32>(Reason));
}

void ABVGameMode::RestartMission(bool bFromCheckpoint)
{
	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MissionManager = World->GetSubsystem<UBVMissionManager>())
		{
			if (bFromCheckpoint && MissionManager->RestoreLastCheckpoint())
			{
				bMissionActive = true;
				return;
			}
		}
	}

	// Full restart: reopen the same mission level.
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void ABVGameMode::SetGlobalAlertLevel(EBVGlobalAlertLevel NewLevel)
{
	if (GlobalAlertLevel != NewLevel)
	{
		GlobalAlertLevel = NewLevel;
		// Audio + HUD subscribe to this via the mission manager / game state.
	}
}
