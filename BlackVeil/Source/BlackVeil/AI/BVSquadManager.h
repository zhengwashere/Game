// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/BVTypes.h"
#include "BVSquadManager.generated.h"

class ABVAIController;
class ABVGuardCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBVOnSquadAlarm, FName, SquadID, bool, bAlarmed, FVector, AlarmLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnGuardDownReported, FName, SquadID, FVector, Location);

/**
 * Central coordination for all guards in a level.
 *
 * Guards register here on possession. When one spots, hears or is taken down by
 * something, it reports to this manager, which decides who else needs to know
 * and shares only *approximate* information (a last-known location and a
 * suggested alert state) rather than the player's live position.
 *
 * Search coordination: when a squad searches, this manager divides the area
 * around the contact point into sectors and hands each guard a different one,
 * so they sweep the space instead of conga-lining to the same spot.
 */
UCLASS()
class BLACKVEIL_API UBVSquadManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad") void RegisterGuard(ABVAIController* Controller);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad") void UnregisterGuard(ABVAIController* Controller);

	/** Share a contact with everyone in the squad except the reporter. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad")
	void BroadcastContact(FName SquadID, const FVector& ApproxLocation, uint8 SuggestedAlertState, AActor* Spotter);

	/** A guard went down; nearby guards will investigate and the squad escalates. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad")
	void ReportGuardDown(ABVGuardCharacter* DownedGuard, const FVector& Location);

	/** Raise or clear the squad-wide alarm. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad")
	void SetSquadAlarm(FName SquadID, bool bAlarmed, const FVector& AlarmLocation);

	/** Request reinforcements to a location (spawned by mission logic). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad")
	void CallReinforcements(FName SquadID, const FVector& Location);

	/** Assign each squad member a distinct search sector around a centre point. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Squad")
	void CoordinateSearch(FName SquadID, const FVector& Center, float Radius);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Squad")
	TArray<ABVAIController*> GetSquadMembers(FName SquadID) const;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Squad")
	bool IsSquadAlarmed(FName SquadID) const;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Squad") FBVOnSquadAlarm OnSquadAlarm;
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Squad") FBVOnGuardDownReported OnGuardDownReported;

private:
	/** SquadID -> live controllers. */
	UPROPERTY() TMap<FName, TArray<TWeakObjectPtr<ABVAIController>>> Squads;
	/** SquadID -> alarmed flag. */
	UPROPERTY() TSet<FName> AlarmedSquads;
};
