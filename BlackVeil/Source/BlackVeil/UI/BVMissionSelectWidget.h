// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BlackVeilGameInstance.h"
#include "BVMissionSelectWidget.generated.h"

/** Campaign map: pick any unlocked mission and see its best grade. */
UCLASS(Abstract)
class BLACKVEIL_API UBVMissionSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") bool IsMissionUnlocked(int32 MissionIndex) const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") FString GetMissionGrade(int32 MissionIndex) const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") int32 GetHighestUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") bool SelectMission(int32 MissionIndex);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void SetDifficulty(EDifficulty NewDifficulty);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") EDifficulty GetDifficulty() const { return SelectedDifficulty; }

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Menu") void OnMissionSelected(int32 MissionIndex, bool bUnlocked, const FString& Grade);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") int32 SelectedMissionIndex = 1;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") EDifficulty SelectedDifficulty = EDifficulty::Agent;
};
