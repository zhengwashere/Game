// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/BVTypes.h"
#include "BVInteractableInterface.generated.h"

class ABVPlayerCharacter;

UINTERFACE(MinimalAPI, Blueprintable)
class UBVInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Contract implemented by every object the player can interact with.
 *
 * The interaction component queries these to build prompts, then routes the
 * player's chosen action back through ExecuteInteraction. Multiple actions
 * (e.g. "open quietly" vs "force open") are surfaced via GetInteractionActions.
 */
class BLACKVEIL_API IBVInteractableInterface
{
	GENERATED_BODY()

public:
	/** Primary prompt shown when the player looks at this object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Interaction")
	FBVInteractionPrompt GetInteractionPrompt(ABVPlayerCharacter* Interactor) const;

	/** All actions available, e.g. Door.Open / Door.OpenQuiet / Door.Force. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Interaction")
	TArray<FName> GetInteractionActions(ABVPlayerCharacter* Interactor) const;

	/** Perform one of the actions returned above. Returns true if handled. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Interaction")
	bool ExecuteInteraction(FName ActionID, ABVPlayerCharacter* Interactor);

	/** Whether interaction is currently possible (unlocked, powered, in range...). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Interaction")
	bool CanInteract(ABVPlayerCharacter* Interactor) const;

	/** Highlight intensity for the focus outline (0 = off). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Interaction")
	void SetInteractionHighlight(float Intensity);

	/** Interaction category used for prioritization when several overlaps exist. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlackVeil|Interaction")
	EBVInteractionType GetInteractionType() const;
};
