// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/BVTypes.h"
#include "BVGameMode.generated.h"

class ABVPlayerCharacter;
class ABVGuardCharacter;
class UBVMissionManager;

/**
 * Base game mode. Concrete missions derive from this (or set it directly in
 * World Settings) and drive mission-specific setup through the mission data
 * asset rather than subclassing for every level.
 */
UCLASS()
class BLACKVEIL_API ABVGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABVGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Called by the mission manager when the level's objectives are all met. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|GameMode")
	void OnMissionSucceeded();

	/** Called when a failure condition is met (death, timer, lost asset...). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|GameMode")
	void OnMissionFailed(EBVFailureReason Reason);

	/** Restart the current mission from the last checkpoint or from the start. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|GameMode")
	void RestartMission(bool bFromCheckpoint);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|GameMode")
	bool IsMissionActive() const { return bMissionActive; }

	/** Mission index currently being played (1..12). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|GameMode")
	int32 MissionIndex = 1;

	/** Broadcast so HUD / audio can react to alarm level changes. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|GameMode")
	void SetGlobalAlertLevel(EBVGlobalAlertLevel NewLevel);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|GameMode")
	EBVGlobalAlertLevel GetGlobalAlertLevel() const { return GlobalAlertLevel; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|GameMode")
	bool bMissionActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|GameMode")
	EBVGlobalAlertLevel GlobalAlertLevel = EBVGlobalAlertLevel::Normal;
};
