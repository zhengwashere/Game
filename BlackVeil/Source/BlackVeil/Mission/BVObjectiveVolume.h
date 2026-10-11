// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BVTypes.h"
#include "BVObjectiveVolume.generated.h"

class UBoxComponent;
class UBillboardComponent;
class ABVPlayerCharacter;

/**
 * World trigger that advances an objective. Designers drop these in a level,
 * give them an ObjectiveID that matches the mission data asset, and the
 * mission manager handles the rest.
 */
UCLASS()
class BLACKVEIL_API ABVObjectiveVolume : public AActor
{
	GENERATED_BODY()

public:
	ABVObjectiveVolume();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Objective") FName ObjectiveID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Objective") int32 ProgressAmount = 1;

	/** When true the volume fires once and then stops responding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Objective") bool bSingleUse = true;

	/** When true the player must interact rather than simply walk in. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackVeil|Objective") bool bRequiresInteraction = false;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Objective") void TriggerObjective(AActor* Triggerer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UBoxComponent> TriggerVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UBillboardComponent> EditorIcon;

	UFUNCTION() void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bHasFired = false;
};
