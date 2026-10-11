// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BTTask_Patrol.h"
#include "AI/BVAIController.h"
#include "AI/BVGuardCharacter.h"
#include "AI/BVPatrolComponent.h"
#include "AI/BVBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BlackVeil.h"

/** Per-instance memory so several guards can run this node independently. */
struct FBTPatrolMemory
{
	bool bWaiting = false;
	float WaitElapsed = 0.f;
	float WaitTarget = 0.f;
	FVector TargetLocation = FVector::ZeroVector;
};

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol Route");
	bNotifyTick = true;
}

uint16 UBTTask_Patrol::GetInstanceMemorySize() const
{
	return sizeof(FBTPatrolMemory);
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	if (!AIC) { return EBTNodeResult::Failed; }

	ABVGuardCharacter* Guard = AIC->GetGuard();
	UBVPatrolComponent* Patrol = Guard ? Guard->GetPatrolComponent() : nullptr;
	if (!Patrol || !Patrol->HasRoute()) { return EBTNodeResult::Failed; }

	FBTPatrolMemory* Memory = reinterpret_cast<FBTPatrolMemory*>(NodeMemory);
	Memory->bWaiting = false;
	Memory->WaitElapsed = 0.f;
	Memory->TargetLocation = Patrol->GetCurrentWaypointLocation();

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsVector(BVBlackboardKeys::PatrolLocation, Memory->TargetLocation);
	}

	AIC->MoveToLocation(Memory->TargetLocation, AcceptanceRadius);
	return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	ABVGuardCharacter* Guard = AIC ? AIC->GetGuard() : nullptr;
	UBVPatrolComponent* Patrol = Guard ? Guard->GetPatrolComponent() : nullptr;
	if (!AIC || !Patrol)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FBTPatrolMemory* Memory = reinterpret_cast<FBTPatrolMemory*>(NodeMemory);

	if (!Memory->bWaiting)
	{
		const float Dist = FVector::Dist(Guard->GetActorLocation(), Memory->TargetLocation);
		if (Dist <= AcceptanceRadius)
		{
			Memory->bWaiting = true;
			Memory->WaitElapsed = 0.f;
			Memory->WaitTarget = Patrol->bPauseAtWaypoints ? Patrol->GetCurrentWaypoint().WaitDuration : 0.f;

			// Face the configured look direction while waiting.
			const FBVPatrolWaypoint& WP = Patrol->GetCurrentWaypoint();
			if (!FMath::IsNearlyZero(WP.LookYawOffset))
			{
				const FRotator NewRot(0.f, WP.LookYawOffset, 0.f);
				Guard->SetActorRotation(NewRot);
			}
		}
	}
	else
	{
		Memory->WaitElapsed += DeltaSeconds;
		if (Memory->WaitElapsed >= Memory->WaitTarget)
		{
			Patrol->AdvanceWaypoint();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
