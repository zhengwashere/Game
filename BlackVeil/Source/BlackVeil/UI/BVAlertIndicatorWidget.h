// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BVTypes.h"
#include "BVAlertIndicatorWidget.generated.h"

/** Shows the global alert level and directional pips for nearby alerted guards. */
UCLASS(Abstract)
class BLACKVEIL_API UBVAlertIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void SetGlobalAlertLevel(EBVGlobalAlertLevel Level);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") EBVGlobalAlertLevel GetGlobalAlertLevel() const { return AlertLevel; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") FText GetAlertText() const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") int32 GetNearbyAlertedGuardCount() const { return NearbyGuardCount; }
	/** Direction (degrees) to the nearest alerted guard, relative to the camera. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") float GetNearestThreatBearing() const { return NearestThreatBearing; }

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnAlertLevelChanged(EBVGlobalAlertLevel Level);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnThreatsUpdated(int32 Count, float Bearing);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") EBVGlobalAlertLevel AlertLevel = EBVGlobalAlertLevel::Normal;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") int32 NearbyGuardCount = 0;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") float NearestThreatBearing = 0.f;
};
