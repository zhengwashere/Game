// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVPatrolRoute.h"
#include "Components/SceneComponent.h"
#include "BlackVeil.h"

ABVPatrolRoute::ABVPatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ABVPatrolRoute::ApplyToPatrolComponent(UBVPatrolComponent* Patrol) const
{
	if (!Patrol) { return; }

	Patrol->PatrolRouteID = RouteID;
	Patrol->PatrolMode = PatrolMode;
	Patrol->Waypoints = Waypoints;
	Patrol->SetWaypointIndex(0);
}
