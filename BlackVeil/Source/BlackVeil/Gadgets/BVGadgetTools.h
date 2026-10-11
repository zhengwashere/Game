// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BVGadgetBase.h"
#include "BVGadgetTools.generated.h"

/** Timed lock-picking tool. Interacts with locked doors/lockers. */
UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetLockpick : public UBVGadgetBase
{
	GENERATED_BODY()
public:
	UBVGadgetLockpick();

	/** Begins picking a lock. Returns the time the pick will take. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") float BeginLockpick(int32 LockDifficulty);

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Gadget") float SecondsPerDifficulty = 2.5f;
};

/** Wireless hacking tool for terminals, doors and cameras. */
UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetHackDevice : public UBVGadgetBase
{
	GENERATED_BODY()
public:
	UBVGadgetHackDevice();

	/** Hacking range in cm. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") float GetHackRange() const { return HackRange; }

	/** Time to defeat a device of the given security tier. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") float GetHackDuration(int32 SecurityTier) const;

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Gadget") float HackRange = 900.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Gadget") float SecondsPerTier = 4.f;
};

/** Passive detector: reveals camera cones and sensor coverage while equipped. */
UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetSurveillanceDetector : public UBVGadgetBase
{
	GENERATED_BODY()
public:
	UBVGadgetSurveillanceDetector();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") void SetDetectorActive(bool bActive);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") bool IsDetectorActive() const { return bDetectorActive; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") float GetDetectionRange() const { return DetectionRange; }

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Gadget") float DetectionRange = 3000.f;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Gadget") bool bDetectorActive = false;
};

/** Fiber-optic camera slid under doors to scout the far side. */
UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetFiberOpticCamera : public UBVGadgetBase
{
	GENERATED_BODY()
public:
	UBVGadgetFiberOpticCamera();

	/** Deploys the scope through a door and enables the picture-in-picture feed. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") bool DeployUnderDoor(AActor* DoorActor);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") void Retract();
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") bool IsDeployed() const { return bDeployed; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Gadget") bool bDeployed = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Gadget") TWeakObjectPtr<AActor> DeployedDoor;
};
