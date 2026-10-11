// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BTTask_Search.h"
#include "AI/BVAIController.h"
#include "AI/BVGuardCharacter.h"
#include "AI/BVAlertStateComponent.h"
#include "AI/BVBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"

struct FBTSearchMemory
{
	int32 CurrentPoint = 0;
	bool bPaused = false;
	float PauseElapsed = 0.f;
	TArray<FVector> Points;
};

UBTTask_Search::UBTTask_Search()
{
	NodeName = TEXT("Coordinated Search");
	bNotifyTick = true;
}

uint16 UBTTask_Search::GetInstanceMemorySize() const { return sizeof(FBTSearchMemory); }

EBTNodeResult::Type UBTTask_Search::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) { return EBTNodeResult::Failed; }

	FBTSearchMemory* M = reinterpret_cast<FBTSearchMemory*>(NodeMemory);
	M->CurrentPoint = 0;
	M->bPaused = false;
	M->PauseElapsed = 0.f;
	M->Points.Reset();

	const FVector Center = BB->GetValueAsBool(BVBlackboardKeys::HasLastKnownLocation)
		? BB->GetValueAsVector(BVBlackboardKeys::LastKnownLocation)
		: (AIC->GetPawn() ? AIC->GetPawn()->GetActorLocation() : FVector::ZeroVector);

	// Build a ring of candidate points, projecting each onto the navmesh so the
	// guard never tries to walk into geometry.
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIC->GetWorld());
	for (int32 i = 0; i < SearchPoints; ++i)
	{
		const float Angle = (2.f * PI * i) / static_cast<float>(SearchPoints);
		const FVector Candidate = Center + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * SearchRadius;

		FNavLocation Projected;
		if (NavSys && NavSys->ProjectPointToNavigation(Candidate, Projected, FVector(300.f, 300.f, 400.f)))
		{
			M->Points.Add(Projected.Location);
		}
		else
		{
			M->Points.Add(Candidate);
		}
	}

	if (M->Points.Num() == 0) { return EBTNodeResult::Failed; }

	AIC->MoveToLocation(M->Points[0], AcceptanceRadius);
	return EBTNodeResult::InProgress;
}

void UBTTask_Search::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	if (!AIC || !AIC->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Abort the search immediately if we reacquire the target.
	if (UBVAlertStateComponent* Alert = AIC->GetAlertState())
	{
		if (Alert->GetAlertState() == EBVAlertState::Combat)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	FBTSearchMemory* M = reinterpret_cast<FBTSearchMemory*>(NodeMemory);
	if (M->Points.Num() == 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const FVector Target = M->Points[FMath::Clamp(M->CurrentPoint, 0, M->Points.Num() - 1)];

	if (!M->bPaused)
	{
		if (FVector::Dist(AIC->GetPawn()->GetActorLocation(), Target) <= AcceptanceRadius)
		{
			M->bPaused = true;
			M->PauseElapsed = 0.f;
		}
		return;
	}

	M->PauseElapsed += DeltaSeconds;
	if (M->PauseElapsed >= PauseAtPoint)
	{
		M->bPaused = false;
		++M->CurrentPoint;
		if (M->CurrentPoint >= M->Points.Num())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
		AIC->MoveToLocation(M->Points[M->CurrentPoint], AcceptanceRadius);
	}
}
