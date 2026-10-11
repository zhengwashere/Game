// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BVPauseMenuWidget.generated.h"

UCLASS(Abstract)
class BLACKVEIL_API UBVPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void ResumeGame();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void RestartFromCheckpoint();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void RestartMission();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void ReturnToMainMenu();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void QuitGame();

	/** Volume sliders (0..1). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void SetMasterVolume(float Value);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void SetMusicVolume(float Value);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void SetSFXVolume(float Value);
};
