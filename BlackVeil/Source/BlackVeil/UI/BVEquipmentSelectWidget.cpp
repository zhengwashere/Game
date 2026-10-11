// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVEquipmentSelectWidget.h"
#include "Core/BlackVeilGameInstance.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVEquipmentSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectedGadgets.SetNum(4);

	if (const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		UnlockedGadgets = GI->CampaignData.UnlockedEquipment.UnlockedGadgets;
	}

	// Weapons are fixed per campaign progression; the rifle unlocks at mission 4.
	UnlockedWeapons = { TEXT("SuppressedPistol"), TEXT("NonLethalProjector") };
	if (const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		if (GI->CampaignData.HighestMissionUnlocked >= 4)
		{
			UnlockedWeapons.Add(TEXT("SuppressedRifle"));
		}
	}

	SelectedWeapon = UnlockedWeapons.Num() > 0 ? UnlockedWeapons[0] : NAME_None;
	OnLoadoutChanged();
}

bool UBVEquipmentSelectWidget::AssignGadgetToSlot(FName GadgetID, int32 SlotIndex)
{
	if (!SelectedGadgets.IsValidIndex(SlotIndex)) { return false; }
	if (!UnlockedGadgets.Contains(GadgetID)) { return false; }

	SelectedGadgets[SlotIndex] = GadgetID;
	OnLoadoutChanged();
	return true;
}

void UBVEquipmentSelectWidget::ClearSlot(int32 SlotIndex)
{
	if (SelectedGadgets.IsValidIndex(SlotIndex))
	{
		SelectedGadgets[SlotIndex] = NAME_None;
		OnLoadoutChanged();
	}
}

void UBVEquipmentSelectWidget::SelectWeapon(FName WeaponID)
{
	if (UnlockedWeapons.Contains(WeaponID))
	{
		SelectedWeapon = WeaponID;
		OnLoadoutChanged();
	}
}

void UBVEquipmentSelectWidget::ConfirmLoadout()
{
	// The chosen loadout is written to the game instance and read by the mission
	// manager when the level loads, so it survives the level transition.
	if (UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->CampaignData.UnlockedEquipment.UnlockedGadgets = UnlockedGadgets;
		GI->SaveCampaign();
	}
}
