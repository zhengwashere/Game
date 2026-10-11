// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

/**
 * Blackboard key names shared between the AI controller and every behaviour
 * tree node. Defined once here to prevent typos drifting between files.
 */
namespace BVBlackboardKeys
{
	// Object keys
	const FName TargetActor(TEXT("TargetActor"));
	const FName SelfActor(TEXT("SelfActor"));

	// Vector keys
	const FName HomeLocation(TEXT("HomeLocation"));
	const FName PatrolLocation(TEXT("PatrolLocation"));
	const FName LastKnownLocation(TEXT("LastKnownLocation"));
	const FName InvestigateLocation(TEXT("InvestigateLocation"));
	const FName NoiseLocation(TEXT("NoiseLocation"));
	const FName CoverLocation(TEXT("CoverLocation"));

	// Bool keys
	const FName HasLastKnownLocation(TEXT("HasLastKnownLocation"));
	const FName HasPatrolRoute(TEXT("HasPatrolRoute"));
	const FName IsInCombat(TEXT("IsInCombat"));
	const FName CanSeePlayer(TEXT("CanSeePlayer"));
	const FName HasHeardNoise(TEXT("HasHeardNoise"));

	// Float / int keys
	const FName Suspicion(TEXT("Suspicion"));
	const FName AlertState(TEXT("AlertState"));
	const FName SearchIndex(TEXT("SearchIndex"));
	const FName WaitTimer(TEXT("WaitTimer"));
}
