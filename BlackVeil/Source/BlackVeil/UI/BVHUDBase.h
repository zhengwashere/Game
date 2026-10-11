// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/BVTypes.h"
#include "Core/BlackVeilGameInstance.h"
#include "BVHUDBase.generated.h"

class UBVStealthHUDWidget;
class UBVObjectiveTrackerWidget;
class UBVAlertIndicatorWidget;
class UBVEquipmentHUDWidget;
class UBVSubtitleWidget;
class UBVMissionCompleteWidget;

/**
 * Master HUD. Owns every gameplay widget, shows and hides them by game state,
 * and routes mission/alert events to the relevant widget.
 */
UCLASS()
class BLACKVEIL_API ABVHUDBase : public AHUD
{
	GENERATED_BODY()

public:
	ABVHUDBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void ShowGameplayHUD(bool bShow);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void ShowMissionCompleteScreen(const FMissionScore& Score);

	/** Flashes the detection warning on the stealth widget. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void NotifyPlayerDetected();

	/** Updates the alert indicator from the world's worst alert level. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void SetAlertLevel(EBVGlobalAlertLevel Level);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") UBVStealthHUDWidget* GetStealthWidget() const { return StealthWidget; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") UBVEquipmentHUDWidget* GetEquipmentWidget() const { return EquipmentWidget; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|HUD") TSubclassOf<UBVStealthHUDWidget> StealthWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|HUD") TSubclassOf<UBVObjectiveTrackerWidget> ObjectiveWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|HUD") TSubclassOf<UBVAlertIndicatorWidget> AlertWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|HUD") TSubclassOf<UBVEquipmentHUDWidget> EquipmentWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|HUD") TSubclassOf<UBVSubtitleWidget> SubtitleWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|HUD") TSubclassOf<UBVMissionCompleteWidget> MissionCompleteWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TObjectPtr<UBVStealthHUDWidget> StealthWidget;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TObjectPtr<UBVObjectiveTrackerWidget> ObjectiveWidget;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TObjectPtr<UBVAlertIndicatorWidget> AlertWidget;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TObjectPtr<UBVEquipmentHUDWidget> EquipmentWidget;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TObjectPtr<UBVSubtitleWidget> SubtitleWidget;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TObjectPtr<UBVMissionCompleteWidget> MissionCompleteWidget;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") EBVGlobalAlertLevel CurrentAlertLevel = EBVGlobalAlertLevel::Normal;

	UFUNCTION() void HandleMissionCompleted(const FMissionScore& Score);
};
