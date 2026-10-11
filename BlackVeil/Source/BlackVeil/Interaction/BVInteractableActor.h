// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BVInteractableInterface.h"
#include "BVInteractableActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnInteracted, FName, ActionID, AActor*, Interactor);

/**
 * Convenient base class for interaction objects. Implements highlighting,
 * prompt bookkeeping and action dispatch, and exposes delegates so level
 * logic (Blueprints) can respond to player actions without new C++ subclasses.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BLACKVEIL_API ABVInteractableActor : public AActor, public IBVInteractableInterface
{
	GENERATED_BODY()

public:
	ABVInteractableActor();

	// --- IBVInteractableInterface ---
	virtual FBVInteractionPrompt GetInteractionPrompt_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual TArray<FName> GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual bool ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor) override;
	virtual bool CanInteract_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual void SetInteractionHighlight_Implementation(float Intensity) override;
	virtual EBVInteractionType GetInteractionType_Implementation() const override;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Interaction")
	FBVOnInteracted OnInteracted;

protected:
	/** Message shown on the interaction prompt. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Interaction")
	FText PromptText;

	/** Semantic icon tag for the prompt (resolved by the HUD to an icon). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Interaction")
	FName IconTag;

	/** Seconds the player must hold the interact key; 0 = tap. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Interaction")
	float HoldDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Interaction")
	EBVInteractionType InteractionType = EBVInteractionType::None;

	/** Actions this object advertises; subclasses may add conditionally. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Interaction")
	TArray<FName> AvailableActions;

	/** Mesh used for the focus/highlight outline. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Interaction")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	/** When false, CanInteract returns false and the prompt is greyed out. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Interaction")
	bool bIsInteractable = true;
};
