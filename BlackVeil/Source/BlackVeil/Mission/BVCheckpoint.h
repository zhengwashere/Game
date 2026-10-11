// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BVCheckpoint.generated.h"

class UBoxComponent;
class ABVPlayerCharacter;

/**
 * Respawn point. Walking through records the player's transform, the mission
 * timer and objective progress, so a restart resumes here instead of the level
 * beginning.
 */
UCLASS()
class BLACKVEIL_API ABVCheckpoint : public AActor
{
	GENERATED_BODY()

public:
	ABVCheckpoint();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Checkpoint") FName CheckpointID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Checkpoint") int32 Order = 0;

	/** When true the checkpoint can be re-triggered (e.g. resupply points). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Checkpoint") bool bRepeatable = false;

	/** Restores the player to this checkpoint. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Checkpoint") void RestoreCheckpoint();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UBoxComponent> TriggerVolume;

	UFUNCTION() void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Transform the player is restored to. */
	UPROPERTY() FTransform SavedTransform;
	UPROPERTY() float SavedMissionTime = 0.f;
	UPROPERTY() bool bHasBeenReached = false;
};
