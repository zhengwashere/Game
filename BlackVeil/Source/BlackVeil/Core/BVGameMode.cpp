// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGameMode.h"
#include "BlackVeilGameInstance.h"
#include "Mission/BVMissionManager.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVPlayerController.h"
#include "UI/BVHUDBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "AI/BVGuardCharacter.h"
#include "AI/BVAlertStateComponent.h"
#include "AI/BVSquadManager.h"
#include "EngineUtils.h"
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

	UWorld* World = GetWorld();
	if (!World) { return; }

	// Keep the mission clock running and mirror squad alert state onto the
	// game mode so the HUD and audio can react to it.
	if (UBVMissionManager* MissionManager = World->GetSubsystem<UBVMissionManager>())
	{
		MissionManager->TickMission(DeltaSeconds);
	}

	if (UBVSquadManager* Squad = World->GetSubsystem<UBVSquadManager>())
	{
		// Sample the worst alert state across all registered guards by asking the
		// player's combat contact; here we simply promote to Combat when any guard
		is in the combat state.
		EBVGlobalAlertLevel Worst = EBVGlobalAlertLevel::Normal;
		for (TActorIterator<ABVGuardCharacter> It(World); It; ++It)
		{
			ABVGuardCharacter* Guard = *It;
			if (!Guard || Guard->IsIncapacitated()) { continue; }
			const UBVAlertStateComponent* Alert = Guard->GetAlertStateComponent();
			if (!Alert) { continue; }

			switch (Alert->GetAlertState())
			{
			case EBVAlertState::Combat:        Worst = EBVGlobalAlertLevel::Combat; break;
			case EBVAlertState::Alerted:
				if (Worst < EBVGlobalAlertLevel::Alarmed) { Worst = EBVGlobalAlertLevel::Alarmed; }
				break;
			case EBVAlertState::Searching:
			case EBVAlertState::Investigating:
				if (Worst < EBVGlobalAlertLevel::Searching) { Worst = EBVGlobalAlertLevel::Searching; }
				break;
			case EBVAlertState::Suspicious:
				if (Worst < EBVGlobalAlertLevel::Suspicious) { Worst = EBVGlobalAlertLevel::Suspicious; }
				break;
			default: break;
			}
		}
		SetGlobalAlertLevel(Worst);
	}
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
