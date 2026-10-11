// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/BVTypes.h"
#include "BVGuardCharacter.generated.h"

class USpotLightComponent;
class UBVPatrolComponent;
class UBVAlertStateComponent;
class UBehaviorTree;
class ABVHidingSpot;

/** Guard archetypes with distinct capabilities and behaviour biases. */
UENUM(BlueprintType)
enum class EBVGuardType : uint8
{
	Standard     UMETA(DisplayName = "Standard Guard"),
	Armored      UMETA(DisplayName = "Armored Guard"),
	Tech         UMETA(DisplayName = "Technician"),
	Surveillance UMETA(DisplayName = "Surveillance Operator"),
	Commander    UMETA(DisplayName = "Commander"),
	Elite        UMETA(DisplayName = "Elite Operative")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnGuardIncapacitated, ABVGuardCharacter*, Guard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnGuardAlertStateChanged, ABVGuardCharacter*, Guard, uint8, NewState);

/**
 * Base enemy guard. Handles health, incapacitation, the flashlight, patrol and
 * alert state components, and exposes the hooks the AI controller and the
 * player's takedown system call into.
 */
UCLASS()
class BLACKVEIL_API ABVGuardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABVGuardCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// ---- Identity ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Guard")
	EBVGuardType GuardType = EBVGuardType::Standard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Guard")
	FBVGuardPersonality Personality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Guard")
	FName SquadID = TEXT("Squad_A");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Guard")
	FName PatrolRouteID;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") EBVGuardType GetGuardType() const { return GuardType; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") UBVPatrolComponent* GetPatrolComponent() const { return PatrolComponent; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") UBVAlertStateComponent* GetAlertStateComponent() const { return AlertStateComponent; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") USpotLightComponent* GetFlashlight() const { return Flashlight; }

	// ---- Status ----
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool IsIncapacitated() const { return bIncapacitated; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool IsDead() const { return bDead; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool IsBeingCarried() const { return bBeingCarried; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool IsConcealed() const { return bConcealed; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool IsAlertedToPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void ApplyDamageToGuard(float Amount, AActor* Causer);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void ReceiveTakedown(EBVTakedownType Type, EBVLethality Lethality);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void SetBeingCarried(bool bCarried);

	/** Conceal this body inside a hiding spot; it stops being discoverable. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void ConcealIn(ABVHidingSpot* Spot);

	/** Called by other guards / the squad when this guard fails to report in. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void OnDiscoveredBy(ABVGuardCharacter* Discoverer);

	/** Toggle the flashlight (surveillance guards keep theirs on in dark zones). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void SetFlashlightEnabled(bool bEnabled);

	/** Apply a temporary status effect (blind, stun, deafen...). */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void ApplyStatusEffect(EBVStatusEffect Effect, float Duration);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool HasStatusEffect(EBVStatusEffect Effect) const;

	/** Sight multiplier from active status effects (blinded guards see almost nothing). */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") float GetSightMultiplier() const;

	/** Hearing multiplier from active status effects. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") float GetHearingMultiplier() const;

	// ---- Combat / weapons ----
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Guard") void FireAtTarget(AActor* Target);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Guard") bool CanFire() const;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Guard") FBVOnGuardIncapacitated OnGuardIncapacitated;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<USpotLightComponent> Flashlight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UBVPatrolComponent> PatrolComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UBVAlertStateComponent> AlertStateComponent;

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Guard") float MaxHealth = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Guard") float ArmorMitigation = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Guard") float WeaponRange = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Guard") float FireInterval = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Guard") float WeaponDamage = 12.f;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Guard") float Health = 100.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Guard") bool bIncapacitated = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Guard") bool bDead = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Guard") bool bBeingCarried = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Guard") bool bConcealed = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Guard") TWeakObjectPtr<ABVHidingSpot> ConcealmentSpot;

	float LastFireTime = -100.f;

	/** Effect -> world time at which it expires. */
	UPROPERTY() TMap<EBVStatusEffect, float> StatusEffectEndTimes;

	/** Disables collision, movement and AI, and plays the appropriate pose. */
	void EnterIncapacitatedState(EBVLethality Lethality);
};
