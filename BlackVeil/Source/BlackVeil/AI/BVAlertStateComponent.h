// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BVAlertStateComponent.generated.h"

/** The guard awareness ladder, from blissfully unaware to active combat. */
UENUM(BlueprintType)
enum class EBVAlertState : uint8
{
	Unaware       UMETA(DisplayName = "Unaware"),
	Suspicious    UMETA(DisplayName = "Suspicious"),
	Investigating UMETA(DisplayName = "Investigating"),
	Searching     UMETA(DisplayName = "Searching"),
	Alerted       UMETA(DisplayName = "Alerted"),
	Combat        UMETA(DisplayName = "Combat"),
	Returning     UMETA(DisplayName = "Returning")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnAlertStateChanged, EBVAlertState, OldState, EBVAlertState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnSuspicionChanged, float, Suspicion);

/**
 * Per-guard awareness machine.
 *
 * A suspicion meter (0-100) rises while the guard can see or hear something
 * interesting and decays when they cannot. Reaching the top escalates the
 * state; timers then govern how long each state lasts before the guard
 * de-escalates back toward patrol.
 *
 * Crucially this component only ever receives *approximate* information
 * (a sighting position, a heard noise location) - it never queries the player
 * directly, so guards cannot see through walls.
 */
UCLASS(ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVAlertStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVAlertStateComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") EBVAlertState GetAlertState() const { return AlertState; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") float GetSuspicion() const { return Suspicion; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") FVector GetLastKnownPlayerLocation() const { return LastKnownPlayerLocation; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") bool HasLastKnownLocation() const { return bHasLastKnownLocation; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI") float GetTimeSinceLastSighting() const { return TimeSinceLastSighting; }

	/** Raise suspicion from a sighting. Location is the *observed* point, not the true player position. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI")
	void ReportSighting(const FVector& ObservedLocation, float Confidence);

	/** Register a heard noise; only escalates if the guard has no better information. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI")
	void ReportNoise(const FVector& NoiseLocation, float Loudness);

	/** Another guard shared a contact. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI")
	void ReceiveCommunicatedContact(const FVector& Location, EBVAlertState SuggestedState);

	/** Force a state (alarms, scripted events, mission logic). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI")
	void SetAlertState(EBVAlertState NewState);

	/** Called when the guard gives up and heads home. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|AI")
	void ReturnToPatrol();

	/** True while the guard should be actively hunting the player. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI")
	bool IsHunting() const;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|AI")
	static FString GetAlertStateName(EBVAlertState State);

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|AI") FBVOnAlertStateChanged OnAlertStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|AI") FBVOnSuspicionChanged OnSuspicionChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float SuspicionThreshold = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float SuspicionDecayRate = 6.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float SightSuspicionGain = 55.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float NoiseSuspicionGain = 18.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float InvestigatingDuration = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float SearchingDuration = 22.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float AlertedDuration = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|AI|Tuning") float CombatCooldown = 12.f;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") EBVAlertState AlertState = EBVAlertState::Unaware;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") float Suspicion = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") FVector LastKnownPlayerLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") bool bHasLastKnownLocation = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|AI") float TimeSinceLastSighting = 999.f;

private:
	float StateTimer = 0.f;
	void EvaluateEscalation();
	void EvaluateDeescalation(float DeltaTime);
};
