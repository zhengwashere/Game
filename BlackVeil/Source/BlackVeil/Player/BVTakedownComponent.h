// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BVTypes.h"
#include "BVTakedownComponent.generated.h"

class ABVGuardCharacter;
class ABVHidingSpot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnTakedownPerformed, ABVGuardCharacter*, Target, EBVLethality, Lethality);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnBodyCarryChanged, bool, bCarrying);

/**
 * Handles contextual silent takedowns and body management.
 *
 * A takedown is offered when:
 *   - a living guard is within reach, and
 *   - the player is behind them (stealth), above them (drop attack), or
 *     around a corner they are not looking past.
 *
 * Incapacitated guards can be picked up, carried (slower, no sprint) and
 * concealed in hiding spots so patrols do not discover them.
 */
UCLASS(ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVTakedownComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVTakedownComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Attempt a takedown on the best candidate in range. Returns true if started. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Takedown")
	bool TryTakedown(EBVLethality Lethality);

	/** True when a takedown prompt should be shown. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Takedown")
	bool HasTakedownTarget() const { return BestTarget.IsValid(); }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Takedown")
	ABVGuardCharacter* GetTakedownTarget() const { return BestTarget.Get(); }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Takedown")
	EBVTakedownType GetTakedownType() const { return PendingType; }

	// ---- Body management ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Takedown")
	bool PickUpNearestBody();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Takedown")
	void DropCarriedBody();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Takedown")
	bool IsCarryingBody() const { return CarriedGuard.IsValid(); }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Takedown")
	ABVGuardCharacter* GetCarriedBody() const { return CarriedGuard.Get(); }

	/** Conceal the carried body inside a hiding spot (locker, container...). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Takedown")
	bool HideCarriedBodyIn(ABVHidingSpot* Spot);

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Takedown")
	FBVOnTakedownPerformed OnTakedownPerformed;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Takedown")
	FBVOnBodyCarryChanged OnBodyCarryChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Takedown|Tuning") float TakedownReach = 160.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Takedown|Tuning") float BehindAngleDegrees = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Takedown|Tuning") float BodyPickupReach = 180.f;

private:
	void EvaluateTakedownTargets();

	TWeakObjectPtr<ABVGuardCharacter> BestTarget;
	TWeakObjectPtr<ABVGuardCharacter> CarriedGuard;
	EBVTakedownType PendingType = EBVTakedownType::Behind;
	float TimeSinceEval = 0.f;
	float EvalInterval = 0.15f;
};
