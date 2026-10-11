// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BVMainMenuWidget.generated.h"

UCLASS(Abstract)
class BLACKVEIL_API UBVMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void StartNewGame();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void ContinueCampaign();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void OpenMissionSelect();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void OpenOptions();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void QuitGame();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") bool HasExistingSave() const { return bHasSave; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") FText GetContinueLabel() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Menu") void OnSaveStateResolved(bool bSaveExists);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") bool bHasSave = false;
};
