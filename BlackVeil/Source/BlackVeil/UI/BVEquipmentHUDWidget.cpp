// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVEquipmentHUDWidget.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVInteractionComponent.h"
#include "Gadgets/BVEquipmentInventory.h"
#include "Gadgets/BVGadgetBase.h"
#include "Gadgets/BVWeapons.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVPlayerCharacter* UBVEquipmentHUDWidget::GetPlayer() const
{
	UWorld* World = GetWorld();
	return World && World->GetFirstPlayerController()
		? Cast<ABVPlayerCharacter>(World->GetFirstPlayerController()->GetPawn()) : nullptr;
}

UBVGadgetBase* UBVEquipmentHUDWidget::GetGadgetSlot(int32 Index) const
{
	ABVPlayerCharacter* Player = GetPlayer();
	UBVEquipmentInventory* Inv = Player ? Player->GetEquipmentInventory() : nullptr;
	return Inv ? Inv->GetGadgetInSlot(Index) : nullptr;
}

int32 UBVEquipmentHUDWidget::GetGadgetCharges(int32 Index) const
{
	UBVGadgetBase* Gadget = GetGadgetSlot(Index);
	return Gadget ? Gadget->GetCurrentCharges() : 0;
}

void UBVEquipmentHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ABVPlayerCharacter* Player = GetPlayer();
	if (!Player) { return; }

	UBVEquipmentInventory* Inv = Player->GetEquipmentInventory();
	if (!Inv) { return; }

	// Slot changes.
	if (Inv->GetActiveSlotIndex() != ActiveSlotIndex)
	{
		ActiveSlotIndex = Inv->GetActiveSlotIndex();
		OnSlotChanged(ActiveSlotIndex);
	}

	// Weapon ammo readout.
	if (UBVWeaponBase* Weapon = Inv->GetCurrentWeapon())
	{
		const int32 Mag = Weapon->GetMagazineAmmo();
		const int32 Reserve = Weapon->GetReserveAmmo();
		if (Mag != MagazineAmmo || Reserve != ReserveAmmo || WeaponName.IsEmpty())
		{
			MagazineAmmo = Mag;
			ReserveAmmo = Reserve;
			bSuppressed = Weapon->IsSuppressed();
			WeaponName = Weapon->DisplayName;
			OnWeaponChanged(WeaponName, MagazineAmmo, ReserveAmmo);
		}
	}

	// Interaction prompt text.
	if (UBVInteractionComponent* Interaction = Player->GetInteractionComponent())
	{
		const FBVInteractionPrompt Prompt = Interaction->GetCurrentPrompt();
		const FText NewText = Prompt.bIsAvailable ? Prompt.ActionText : Prompt.UnavailableReason;
		if (!NewText.EqualTo(InteractionPromptText))
		{
			InteractionPromptText = NewText;
			OnInteractionPromptChanged(InteractionPromptText);
		}
	}
}
