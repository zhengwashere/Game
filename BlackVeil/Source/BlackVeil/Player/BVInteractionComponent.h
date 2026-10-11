// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BVTypes.h"
#include "BVInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnInteractionPromptChanged, const FBVInteractionPrompt&, Prompt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnInteractionExecuted, FName, ActionID, AActor*, Target);

/**
 * Scans for interactable objects in front of the player, maintains the current
 * focus (highest-priority candidate), builds the on-screen prompt and routes
 * the player's chosen action back to the target.
 *
 * Priority order when several objects overlap:
 *   1. Closest to the camera's aim ray
 *   2. Higher category weight (Intel/AlarmPanel beat generic Doors)
 *   3. Closest in world distance
 */
UCLASS(ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVInteractionComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Player pressed the interact key. Starts a hold if the prompt requires it. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Interaction")
	void BeginInteract();

	/** Player released the interact key. Cancels an incomplete hold. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Interaction")
	void EndInteract();

	/** Rotate to the next available action for the focused object. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Interaction")
	void CycleAction();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Interaction")
	FBVInteractionPrompt GetCurrentPrompt() const { return CurrentPrompt; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Interaction")
	AActor* GetFocusedActor() const { return FocusedActor.Get(); }

	/** Force-execute a specific action on the focused actor (used by UI menus). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Interaction")
	bool ExecuteActionOnFocus(FName ActionID);

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Interaction")
	FBVOnInteractionPromptChanged OnPromptChanged;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Interaction")
	FBVOnInteractionExecuted OnInteractionExecuted;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Interaction|Tuning") float MaxInteractRange = 260.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Interaction|Tuning") float ScanInterval = 0.12f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Interaction|Tuning") float AimConeDegrees = 45.f;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Interaction") TWeakObjectPtr<AActor> FocusedActor;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Interaction") FBVInteractionPrompt CurrentPrompt;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Interaction") TArray<FName> FocusedActions;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Interaction") int32 SelectedActionIndex = 0;

private:
	void ScanForInteractables();
	void UpdatePromptForFocus();
	int32 GetCategoryWeight(EBVInteractionType Type) const;

	float TimeSinceScan = 0.f;
	bool bHoldActive = false;
	float HoldElapsed = 0.f;
	FTimerHandle HoldTimerHandle;
};
