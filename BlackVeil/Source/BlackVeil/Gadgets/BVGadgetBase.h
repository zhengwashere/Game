// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BVGadgetBase.generated.h"

class ABVPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnGadgetChargesChanged, FName, GadgetID, int32, RemainingCharges);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnGadgetUsed, FName, GadgetID);

/**
 * Base class for every piece of tactical equipment.
 *
 * Held gadgets (lockpick, hack device, detector) and deployed gadgets
 * (throwables, cameras) both derive from this so the inventory can treat them
 * uniformly. Subclasses implement UseGadget().
 */
UCLASS(Abstract, ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVGadgetBase();

	/** Stable identifier matching the data table / mission loadout. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") FName GadgetID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget", meta = (MultiLine = true)) FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") FName IconTag;

	/** Charges at mission start (and the maximum refillable amount). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") int32 MaxCharges = 3;

	/** How far the gadget's use is audible, in cm. 0 = silent. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") float SoundEmissionRadius = 0.f;

	/** When true the gadget is aimed and thrown rather than used in place. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") bool bIsThrowable = false;

	/** When true the gadget is a weapon-like device that stays equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Gadget") bool bIsPersistent = false;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") int32 GetCurrentCharges() const { return CurrentCharges; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") int32 GetMaxCharges() const { return MaxCharges; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") bool CanUse() const { return CurrentCharges > 0; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Gadget") bool IsThrowable() const { return bIsThrowable; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") void SetCharges(int32 NewCharges);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget") void AddCharges(int32 Delta);

	/**
	 * Activate the gadget. Returns true if the action was consumed.
	 * @param User          The operative using the gadget.
	 * @param AimLocation   Where the player is aiming, for thrown devices.
	 */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Gadget")
	virtual bool UseGadget(ABVPlayerCharacter* User, const FVector& AimLocation);

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Gadget") FBVOnGadgetChargesChanged OnChargesChanged;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Gadget") FBVOnGadgetUsed OnGadgetUsed;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Gadget") int32 CurrentCharges = 0;

	virtual void BeginPlay() override;

	/** Emits the gadget's operating noise into the world noise subsystem. */
	void EmitGadgetNoise(ABVPlayerCharacter* User);
};
