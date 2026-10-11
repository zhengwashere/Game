// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BVEquipmentHUDWidget.generated.h"

class ABVPlayerCharacter;
class UBVGadgetBase;
class UBVWeaponBase;

/** Gadget slots, ammo readout and the contextual crosshair. */
UCLASS(Abstract)
class BLACKVEIL_API UBVEquipmentHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") FText GetWeaponName() const { return WeaponName; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") int32 GetMagazineAmmo() const { return MagazineAmmo; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") int32 GetReserveAmmo() const { return ReserveAmmo; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") bool IsWeaponSuppressed() const { return bSuppressed; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") UBVGadgetBase* GetGadgetSlot(int32 Index) const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") int32 GetGadgetCharges(int32 Index) const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|HUD") FText GetInteractionPromptText() const { return InteractionPromptText; }

	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnWeaponChanged(const FText& Name, int32 Mag, int32 Reserve);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnSlotChanged(int32 SlotIndex);
	UFUNCTION(BlueprintImplementableEvent, Category = "BlackVeil|HUD") void OnInteractionPromptChanged(const FText& Prompt);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") int32 ActiveSlotIndex = -1;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") FText WeaponName;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") int32 MagazineAmmo = 0;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") int32 ReserveAmmo = 0;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") bool bSuppressed = true;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|HUD") FText InteractionPromptText;

private:
	ABVPlayerCharacter* GetPlayer() const;
};
