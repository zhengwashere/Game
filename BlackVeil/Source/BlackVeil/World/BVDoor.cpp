// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVDoor.h"
#include "World/BVNoiseSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Player/BVPlayerCharacter.h"
#include "Gadgets/BVEquipmentInventory.h"
#include "Gadgets/BVGadgetTools.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVDoor::ABVDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionType = EBVInteractionType::Door;
	PromptText = NSLOCTEXT("BlackVeil", "DoorOpen", "Open Door");
	IconTag = TEXT("Door");
	HoldDuration = 0.f;

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(MeshComponent);

	SecondaryLeaf = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryLeaf"));
	SecondaryLeaf->SetupAttachment(Pivot);
	SecondaryLeaf->SetVisibility(false);
	SecondaryLeaf->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABVDoor::CanInteract_Implementation(ABVPlayerCharacter* Interactor) const
{
	// A locked door can always be interacted with; the prompt explains what is
	// needed, and the actions list reflects what the player currently carries.
	return true;
}

TArray<FName> ABVDoor::GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const
{
	TArray<FName> Actions;

	if (!bIsOpen)
	{
		Actions.Add(TEXT("Door.OpenQuiet"));
		Actions.Add(TEXT("Door.OpenNormal"));

		if (LockType == EBVLockType::Mechanical)
		{
			Actions.Add(TEXT("Door.Lockpick"));
		}
		else if (LockType == EBVLockType::Electronic)
		{
			Actions.Add(TEXT("Door.Hack"));
		}
		else if (LockType == EBVLockType::Keycard)
		{
			Actions.Add(TEXT("Door.UseKeycard"));
		}

		// Weaker doors can be brute-forced, at a steep noise cost.
		if (LockType == EBVLockType::Unlocked || LockType == EBVLockType::Mechanical || LockType == EBVLockType::Barred)
		{
			Actions.Add(TEXT("Door.Force"));
		}
	}
	else
	{
		Actions.Add(TEXT("Door.Close"));
	}

	return Actions;
}

FBVInteractionPrompt ABVDoor::GetInteractionPrompt_Implementation(ABVPlayerCharacter* Interactor) const
{
	FBVInteractionPrompt Prompt = Super::GetInteractionPrompt_Implementation(Interactor);

	if (LockType == EBVLockType::Mechanical)      { Prompt.ActionText = NSLOCTEXT("BlackVeil", "DoorLockedMech", "Locked - Lockpick Required"); }
	else if (LockType == EBVLockType::Electronic) { Prompt.ActionText = NSLOCTEXT("BlackVeil", "DoorLockedElec", "Locked - Hack Required"); }
	else if (LockType == EBVLockType::Keycard)    { Prompt.ActionText = NSLOCTEXT("BlackVeil", "DoorLockedCard", "Keycard Required"); }
	else if (LockType == EBVLockType::Biometric)  { Prompt.ActionText = NSLOCTEXT("BlackVeil", "DoorLockedBio", "Biometric Lock - Find Another Route"); }
	else if (LockType == EBVLockType::Barred)     { Prompt.ActionText = NSLOCTEXT("BlackVeil", "DoorBarred", "Barred - Force It"); }

	return Prompt;
}

bool ABVDoor::ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor)
{
	if (ActionID == TEXT("Door.OpenQuiet"))
	{
		if (IsLocked())
		{
			// Quiet opening cannot bypass a lock.
			return false;
		}
		OpenQuietly();
	}
	else if (ActionID == TEXT("Door.OpenNormal"))
	{
		if (IsLocked())
		{
			return false;
		}
		OpenNormally();
	}
	else if (ActionID == TEXT("Door.Close"))
	{
		CloseDoor();
	}
	else if (ActionID == TEXT("Door.Force"))
	{
		ForceOpen();
	}
	else if (ActionID == TEXT("Door.Lockpick"))
	{
		if (LockType != EBVLockType::Mechanical) { return false; }
		if (Interactor && Interactor->GetEquipmentInventory())
		{
			// The pick is held for the duration; on completion the door unlocks.
			Unlock();
			OpenQuietly();
		}
		else
		{
			return false;
		}
	}
	else if (ActionID == TEXT("Door.Hack"))
	{
		if (LockType != EBVLockType::Electronic || !bPowered) { return false; }
		Unlock();
		OpenQuietly();
	}
	else if (ActionID == TEXT("Door.UseKeycard"))
	{
		if (LockType != EBVLockType::Keycard) { return false; }
		Unlock();
		OpenQuietly();
	}
	else
	{
		return false;
	}

	return Super::ExecuteInteraction_Implementation(ActionID, Interactor);
}

void ABVDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 0.4f))
	{
		return;
	}

	// Smooth swing with an ease-out so doors do not snap.
	CurrentAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaSeconds, OpenSpeed / 90.f);

	const float Direction = bOpenInward ? -1.f : 1.f;
	if (Pivot)
	{
		Pivot->SetRelativeRotation(FRotator(0.f, CurrentAngle * Direction, 0.f));
	}

	// Update collision so the doorway physically opens/closes.
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABVDoor::SetLocked(EBVLockType NewLockType)
{
	LockType = NewLockType;
}

void ABVDoor::Unlock()
{
	LockType = EBVLockType::Unlocked;
}

void ABVDoor::OpenQuietly()
{
	bIsOpen = true;
	TargetAngle = OpenAngle * 0.85f; // cracked open, not flung wide
	ReportDoorNoise(EBVNoiseLevel::Light);
}

void ABVDoor::OpenNormally()
{
	bIsOpen = true;
	TargetAngle = OpenAngle;
	ReportDoorNoise(EBVNoiseLevel::Medium);
}

void ABVDoor::ForceOpen()
{
	// Forcing breaks the lock but makes a racket.
	LockType = EBVLockType::Unlocked;
	bIsOpen = true;
	TargetAngle = OpenAngle * 1.15f;
	ReportDoorNoise(EBVNoiseLevel::Heavy);
}

void ABVDoor::CloseDoor()
{
	bIsOpen = false;
	TargetAngle = 0.f;
	ReportDoorNoise(EBVNoiseLevel::Light);
}

void ABVDoor::SetPowerEnabled(bool bEnabled)
{
	bPowered = bEnabled;
}

void ABVDoor::ReportDoorNoise(EBVNoiseLevel Level)
{
	if (UWorld* World = GetWorld())
	{
		if (UBVNoiseSubsystem* Noise = World->GetSubsystem<UBVNoiseSubsystem>())
		{
			Noise->ReportNoise(GetActorLocation(), Level, this, false, TEXT("Door"));
		}
	}
}
