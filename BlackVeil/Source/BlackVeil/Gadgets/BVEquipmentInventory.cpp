// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVEquipmentInventory.h"
#include "BVGadgetBase.h"
#include "BVGadgetThrowables.h"
#include "BVGadgetTools.h"
#include "BVWeapons.h"
#include "Player/BVPlayerCharacter.h"
#include "Engine/World.h"
#include "BlackVeil.h"

UBVEquipmentInventory::UBVEquipmentInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBVEquipmentInventory::BeginPlay()
{
	Super::BeginPlay();
	BuildRegistry();
	GadgetSlots.SetNum(4);
}

ABVPlayerCharacter* UBVEquipmentInventory::GetOwningCharacter() const
{
	return Cast<ABVPlayerCharacter>(GetOwner());
}

void UBVEquipmentInventory::BuildRegistry()
{
	GadgetRegistry.Empty();
	GadgetRegistry.Add(TEXT("NoiseMaker"), UBVGadgetNoiseMaker::StaticClass());
	GadgetRegistry.Add(TEXT("Flashbang"), UBVGadgetFlashbang::StaticClass());
	GadgetRegistry.Add(TEXT("SmokeGrenade"), UBVGadgetSmokeGrenade::StaticClass());
	GadgetRegistry.Add(TEXT("EMP"), UBVGadgetEMP::StaticClass());
	GadgetRegistry.Add(TEXT("StickyCamera"), UBVGadgetStickyCamera::StaticClass());
	GadgetRegistry.Add(TEXT("Lockpick"), UBVGadgetLockpick::StaticClass());
	GadgetRegistry.Add(TEXT("HackDevice"), UBVGadgetHackDevice::StaticClass());
	GadgetRegistry.Add(TEXT("SurveillanceDetector"), UBVGadgetSurveillanceDetector::StaticClass());
	GadgetRegistry.Add(TEXT("FiberOpticCamera"), UBVGadgetFiberOpticCamera::StaticClass());

	WeaponRegistry.Empty();
	WeaponRegistry.Add(TEXT("SuppressedPistol"), UBVWeaponSuppressedPistol::StaticClass());
	WeaponRegistry.Add(TEXT("SuppressedRifle"), UBVWeaponSuppressedRifle::StaticClass());
	WeaponRegistry.Add(TEXT("NonLethalProjector"), UBVWeaponNonLethal::StaticClass());
}

UBVGadgetBase* UBVEquipmentInventory::AddGadgetByID(FName GadgetID, int32 PreferredSlot)
{
	if (GadgetRegistry.Num() == 0) { BuildRegistry(); }

	const TSubclassOf<UBVGadgetBase>* ClassPtr = GadgetRegistry.Find(GadgetID);
	if (!ClassPtr || !*ClassPtr)
	{
		UE_LOG(LogBlackVeil, Warning, TEXT("Unknown gadget ID: %s"), *GadgetID.ToString());
		return nullptr;
	}

	UBVGadgetBase* Gadget = NewObject<UBVGadgetBase>(GetOwner(), *ClassPtr);
	if (!Gadget) { return nullptr; }

	Gadget->RegisterComponent();

	// Place into the requested slot, else the first free one.
	int32 Slot = PreferredSlot;
	if (Slot < 0 || Slot >= GadgetSlots.Num() || GadgetSlots[Slot] != nullptr)
	{
		Slot = GadgetSlots.IndexOfByKey(nullptr);
	}

	if (Slot >= 0 && Slot < GadgetSlots.Num())
	{
		GadgetSlots[Slot] = Gadget;
	}
	else
	{
		GadgetSlots.Add(Gadget);
	}

	return Gadget;
}

UBVWeaponBase* UBVEquipmentInventory::AddWeaponByID(FName WeaponID)
{
	if (WeaponRegistry.Num() == 0) { BuildRegistry(); }

	const TSubclassOf<UBVWeaponBase>* ClassPtr = WeaponRegistry.Find(WeaponID);
	if (!ClassPtr || !*ClassPtr)
	{
		UE_LOG(LogBlackVeil, Warning, TEXT("Unknown weapon ID: %s"), *WeaponID.ToString());
		return nullptr;
	}

	UBVWeaponBase* Weapon = NewObject<UBVWeaponBase>(GetOwner(), *ClassPtr);
	if (!Weapon) { return nullptr; }

	Weapon->RegisterComponent();
	Weapons.Add(Weapon);
	return Weapon;
}

void UBVEquipmentInventory::SetupLoadout(const TArray<FName>& GadgetIDs, const TArray<FName>& WeaponIDs)
{
	GadgetSlots.Empty();
	GadgetSlots.SetNum(4);

	int32 Slot = 0;
	for (const FName& ID : GadgetIDs)
	{
		if (Slot >= 4) { break; }
		AddGadgetByID(ID, Slot);
		++Slot;
	}

	for (const FName& WeaponID : WeaponIDs)
	{
		AddWeaponByID(WeaponID);
	}

	// Default to the primary weapon.
	if (Weapons.Num() > 0)
	{
		SelectWeaponSlot(0);
	}
}

UBVGadgetBase* UBVEquipmentInventory::GetGadgetInSlot(int32 SlotIndex) const
{
	return GadgetSlots.IsValidIndex(SlotIndex) ? GadgetSlots[SlotIndex].Get() : nullptr;
}

UBVGadgetBase* UBVEquipmentInventory::GetActiveGadget() const
{
	return ActiveSlotIndex >= 0 ? GetGadgetInSlot(ActiveSlotIndex) : nullptr;
}

UBVWeaponBase* UBVEquipmentInventory::GetCurrentWeapon() const
{
	return Weapons.IsValidIndex(CurrentWeaponIndex) ? Weapons[CurrentWeaponIndex].Get() : nullptr;
}

void UBVEquipmentInventory::SelectGadgetSlot(int32 SlotIndex)
{
	if (!GadgetSlots.IsValidIndex(SlotIndex) || !GadgetSlots[SlotIndex])
	{
		return;
	}

	// Switching away from the weapon cancels any burst fire.
	StopFire();

	ActiveSlotIndex = SlotIndex;
	OnActiveSlotChanged.Broadcast(ActiveSlotIndex);

	UE_LOG(LogBlackVeil, Verbose, TEXT("Selected gadget slot %d"), SlotIndex);
}

void UBVEquipmentInventory::SelectWeaponSlot(int32 WeaponIndex)
{
	if (!Weapons.IsValidIndex(WeaponIndex)) { return; }

	StopFire();
	CurrentWeaponIndex = WeaponIndex;
	ActiveSlotIndex = -1;
	OnActiveSlotChanged.Broadcast(ActiveSlotIndex);
}

void UBVEquipmentInventory::CycleWeapon()
{
	if (Weapons.Num() <= 1)
	{
		SelectWeaponSlot(0);
		return;
	}
	SelectWeaponSlot((CurrentWeaponIndex + 1) % Weapons.Num());
}

void UBVEquipmentInventory::UseActiveGadget(const FVector& AimLocation)
{
	UBVGadgetBase* Gadget = GetActiveGadget();
	if (!Gadget) { return; }

	Gadget->UseGadget(GetOwningCharacter(), AimLocation);
}

void UBVEquipmentInventory::StartFire()
{
	// Gadgets are used instantly; the weapon fires.
	if (UBVGadgetBase* Gadget = GetActiveGadget())
	{
		ABVPlayerCharacter* Character = GetOwningCharacter();
		const FVector Aim = Character
			? Character->GetActorLocation() + Character->GetControlRotation().Vector() * 3000.f
			: FVector::ZeroVector;
		Gadget->UseGadget(Character, Aim);
		return;
	}

	if (UBVWeaponBase* Weapon = GetCurrentWeapon())
	{
		Weapon->StartFire(GetOwningCharacter());
	}
}

void UBVEquipmentInventory::StopFire()
{
	if (UBVWeaponBase* Weapon = GetCurrentWeapon())
	{
		Weapon->StopFire();
	}
}

void UBVEquipmentInventory::Reload()
{
	if (UBVWeaponBase* Weapon = GetCurrentWeapon())
	{
		Weapon->Reload();
	}
}

void UBVEquipmentInventory::RefillAll()
{
	for (TObjectPtr<UBVGadgetBase>& Gadget : GadgetSlots)
	{
		if (Gadget) { Gadget->SetCharges(Gadget->GetMaxCharges()); }
	}

	for (TObjectPtr<UBVWeaponBase>& Weapon : Weapons)
	{
		if (Weapon) { Weapon->Reload(); }
	}
}
