// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BVHackMinigameWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnHackResolved, bool, bSuccess);

/**
 * Hacking / lockpicking minigame: a timing bar the player must stop inside a
 * moving sweet spot. Difficulty narrows the spot and speeds it up.
 */
UCLASS(Abstract)
class BLACKVEIL_API UBVHackMinigameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Begin a hack. SecurityTier 1..5. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Hack") void BeginHack(int32 SecurityTier, FName TargetID);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Hack") void SubmitHack();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Hack") void CancelHack();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hack") float GetCursorPosition() const { return CursorPosition; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hack") float GetSweetSpotCenter() const { return SweetSpotCenter; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hack") float GetSweetSpotHalfWidth() const { return SweetSpotHalfWidth; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hack") bool IsActive() const { return bActive; }

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Hack") FBVOnHackResolved OnHackResolved;

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Hack") void OnHackStarted(int32 SecurityTier);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Hack") void OnHackFinished(bool bSuccess);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hack") bool bActive = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hack") float CursorPosition = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hack") float SweetSpotCenter = 0.5f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hack") float SweetSpotHalfWidth = 0.15f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hack") int32 CurrentTier = 1;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hack") FName CurrentTargetID;
	float CursorDirection = 1.f;
	float CursorSpeed = 1.f;
};
