// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/BVInteractableActor.h"
#include "BVElectronicDeviceInterface.h"
#include "BVSecurityTerminal.generated.h"

/**
 * A networked security console. Hacking it lets the operative shut down a
 * camera group, unlock a set of doors, or kill the lights in a zone - the
 * classic "turn their own security against them" move.
 */
UCLASS()
class BLACKVEIL_API ABVSecurityTerminal : public ABVInteractableActor, public IBVElectronicDeviceInterface
{
	GENERATED_BODY()

public:
	ABVSecurityTerminal();

	/** Security tier: 1 = trivial, 5 = hardened. Drives hack duration. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Terminal") int32 SecurityTier = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Terminal") FName CameraGroupToDisable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Terminal") TArray<FName> DoorTagsToUnlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Terminal") FName LightGroupToKill;

	/** Objective ID completed by successfully hacking this terminal (optional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Terminal") FName LinkedObjectiveID;

	virtual TArray<FName> GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual bool ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Terminal") void ApplyHackEffects();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Terminal") void DisableCamerasInGroup();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Terminal") void UnlockTaggedDoors();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Terminal") void KillLightsInGroup();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Terminal") bool IsHacked() const { return bHacked; }

	virtual void DisableFor_Implementation(float Duration) override;
	virtual void RestorePower_Implementation() override;
	virtual bool IsDisabled_Implementation() const override { return bDisabled; }
	virtual bool HasPower_Implementation() const override { return bPowered; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<class UPointLightComponent> ScreenLight;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Terminal") bool bHacked = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Terminal") bool bDisabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Terminal") bool bPowered = true;
};
