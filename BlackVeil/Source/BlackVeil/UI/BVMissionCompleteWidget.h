// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BlackVeilGameInstance.h"
#include "BVMissionCompleteWidget.generated.h"

/** Post-mission results: grade, stats breakdown and next actions. */
UCLASS(Abstract)
class BLACKVEIL_API UBVMissionCompleteWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void SetupFromScore(const FMissionScore& InScore);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") const FMissionScore& GetScore() const { return Score; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") FText GetGradeText() const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") FText GetTimeText() const;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void OnContinueClicked();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void OnReplayClicked();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void OnMainMenuClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnScoreReady(const FMissionScore& InScore);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") FMissionScore Score;
};
