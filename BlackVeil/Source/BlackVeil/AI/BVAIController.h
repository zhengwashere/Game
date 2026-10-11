// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Core/BVTypes.h"
#include "BVAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class ABVGuardCharacter;
class UBVAlertStateComponent;
class UBVSquadManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnAISpottedPlayer, AActor*, SpottedActor, FVector, ApproxLocation);

/**
 * Enemy AI controller.
 *
 * Owns the perception component, the blackboard and the behaviour tree, and
 * arbitrates between them. The critical design rule is that this class never
 * asks the game where the player *is* - it only forwards what the senses and
 * squad communication actually deliver.
 */
UCLASS()
class BLACKVEIL_API ABVAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABVAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	// ---- Accessors ----
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") ABVGuardCharacter* GetGuard() const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") UBVAlertStateComponent* GetAlertState() const;
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") UBVSquadManager* GetSquadManager() const { return SquadManager; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") UAIPerceptionComponent* GetBVPerception() const { return PerceptionComponent; }

	/** Current effective sight distance after lighting/posture modifiers. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") float GetEffectiveSightRange() const;

	/** Force the guard into a hunting state at an approximate location. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI") void InvestigateLocation(const FVector& Location);

	/** Called by the squad manager to share a contact. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI")
	void ReceiveSquadContact(const FVector& ApproxLocation, uint8 SuggestedAlertState, AActor* SharedBy);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI") void EnterCombat(AActor* Threat);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI") void ReportBodyFound(AActor* Body);

	/** Writes current perception/alert values into the blackboard. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI") void SyncBlackboard();

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|AI") FBVOnAISpottedPlayer OnSpottedPlayer;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|AI") TObjectPtr<UAIPerceptionComponent> PerceptionComponent;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
	/** Fetched from the world subsystem on possess; shared by all guards. */
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") TWeakObjectPtr<UBVSquadManager> SquadManager;

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI") TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float BaseSightRange = 2400.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float LoseSightRange = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float PeripheralVisionAngle = 62.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float HearingRange = 1700.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float DarkSightPenalty = 0.35f;

	/** Vision confirmation: even a perceived target must pass this to count. */
	bool ValidateSight(AActor* SeenActor, const FVector& SeenLocation) const;

	UFUNCTION()
	void HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	void ConfigurePerception();

	/** Time of last successful sighting, used for reaction delay gating. */
	float LastSightTime = -100.f;
	TWeakObjectPtr<AActor> PendingSightTarget;
};
