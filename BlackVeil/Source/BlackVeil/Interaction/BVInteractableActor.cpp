// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Player/BVPlayerCharacter.h"

ABVInteractableActor::ABVInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);

	// Interactables respond to the interact trace channel by default.
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	MeshComponent->SetGenerateOverlapEvents(true);
}

FBVInteractionPrompt ABVInteractableActor::GetInteractionPrompt_Implementation(ABVPlayerCharacter* Interactor) const
{
	FBVInteractionPrompt Prompt;
	Prompt.ActionText = PromptText;
	Prompt.IconTag = IconTag;
	Prompt.HoldDuration = HoldDuration;
	Prompt.InteractionType = InteractionType;
	Prompt.TargetActor = const_cast<ABVInteractableActor*>(this);
	Prompt.bIsAvailable = CanInteract_Implementation(Interactor);
	if (!Prompt.bIsAvailable)
	{
		Prompt.UnavailableReason = NSLOCTEXT("BlackVeil", "Locked", "Locked");
	}
	return Prompt;
}

TArray<FName> ABVInteractableActor::GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const
{
	return AvailableActions;
}

bool ABVInteractableActor::ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return false;
	}
	OnInteracted.Broadcast(ActionID, Interactor);
	return true;
}

bool ABVInteractableActor::CanInteract_Implementation(ABVPlayerCharacter* Interactor) const
{
	return bIsInteractable;
}

void ABVInteractableActor::SetInteractionHighlight_Implementation(float Intensity)
{
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(Intensity > 0.f);
		MeshComponent->SetCustomDepthStencilValue(Intensity > 0.f ? 1 : 0);
	}
}

EBVInteractionType ABVInteractableActor::GetInteractionType_Implementation() const
{
	return InteractionType;
}
