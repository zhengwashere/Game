// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BTService_UpdatePerception.h"
#include "AI/BVAIController.h"
#include "AI/BVGuardCharacter.h"
#include "AI/BVPatrolComponent.h"
#include "AI/BVBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_UpdatePerception::UBTService_UpdatePerception()
{
	NodeName = TEXT("Update Perception");
	Interval = 0.25f;
	RandomDeviation = 0.05f;
}

void UBTService_UpdatePerception::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) { return; }

	// Refresh perception-derived keys.
	AIC->SyncBlackboard();

	// Keep the patrol target current so the patrol task always has a destination.
	if (const ABVGuardCharacter* Guard = AIC->GetGuard())
	{
		if (const UBVPatrolComponent* Patrol = Guard->GetPatrolComponent())
		{
			BB->SetValueAsBool(BVBlackboardKeys::HasPatrolRoute, Patrol->HasRoute());
			if (Patrol->HasRoute())
			{
				BB->SetValueAsVector(BVBlackboardKeys::PatrolLocation, Patrol->GetCurrentWaypointLocation());
			}
		}
	}
}
