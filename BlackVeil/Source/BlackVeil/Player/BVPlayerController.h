// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BVPlayerController.generated.h"

class ABVPlayerCharacter;
class ABVHUDBase;
class UBVPauseMenuWidget;

/**
 * Owns the player's HUD, pause state and camera view target.
 * Keeps the character focused on gameplay while this class handles
 * presentation and input mode transitions.
 */
UCLASS()
class BLACKVEIL_API ABVPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABVPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Controller")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Controller")
	void ShowPauseMenu(bool bShow);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Controller")
	bool IsPauseMenuVisible() const { return bPauseMenuVisible; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Controller")
	ABVHUDBase* GetBlackVeilHUD() const;

	/** Blueprint hook for mission-specific camera intro sequences. */
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Controller")
	void OnMissionIntro();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|UI") TSubclassOf<UBVPauseMenuWidget> PauseMenuClass;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|UI") TObjectPtr<UBVPauseMenuWidget> PauseMenuWidget;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|UI") bool bPauseMenuVisible = false;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Player") TWeakObjectPtr<ABVPlayerCharacter> BVCharacter;

	UFUNCTION() void HandleSpeedChanged(EBVMovementSpeed NewSpeed, EBVNoiseLevel Noise);
};
