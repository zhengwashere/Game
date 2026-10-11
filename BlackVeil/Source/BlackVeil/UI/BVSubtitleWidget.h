// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BVSubtitleWidget.generated.h"

/** Radio dialogue display with a speaker tag and auto-dismiss. */
UCLASS(Abstract)
class BLACKVEIL_API UBVSubtitleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD")
	void ShowLine(FName SpeakerID, const FText& SpeakerName, const FText& Line, float Duration);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void ClearLine();
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") bool IsShowing() const { return bShowing; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") const FText& GetCurrentLine() const { return CurrentLine; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") const FText& GetSpeakerName() const { return CurrentSpeaker; }

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnLineShown(const FText& Speaker, const FText& Line);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnLineHidden();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") bool bShowing = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") FText CurrentLine;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") FText CurrentSpeaker;
	float RemainingTime = 0.f;
};
