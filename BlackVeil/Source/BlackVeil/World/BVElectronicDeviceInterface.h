// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BVElectronicDeviceInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBVElectronicDeviceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by anything powered: cameras, terminals, alarm panels, lights.
 * The EMP gadget and generator sabotage route through this so new electronic
 * objects become EMP-able automatically.
 */
class BLACKVEIL_API IBVElectronicDeviceInterface
{
	GENERATED_BODY()

public:
	/** Power down for Duration seconds (<=0 means indefinitely). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Electronic")
	void DisableFor(float Duration);

	/** Restore power immediately. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Electronic")
	void RestorePower();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Electronic")
	bool IsDisabled() const;

	/** Whether the device is currently powered (breaker on, not sabotaged). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Electronic")
	bool HasPower() const;
};
