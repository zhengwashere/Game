// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BVTypes.h"
#include "BVObjectiveTrackerWidget.generated.h"

/**
 * Live objective list. Subscribes to the mission manager so objectives appear,
 * update and complete without the HUD polling anything.
 */
UCLASS(Abstract)
class BLACKVEIL_API UBVObjectiveTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") const TArray<FBVObjectiveDisplayData>& GetObjectives() const { return Objectives; }

	/** Rebuild the visible list. Call from Blueprint when the array changes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnObjectivesChanged(const TArray<FBVObjectiveDisplayData>& AllObjectives);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnObjectiveUpdated(const FBVObjectiveDisplayData& Objective);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") TArray<FBVObjectiveDisplayData> Objectives;

	UFUNCTION() void HandleObjectivesRefreshed(const TArray<FBVObjectiveDisplayData>& AllObjectives);
	UFUNCTION() void HandleObjectiveUpdated(const FBVObjectiveDisplayData& Objective);
};
