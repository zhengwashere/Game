// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BVTypes.h"
#include "BVStealthComponent.generated.h"

class ALight;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnVisibilityChanged, float, Visibility, bool, bInShadow);

/**
 * Computes how exposed the player is to enemy sight.
 *
 * Visibility is derived from:
 *   - sampled light intensity at the player (with line-of-sight shadowing)
 *   - posture (prone < crouch < standing)
 *   - movement (a sprinting operative is far easier to spot)
 *   - leaning (peeking exposes more of the body)
 *
 * The resulting 0..1 value is what enemy perception multiplies its effective
 * sight range by, so darkness genuinely protects the player.
 */
UCLASS(ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVStealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVStealthComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ---- Inputs pushed by the owning character ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Stealth") void SetPlayerPosture(EBVPosture NewPosture);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Stealth") void SetMovementSpeed(EBVMovementSpeed NewSpeed);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Stealth") void SetLeaning(bool bNewLeaning);

	/** Set by the owning character each tick from its ground physical material. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Stealth") void SetCurrentSurface(EBVSurfaceType NewSurface);

	// ---- Outputs consumed by AI + HUD ----
	/** 0 = invisible in deep shadow, 1 = fully lit and obvious. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Stealth") float GetVisibilityLevel() const { return VisibilityLevel; }

	/** Raw light exposure before posture/movement modifiers. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Stealth") float GetLightExposure() const { return LightExposure; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Stealth") bool IsInShadow() const { return LightExposure < ShadowThreshold; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Stealth") EBVNoiseLevel GetCurrentNoiseLevel() const { return CurrentNoise; }

	/** Radius in cm within which the player's current movement can be heard. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Stealth") float GetNoiseRadius() const;

	/** Multiplier applied to enemy effective sight range against this player. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Stealth") float GetDetectionMultiplier() const;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Stealth")
	FBVOnVisibilityChanged OnVisibilityChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Stealth|Tuning") float SampleInterval = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Stealth|Tuning") float LightSampleRadius = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Stealth|Tuning") float ShadowThreshold = 0.18f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Stealth|Tuning") float AmbientBaseline = 0.06f;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") float VisibilityLevel = 0.5f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") float LightExposure = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") EBVNoiseLevel CurrentNoise = EBVNoiseLevel::Silent;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") EBVPosture Posture = EBVPosture::Standing;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") EBVMovementSpeed Speed = EBVMovementSpeed::Walk;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") bool bLeaning = false;

private:
	void SampleLighting();
	float PostureMultiplier() const;
	float MovementMultiplier() const;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Stealth") EBVSurfaceType CurrentSurface = EBVSurfaceType::Concrete;

	float TimeSinceSample = 0.f;
	float FootstepAccumulator = 0.f;
	TArray<TWeakObjectPtr<AActor>> CachedLights;
	float LightCacheAge = 0.f;
	void RefreshLightCache();
};
