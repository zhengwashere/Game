// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVInteractionComponent.h"
#include "BVPlayerCharacter.h"
#include "Interaction/BVInteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "EngineUtils.h"
#include "BlackVeil.h"

UBVInteractionComponent::UBVInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void UBVInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

int32 UBVInteractionComponent::GetCategoryWeight(EBVInteractionType Type) const
{
	// Higher = more important when multiple objects overlap.
	switch (Type)
	{
	case EBVInteractionType::Intel:       return 100;
	case EBVInteractionType::AlarmPanel:  return 90;
	case EBVInteractionType::Body:        return 80;
	case EBVInteractionType::HidingSpot:  return 70;
	case EBVInteractionType::Terminal:    return 60;
	case EBVInteractionType::Camera:      return 55;
	case EBVInteractionType::Door:        return 50;
	case EBVInteractionType::Lock:        return 45;
	case EBVInteractionType::Switch:      return 40;
	case EBVInteractionType::Pickup:      return 35;
	case EBVInteractionType::Vent:        return 30;
	case EBVInteractionType::Ladder:      return 25;
	default:                              return 10;
	}
}

void UBVInteractionComponent::ScanForInteractables()
{
	ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner());
	UWorld* World = GetWorld();
	if (!Player || !World) { return; }

	const FVector PlayerLoc = Player->GetActorLocation() + FVector(0.f, 0.f, 40.f);
	const FVector ViewDir = Player->GetControlRotation().Vector();

	AActor* BestActor = nullptr;
	float BestScore = -FLT_MAX;

	// Gather interactables within range. Interactables implement the interface,
	// so we filter by interface rather than by class.
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor == Player) { continue; }
		if (!Actor->GetClass()->ImplementsInterface(UBVInteractableInterface::StaticClass())) { continue; }

		const float Dist = FVector::Dist(PlayerLoc, Actor->GetActorLocation());
		if (Dist > MaxInteractRange) { continue; }

		const FVector ToActor = (Actor->GetActorLocation() - PlayerLoc).GetSafeNormal();
		const float Dot = FVector::DotProduct(ViewDir, ToActor);
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));
		if (Angle > AimConeDegrees) { continue; }

		// Must have clear line of sight.
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(BVInteractLOS), false, Player);
		if (World->LineTraceSingleByChannel(Hit, PlayerLoc, Actor->GetActorLocation(), ECC_Visibility, Params))
		{
			if (Hit.GetActor() != Actor) { continue; }
		}

		const EBVInteractionType Type = IBVInteractableInterface::Execute_GetInteractionType(Actor);
		// Score: aim accuracy dominates, category weight breaks ties, distance last.
		const float Score = (Dot * 100.f) + GetCategoryWeight(Type) - (Dist * 0.05f);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestActor = Actor;
		}
	}

	// Update highlight on focus change.
	if (FocusedActor.Get() != BestActor)
	{
		if (AActor* Old = FocusedActor.Get())
		{
			IBVInteractableInterface::Execute_SetInteractionHighlight(Old, 0.f);
		}
		FocusedActor = BestActor;
		SelectedActionIndex = 0;
		if (BestActor)
		{
			IBVInteractableInterface::Execute_SetInteractionHighlight(BestActor, 1.f);
			FocusedActions = IBVInteractableInterface::Execute_GetInteractionActions(BestActor, Player);
		}
		else
		{
			FocusedActions.Reset();
		}
		UpdatePromptForFocus();
	}
}

void UBVInteractionComponent::UpdatePromptForFocus()
{
	ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner());
	AActor* Target = FocusedActor.Get();
	if (!Target || !Player)
	{
		CurrentPrompt = FBVInteractionPrompt();
		OnPromptChanged.Broadcast(CurrentPrompt);
		return;
	}

	FBVInteractionPrompt Prompt = IBVInteractableInterface::Execute_GetInteractionPrompt(Target, Player);

	// If several actions exist, surface the currently selected one in the label.
	if (FocusedActions.Num() > 1)
	{
		const int32 Idx = FMath::Clamp(SelectedActionIndex, 0, FocusedActions.Num() - 1);
		const FName Action = FocusedActions[Idx];
		Prompt.ActionText = FText::FromString(FString::Printf(TEXT("%s  [%d/%d]  (Cycle: R)"),
			*Action.ToString(), Idx + 1, FocusedActions.Num()));
	}
	else if (FocusedActions.Num() == 1)
	{
		Prompt.ActionText = FText::FromString(FocusedActions[0].ToString());
	}

	CurrentPrompt = Prompt;
	OnPromptChanged.Broadcast(CurrentPrompt);
}

void UBVInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceScan += DeltaTime;
	if (TimeSinceScan >= ScanInterval)
	{
		TimeSinceScan = 0.f;
		ScanForInteractables();
	}

	if (bHoldActive)
	{
		HoldElapsed += DeltaTime;
		if (CurrentPrompt.HoldDuration > 0.f && HoldElapsed >= CurrentPrompt.HoldDuration)
		{
			bHoldActive = false;
			AActor* Target = FocusedActor.Get();
			if (Target)
			{
				const FName Action = FocusedActions.Num() > 0
					? FocusedActions[FMath::Clamp(SelectedActionIndex, 0, FocusedActions.Num() - 1)]
					: NAME_None;
				ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner());
				if (IBVInteractableInterface::Execute_ExecuteInteraction(Target, Action, Player))
				{
					OnInteractionExecuted.Broadcast(Action, Target);
				}
			}
		}
	}
}

void UBVInteractionComponent::BeginInteract()
{
	AActor* Target = FocusedActor.Get();
	if (!Target) { return; }

	if (CurrentPrompt.HoldDuration <= 0.f)
	{
		ExecuteActionOnFocus(FocusedActions.Num() > 0
			? FocusedActions[FMath::Clamp(SelectedActionIndex, 0, FocusedActions.Num() - 1)] : NAME_None);
		return;
	}

	bHoldActive = true;
	HoldElapsed = 0.f;
}

void UBVInteractionComponent::EndInteract()
{
	bHoldActive = false;
	HoldElapsed = 0.f;
}

void UBVInteractionComponent::CycleAction()
{
	if (FocusedActions.Num() <= 1) { return; }
	SelectedActionIndex = (SelectedActionIndex + 1) % FocusedActions.Num();
	UpdatePromptForFocus();
}

bool UBVInteractionComponent::ExecuteActionOnFocus(FName ActionID)
{
	AActor* Target = FocusedActor.Get();
	ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner());
	if (!Target || !Player) { return false; }

	const bool bHandled = IBVInteractableInterface::Execute_ExecuteInteraction(Target, ActionID, Player);
	if (bHandled)
	{
		OnInteractionExecuted.Broadcast(ActionID, Target);
	}
	return bHandled;
}
