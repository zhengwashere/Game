// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BVElectronicDeviceInterface.h"
#include "BVDestructibleLight.generated.h"

class ULightComponent;
class UStaticMeshComponent;

/**
 * A switchable, shootable light fixture.
 *
 * Because the player's visibility is computed from real light components, killing
 * a light genuinely darkens the space and makes the operative harder to spot.
 * Lights can be switched, shot out, or knocked offline by an EMP / generator
 * sabotage via the electronic device interface.
 */
UCLASS()
class BLACKVEIL_API ABVDestructibleLight : public AActor, public IBVElectronicDeviceInterface
{
	GENERATED_BODY()

public:
	ABVDestructibleLight();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") FName LightGroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") bool bStartsOn = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") bool bDestructible = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") float Health = 15.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") float BaseIntensity = 3000.f;

	/** Flickering emergency lights add atmosphere and shifting visibility. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") bool bFlickers = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Light") float FlickerSpeed = 7.f;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Light") void SetLightOn(bool bOn);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Light") void ToggleLight();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Light") void TakeLightDamage(float Amount);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Light") bool IsLightOn() const { return bLightOn; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Light") bool IsBroken() const { return bBroken; }

	virtual void DisableFor_Implementation(float Duration) override;
	virtual void RestorePower_Implementation() override;
	virtual bool IsDisabled_Implementation() const override { return !bLightOn; }
	virtual bool HasPower_Implementation() const override { return bPowered; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UStaticMeshComponent> FixtureMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<ULightComponent> LightComponent;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Light") bool bLightOn = true;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Light") bool bBroken = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Light") bool bPowered = true;

	float FlickerAccumulator = 0.f;
	void ApplyLightState();
};
