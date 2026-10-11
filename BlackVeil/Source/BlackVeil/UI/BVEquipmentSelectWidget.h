// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BVEquipmentSelectWidget.generated.h"

/** Pre-mission loadout picker, limited to gadgets the player has unlocked. */
UCLASS(Abstract)
class BLACKVEIL_API UBVEquipmentSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	/** All gadget IDs unlocked in the campaign. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") const TArray<FName>& GetUnlockedGadgets() const { return UnlockedGadgets; }
	/** The four currently chosen gadget slots (may contain NAME_None). */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") const TArray<FName>& GetSelectedGadgets() const { return SelectedGadgets; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") const TArray<FName>& GetUnlockedWeapons() const { return UnlockedWeapons; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Menu") FName GetSelectedWeapon() const { return SelectedWeapon; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") bool AssignGadgetToSlot(FName GadgetID, int32 SlotIndex);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void ClearSlot(int32 SlotIndex);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void SelectWeapon(FName WeaponID);

	/** Persists the loadout into the game instance for the next mission launch. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Menu") void ConfirmLoadout();

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|Menu") void OnLoadoutChanged();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") TArray<FName> UnlockedGadgets;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") TArray<FName> SelectedGadgets;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") TArray<FName> UnlockedWeapons;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Menu") FName SelectedWeapon;
};
