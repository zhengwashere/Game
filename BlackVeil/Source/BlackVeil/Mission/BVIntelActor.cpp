// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVIntelActor.h"
#include "BVMissionManager.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVIntelActor::ABVIntelActor()
{
	PromptText = NSLOCTEXT("BlackVeil", "CollectIntel", "Collect Intelligence");
	IconTag = TEXT("Intel");
	InteractionType = EBVInteractionType::Intel;
	AvailableActions = { TEXT("Intel.Collect") };

	GlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GlowLight"));
	GlowLight->SetupAttachment(MeshComponent);
	GlowLight->SetIntensity(600.f);
	GlowLight->SetAttenuationRadius(300.f);
	GlowLight->SetLightColor(FLinearColor(0.2f, 0.8f, 1.f));
}

bool ABVIntelActor::ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor)
{
	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->CollectIntel(IntelID);
		}
	}

	// Remove from the world once collected.
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	return Super::ExecuteInteraction_Implementation(ActionID, Interactor);
}
