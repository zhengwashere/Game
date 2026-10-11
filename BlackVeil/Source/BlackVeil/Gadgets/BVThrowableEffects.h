// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BVThrowableProjectile.h"
#include "BVThrowableEffects.generated.h"

class USceneCaptureComponent2D;
class UNiagaraComponent;

/** Distraction device: rattles and beeps, pulling guards toward it. */
UCLASS()
class BLACKVEIL_API ABVProjectile_NoiseMaker : public ABVThrowableProjectile
{
	GENERATED_BODY()
public:
	ABVProjectile_NoiseMaker();
	virtual void Detonate() override;
protected:
	/** How long the device keeps making noise. */
	UPROPERTY(EditDefaultsOnly, Category = "NoiseMaker") float LingerDuration = 12.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoiseMaker") float PulseInterval = 1.6f;
	int32 PulsesRemaining = 0;
	FTimerHandle PulseTimer;
	void PulseNoise();
};

/** Flashbang: blinds and deafens guards in line of sight of the burst. */
UCLASS()
class BLACKVEIL_API ABVProjectile_Flashbang : public ABVThrowableProjectile
{
	GENERATED_BODY()
public:
	ABVProjectile_Flashbang();
	virtual void Detonate() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Flashbang") float BlindDuration = 9.f;
	UPROPERTY(EditDefaultsOnly, Category = "Flashbang") float DeafenedDuration = 6.f;
};

/** Smoke: blocks AI sight lines for a while. */
UCLASS()
class BLACKVEIL_API ABVProjectile_Smoke : public ABVThrowableProjectile
{
	GENERATED_BODY()
public:
	ABVProjectile_Smoke();
	virtual void Detonate() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Smoke") float SmokeDuration = 16.f;
	UPROPERTY(EditDefaultsOnly, Category = "Smoke") float SmokeRadius = 700.f;
};

/** EMP: kills power to cameras, terminals, alarms and lights in radius. */
UCLASS()
class BLACKVEIL_API ABVProjectile_EMP : public ABVThrowableProjectile
{
	GENERATED_BODY()
public:
	ABVProjectile_EMP();
	virtual void Detonate() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "EMP") float DisableDuration = 30.f;
};

/** Sticky camera: attaches to a surface and streams a feed back to the player. */
UCLASS()
class BLACKVEIL_API ABVProjectile_StickyCamera : public ABVThrowableProjectile
{
	GENERATED_BODY()
public:
	ABVProjectile_StickyCamera();
	virtual void Detonate() override;
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Camera") void EmitDistractionNoise();
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Camera") USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComponent; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<USceneCaptureComponent2D> CaptureComponent;
};
