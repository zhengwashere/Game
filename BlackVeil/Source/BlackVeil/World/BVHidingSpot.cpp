// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVHidingSpot.h"
#include "AI/BVGuardCharacter.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVTakedownComponent.h"
#include "Player/BVStealthComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVHidingSpot::ABVHidingSpot()
{
	InteractionType = EBVInteractionType::HidingSpot;
	PromptText = NSLOCTEXT("BlackVeil", "HidingSpot", "Hiding Spot");
	IconTag = TEXT("HidingSpot");
	AvailableActions = { TEXT("Hide.Self") };
}

bool ABVHidingSpot::CanAcceptBody() const
{
	return StoredBodies.Num() < BodyCapacity;
}

FVector ABVHidingSpot::GetConcealmentLocation() const
{
	return GetActorLocation() + FVector(0.f, 0.f, 20.f);
}

TArray<FName> ABVHidingSpot::GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const
{
	TArray<FName> Actions;

	if (bPlayerHidden)
	{
		Actions.Add(TEXT("Hide.Exit"));
		return Actions;
	}

	if (bPlayerCanHide)
	{
		Actions.Add(TEXT("Hide.Self"));
	}

	// Offer body concealment when the player is dragging someone.
	if (Interactor && Interactor->GetTakedownComponent() && Interactor->GetTakedownComponent()->IsCarryingBody())
	{
		if (CanAcceptBody())
		{
			Actions.Add(TEXT("Hide.Body"));
		}
	}

	return Actions;
}

bool ABVHidingSpot::ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor)
{
	if (ActionID == TEXT("Hide.Self"))
	{
		ConcealPlayer(Interactor);
	}
	else if (ActionID == TEXT("Hide.Exit"))
	{
		EjectPlayer(Interactor);
	}
	else if (ActionID == TEXT("Hide.Body"))
	{
		if (!Interactor || !Interactor->GetTakedownComponent()) { return false; }
		return Interactor->GetTakedownComponent()->HideCarriedBodyIn(this);
	}
	else
	{
		return false;
	}

	return Super::ExecuteInteraction_Implementation(ActionID, Interactor);
}

void ABVHidingSpot::ConcealBody(ABVGuardCharacter* Guard)
{
	if (!Guard || !CanAcceptBody()) { return; }

	StoredBodies.Add(Guard);
	Guard->ConcealIn(this);

	UE_LOG(LogBlackVeil, Log, TEXT("Body stored in %s (%d/%d)"), *GetName(), StoredBodies.Num(), BodyCapacity);
}

void ABVHidingSpot::ConcealPlayer(ABVPlayerCharacter* Player)
{
	if (!Player || !bPlayerCanHide) { return; }

	bPlayerHidden = true;

	// Teleport the operative inside and mark them as fully concealed.
	Player->SetActorLocation(GetConcealmentLocation());

	if (UBVStealthComponent* Stealth = Player->GetStealthComponent())
	{
		Stealth->SetPlayerPosture(EBVPosture::InCover);
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Player hidden in %s"), *GetName());
}

void ABVHidingSpot::EjectPlayer(ABVPlayerCharacter* Player)
{
	if (!Player) { return; }

	bPlayerHidden = false;

	// Pop the player back out in front of the container.
	const FVector ExitLocation = GetActorLocation() + GetActorForwardVector() * 110.f;
	Player->SetActorLocation(ExitLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (UBVStealthComponent* Stealth = Player->GetStealthComponent())
	{
		Stealth->SetPlayerPosture(EBVPosture::Standing);
	}
}
