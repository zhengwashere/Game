// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/BVInteractableActor.h"
#include "BVHidingSpot.generated.h"

class ABVGuardCharacter;

UENUM(BlueprintType)
enum class EBVHidingSpotType : uint8
{
	Locker    UMETA(DisplayName = "Locker"),
	Closet    UMETA(DisplayName = "Closet"),
	Container UMETA(DisplayName = "Container"),
	UnderDesk UMETA(DisplayName = "Under Desk"),
	Vehicle   UMETA(DisplayName = "Vehicle")
};

/**
 * Concealment furniture. Guards can be stuffed inside to keep patrols from
 * finding them, and the player can hide to break line of sight entirely.
 */
UCLASS()
class BLACKVEIL_API ABVHidingSpot : public ABVInteractableActor
{
	GENERATED_BODY()

public:
	ABVHidingSpot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Hiding") EBVHidingSpotType SpotType = EBVHidingSpotType::Locker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Hiding") int32 BodyCapacity = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Hiding") bool bPlayerCanHide = true;

	virtual TArray<FName> GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual bool ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor) override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hiding") bool CanAcceptBody() const;
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Hiding") void ConcealBody(ABVGuardCharacter* Guard);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Hiding") void ConcealPlayer(ABVPlayerCharacter* Player);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Hiding") void EjectPlayer(ABVPlayerCharacter* Player);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hiding") bool IsPlayerHidden() const { return bPlayerHidden; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hiding") int32 GetStoredBodyCount() const { return StoredBodies.Num(); }

	/** Location where concealed bodies are placed (inside the container). */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Hiding") FVector GetConcealmentLocation() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hiding") TArray<TWeakObjectPtr<ABVGuardCharacter>> StoredBodies;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Hiding") bool bPlayerHidden = false;
};
