// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVPatrolComponent.h"
#include "BlackVeil.h"

UBVPatrolComponent::UBVPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FVector UBVPatrolComponent::GetCurrentWaypointLocation() const
{
	if (Waypoints.Num() == 0)
	{
		return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
	}
	return Waypoints[FMath::Clamp(CurrentIndex, 0, Waypoints.Num() - 1)].Location;
}

void UBVPatrolComponent::AdvanceWaypoint()
{
	if (Waypoints.Num() == 0)
	{
		return;
	}

	switch (PatrolMode)
	{
	case EBVPatrolMode::Loop:
		CurrentIndex = (CurrentIndex + 1) % Waypoints.Num();
		break;

	case EBVPatrolMode::PingPong:
		CurrentIndex += Direction;
		if (CurrentIndex >= Waypoints.Num())
		{
			CurrentIndex = FMath::Max(Waypoints.Num() - 2, 0);
			Direction = -1;
		}
		else if (CurrentIndex < 0)
		{
			CurrentIndex = FMath::Min(1, Waypoints.Num() - 1);
			Direction = 1;
		}
		break;

	case EBVPatrolMode::OneShot:
		CurrentIndex = FMath::Min(CurrentIndex + 1, Waypoints.Num() - 1);
		break;
	}
}

void UBVPatrolComponent::SetWaypointIndex(int32 Index)
{
	CurrentIndex = FMath::Clamp(Index, 0, FMath::Max(Waypoints.Num() - 1, 0));
}

int32 UBVPatrolComponent::FindNearestWaypointIndex(const FVector& Location) const
{
	int32 BestIndex = 0;
	float BestDistSq = FLT_MAX;
	for (int32 i = 0; i < Waypoints.Num(); ++i)
	{
		const float DistSq = FVector::DistSquared(Location, Waypoints[i].Location);
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestIndex = i;
		}
	}
	return BestIndex;
}
