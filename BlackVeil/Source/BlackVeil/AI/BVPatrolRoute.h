// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BVPatrolComponent.h"
#include "BVPatrolRoute.generated.h"

/**
 * A named patrol route placed in the level. Guards look routes up by RouteID at
 * spawn, which lets a single route be shared by several guards on a schedule.
 */
UCLASS()
class BLACKVEIL_API ABVPatrolRoute : public AActor
{
	GENERATED_BODY()

public:
	ABVPatrolRoute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Patrol") FName RouteID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Patrol") EBVPatrolMode PatrolMode = EBVPatrolMode::Loop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Patrol") TArray<FBVPatrolWaypoint> Waypoints;

	/** Populate a guard's patrol component from this route. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Patrol")
	void ApplyToPatrolComponent(UBVPatrolComponent* Patrol) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<USceneComponent> Root;
};
