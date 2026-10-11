// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Mission/BVMissionData.h"
#include "BVBriefingWidget.generated.h"

/** Pre-mission briefing: story text, objectives preview, difficulty, loadout. */
UCLASS(Abstract)
class BLACKVEIL_API UBVBriefingWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void LoadBriefingForMission(int32 MissionIndex);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") FText GetMissionName() const { return MissionName; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") FText GetLocationName() const { return LocationName; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") FText GetBriefingText() const { return BriefingText; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") const TArray<FBVObjectiveData>& GetObjectives() const { return Objectives; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") const TArray<FName>& GetIntelIDs() const { return IntelIDs; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void LaunchMission();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void OpenEquipmentSelect();

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Menu") void OnBriefingLoaded(const FText& Name, const FText& Location, const FText& Body);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") TObjectPtr<UBVMissionData> MissionData;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") FText MissionName;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") FText LocationName;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") FText BriefingText;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") TArray<FBVObjectiveData> Objectives;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") TArray<FName> IntelIDs;
};
