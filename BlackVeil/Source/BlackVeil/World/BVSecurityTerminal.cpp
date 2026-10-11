// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVSecurityTerminal.h"
#include "BVSecurityCamera.h"
#include "BVDestructibleLight.h"
#include "BVDoor.h"
#include "Mission/BVMissionManager.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BlackVeil.h"

ABVSecurityTerminal::ABVSecurityTerminal()
{
	InteractionType = EBVInteractionType::Terminal;
	PromptText = NSLOCTEXT("BlackVeil", "TerminalHack", "Hack Security Terminal");
	IconTag = TEXT("Terminal");
	HoldDuration = 0.f;
	AvailableActions = { TEXT("Terminal.Hack") };

	ScreenLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ScreenLight"));
	ScreenLight->SetupAttachment(MeshComponent);
	ScreenLight->SetIntensity(180.f);
	ScreenLight->SetAttenuationRadius(220.f);
	ScreenLight->SetLightColor(FLinearColor(0.2f, 0.6f, 1.f));
}

TArray<FName> ABVSecurityTerminal::GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const
{
	TArray<FName> Actions;
	if (!bHacked && !bDisabled && bPowered)
	{
		Actions.Add(TEXT("Terminal.Hack"));
	}
	return Actions;
}

bool ABVSecurityTerminal::ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor)
{
	if (ActionID != TEXT("Terminal.Hack")) { return false; }
	if (bDisabled || !bPowered) { return false; }

	ApplyHackEffects();
	return Super::ExecuteInteraction_Implementation(ActionID, Interactor);
}

void ABVSecurityTerminal::ApplyHackEffects()
{
	bHacked = true;
	if (ScreenLight) { ScreenLight->SetLightColor(FLinearColor(0.2f, 1.f, 0.3f)); }

	if (!CameraGroupToDisable.IsNone()) { DisableCamerasInGroup(); }
	if (DoorTagsToUnlock.Num() > 0)      { UnlockTaggedDoors(); }
	if (!LightGroupToKill.IsNone())      { KillLightsInGroup(); }

	if (!LinkedObjectiveID.IsNone())
	{
		if (UWorld* World = GetWorld())
		{
			if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
			{
				MM->CompleteObjective(LinkedObjectiveID);
			}
		}
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Terminal %s hacked."), *GetName());
}

void ABVSecurityTerminal::DisableCamerasInGroup()
{
	for (TActorIterator<ABVSecurityCamera> It(GetWorld()); It; ++It)
	{
		ABVSecurityCamera* Camera = *It;
		if (Camera && Camera->AlarmGroupID == CameraGroupToDisable)
		{
			Camera->SetDetectionEnabled(false);
		}
	}
}

void ABVSecurityTerminal::UnlockTaggedDoors()
{
	for (TActorIterator<ABVDoor> It(GetWorld()); It; ++It)
	{
		ABVDoor* Door = *It;
		if (!Door) { continue; }

		for (const FName& Tag : DoorTagsToUnlock)
		{
			if (Door->ActorHasTag(Tag))
			{
				Door->Unlock();
			}
		}
	}
}

void ABVSecurityTerminal::KillLightsInGroup()
{
	for (TActorIterator<ABVDestructibleLight> It(GetWorld()); It; ++It)
	{
		ABVDestructibleLight* Light = *It;
		if (Light && Light->LightGroupID == LightGroupToKill)
		{
			Light->SetLightOn(false);
		}
	}
}

void ABVSecurityTerminal::DisableFor_Implementation(float Duration)
{
	bDisabled = true;
	if (ScreenLight) { ScreenLight->SetIntensity(20.f); }

	if (Duration > 0.f)
	{
		FTimerHandle T;
		GetWorldTimerManager().SetTimer(T, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			bDisabled = false;
			if (ScreenLight) { ScreenLight->SetIntensity(180.f); }
		}), Duration, false);
	}
}

void ABVSecurityTerminal::RestorePower_Implementation()
{
	bPowered = true;
	bDisabled = false;
	if (ScreenLight) { ScreenLight->SetIntensity(180.f); }
}
