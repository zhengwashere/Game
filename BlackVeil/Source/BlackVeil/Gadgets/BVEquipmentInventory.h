// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BVEquipmentInventory.generated.h"

class UBVGadgetBase;
class UBVWeaponBase;
class ABVPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnActiveSlotChanged, int32, SlotIndex);

/**
 * The operative's loadout: one active weapon plus four gadget slots.
 *
 * The inventory creates gadget and weapon components on demand from a registry
 * of classes keyed by ID, so a mission's loadout is just a list of names read
 * from the mission data asset.
 */
UCLASS(ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVEquipmentInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVEquipmentInventory();

	/** Populate the loadout from mission configuration. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory")
	void SetupLoadout(const TArray<FName>& GadgetIDs, const TArray<FName>& WeaponIDs);

	/** Add a single gadget, optionally into a specific slot. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory")
	UBVGadgetBase* AddGadgetByID(FName GadgetID, int32 PreferredSlot = -1);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory")
	UBVWeaponBase* AddWeaponByID(FName WeaponID);

	// ---- Selection ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void SelectGadgetSlot(int32 SlotIndex);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void SelectWeaponSlot(int32 WeaponIndex);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void CycleWeapon();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Inventory") int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Inventory") UBVGadgetBase* GetGadgetInSlot(int32 SlotIndex) const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Inventory") UBVGadgetBase* GetActiveGadget() const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Inventory") UBVWeaponBase* GetCurrentWeapon() const;

	// ---- Actions ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void UseActiveGadget(const FVector& AimLocation);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void StartFire();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void StopFire();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void Reload();

	/** Refills all charges/ammo (checkpoint resupply). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Inventory") void RefillAll();

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Inventory") FBVOnActiveSlotChanged OnActiveSlotChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Inventory") TArray<TObjectPtr<UBVGadgetBase>> GadgetSlots;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Inventory") TArray<TObjectPtr<UBVWeaponBase>> Weapons;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Inventory") int32 ActiveSlotIndex = -1;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Inventory") int32 CurrentWeaponIndex = 0;

	/** GadgetID -> component class. */
	UPROPERTY() TMap<FName, TSubclassOf<UBVGadgetBase>> GadgetRegistry;
	UPROPERTY() TMap<FName, TSubclassOf<UBVWeaponBase>> WeaponRegistry;

	void BuildRegistry();
	ABVPlayerCharacter* GetOwningCharacter() const;
};
