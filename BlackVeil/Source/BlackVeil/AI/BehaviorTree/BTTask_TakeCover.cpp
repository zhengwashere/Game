// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BTTask_TakeCover.h"
#include "AI/BVAIController.h"
#include "AI/BVBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTTask_TakeCover::UBTTask_TakeCover()
{
	NodeName = TEXT("Take Cover");
}

EBTNodeResult::Type UBTTask_TakeCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB || !AIC->GetPawn()) { return EBTNodeResult::Failed; }

	const FVector Self = AIC->GetPawn()->GetActorLocation();
	const FVector Threat = BB->GetValueAsBool(BVBlackboardKeys::HasLastKnownLocation)
		? BB->GetValueAsVector(BVBlackboardKeys::LastKnownLocation) : FVector::ZeroVector;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIC->GetWorld());
	const float AngleOffset = FMath::FRandRange(0.f, 360.f);

	FVector BestPoint = Self;
	float BestScore = -FLT_MAX;

	for (int32 i = 0; i < Samples; ++i)
	{
		const float Angle = FMath::DegreesToRadians(AngleOffset + (360.f / Samples) * i);
		const FVector Candidate = Self + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * SearchRadius;

		FNavLocation Projected;
		if (!NavSys || !NavSys->ProjectPointToNavigation(Candidate, Projected, FVector(400.f, 400.f, 500.f)))
		{
			continue;
		}

		// Score: we want to be out of the threat's line of sight, but not so far
		// that the guard abandons the fight.
		const FVector Point = Projected.Location;
		float Score = 0.f;

		if (!Threat.IsNearlyZero())
		{
			FCollisionQueryParams Params(SCENE_QUERY_STAT(BVCoverTest), false, AIC->GetPawn());
			FHitResult Hit;
			const bool bBlocked = AIC->GetWorld()->LineTraceSingleByChannel(Hit,
				Threat + FVector(0.f, 0.f, 60.f), Point + FVector(0.f, 0.f, 60.f), ECC_Visibility, Params);
			if (bBlocked) { Score += 200.f; }
			Score -= FVector::Dist(Point, Threat) * 0.05f;
		}

		if (Score > BestScore)
		{
			BestScore = Score;
			BestPoint = Point;
		}
	}

	BB->SetValueAsVector(BVBlackboardKeys::CoverLocation, BestPoint);
	AIC->MoveToLocation(BestPoint, 80.f);
	return EBTNodeResult::Succeeded;
}
