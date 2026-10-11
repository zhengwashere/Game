// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGadgetTools.h"
#include "Player/BVPlayerCharacter.h"
#include "World/BVNoiseSubsystem.h"
#include "Engine/World.h"
#include "BlackVeil.h"

// ----------------------------------------------------------------- Lockpick

UBVGadgetLockpick::UBVGadgetLockpick()
{
	GadgetID = TEXT("Lockpick");
	DisplayName = NSLOCTEXT("BlackVeil", "Lockpick", "Lockpick Set");
	Description = NSLOCTEXT("BlackVeil", "LockpickDesc", "Quietly defeats mechanical locks. Takes longer on higher-security locks.");
	IconTag = TEXT("Gadget_Lockpick");
	MaxCharges = 1; // reusable tool, charges represent durability in some missions
	bIsPersistent = true;
	SoundEmissionRadius = 120.f;
}

float UBVGadgetLockpick::BeginLockpick(int32 LockDifficulty)
{
	// Picking is quiet but not silent: a guard standing right outside may hear.
	if (UBVNoiseSubsystem* Noise = GetWorld() ? GetWorld()->GetSubsystem<UBVNoiseSubsystem>() : nullptr)
	{
		Noise->ReportNoise(GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector,
			EBVNoiseLevel::Light, GetOwner(), false, TEXT("Lockpick"));
	}
	return SecondsPerDifficulty * FMath::Max(1, LockDifficulty);
}

// -------------------------------------------------------------- HackDevice

UBVGadgetHackDevice::UBVGadgetHackDevice()
{
	GadgetID = TEXT("HackDevice");
	DisplayName = NSLOCTEXT("BlackVeil", "HackDevice", "Wireless Hack Device");
	Description = NSLOCTEXT("BlackVeil", "HackDeviceDesc", "Breaks into networked systems: cameras, terminals, electronic locks.");
	IconTag = TEXT("Gadget_HackDevice");
	MaxCharges = 5;
	bIsPersistent = true;
	SoundEmissionRadius = 60.f;
}

float UBVGadgetHackDevice::GetHackDuration(int32 SecurityTier) const
{
	return SecondsPerTier * FMath::Max(1, SecurityTier);
}

// -------------------------------------------------- SurveillanceDetector

UBVGadgetSurveillanceDetector::UBVGadgetSurveillanceDetector()
{
	GadgetID = TEXT("SurveillanceDetector");
	DisplayName = NSLOCTEXT("BlackVeil", "Detector", "Surveillance Detector");
	Description = NSLOCTEXT("BlackVeil", "DetectorDesc", "Highlights nearby cameras and sensors, including their coverage cones.");
	IconTag = TEXT("Gadget_Detector");
	MaxCharges = 1;
	bIsPersistent = true;
	SoundEmissionRadius = 0.f;
}

void UBVGadgetSurveillanceDetector::SetDetectorActive(bool bActive)
{
	bDetectorActive = bActive;
}

// ------------------------------------------------------- FiberOpticCamera

UBVGadgetFiberOpticCamera::UBVGadgetFiberOpticCamera()
{
	GadgetID = TEXT("FiberOpticCamera");
	DisplayName = NSLOCTEXT("BlackVeil", "FiberOptic", "Fiber-Optic Camera");
	Description = NSLOCTEXT("BlackVeil", "FiberOpticDesc", "Slides under doors to reveal what waits on the other side.");
	IconTag = TEXT("Gadget_FiberOptic");
	MaxCharges = 1;
	bIsPersistent = true;
	SoundEmissionRadius = 40.f;
}

bool UBVGadgetFiberOpticCamera::DeployUnderDoor(AActor* DoorActor)
{
	if (!DoorActor) { return false; }
	bDeployed = true;
	DeployedDoor = DoorActor;
	return true;
}

void UBVGadgetFiberOpticCamera::Retract()
{
	bDeployed = false;
	DeployedDoor = nullptr;
}
