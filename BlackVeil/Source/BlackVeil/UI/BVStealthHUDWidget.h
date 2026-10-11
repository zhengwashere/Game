// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BVTypes.h"
#include "BVStealthHUDWidget.generated.h"

class ABVPlayerCharacter;
class UBVStealthComponent;

/**
 * Core stealth readout: visibility meter, noise ring, shadow indicator and
 * detection warning. Pulls live values from the player's stealth component so
 * the player always knows how exposed they are.
 */
UCLASS(Abstract)
class BLACKVEIL_API UBVStealthHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") float GetVisibilityPercent() const { return VisibilityPercent; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") bool IsInShadow() const { return bInShadow; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") float GetNoiseRadius() const { return NoiseRadius; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") EBVNoiseLevel GetNoiseLevel() const { return NoiseLevel; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") bool IsDetected() const { return bDetected; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") float GetDetectionFlashAlpha() const { return DetectionFlashAlpha; }

	/** Called when an enemy first locks onto the player; drives the red vignette. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|HUD") void NotifyDetected();

	/** Blueprint hooks for visuals; the C++ side owns the data. */
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnVisibilityUpdated(float Percent, bool bShadow);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnNoiseUpdated(float Radius, EBVNoiseLevel Level);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnDetectionFlash(float Alpha);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") float VisibilityPercent = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") bool bInShadow = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") float NoiseRadius = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") EBVNoiseLevel NoiseLevel = EBVNoiseLevel::Silent;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") bool bDetected = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") float DetectionFlashAlpha = 0.f;

private:
	ABVPlayerCharacter* GetPlayer() const;
	float DetectionFlashTimer = 0.f;
};
