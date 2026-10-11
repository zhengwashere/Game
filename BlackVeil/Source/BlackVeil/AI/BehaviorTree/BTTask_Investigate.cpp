// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BTTask_Investigate.h"
#include "AI/BVAIController.h"
#include "AI/BVGuardCharacter.h"
#include "AI/BVAlertStateComponent.h"
#include "AI/BVBlackboardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"

struct FBTInvestigateMemory
{
	bool bArrived = false;
	float LookElapsed = 0.f;
	FVector Target = FVector::ZeroVector;
	FRotator ScanStart = FRotator::ZeroRotator;
};

UBTTask_Investigate::UBTTask_Investigate()
{
	NodeName = TEXT("Investigate Location");
	bNotifyTick = true;
}

uint16 UBTTask_Investigate::GetInstanceMemorySize() const { return sizeof(FBTInvestigateMemory); }

EBTNodeResult::Type UBTTask_Investigate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	if (!AIC) { return EBTNodeResult::Failed; }

	FBTInvestigateMemory* M = reinterpret_cast<FBTInvestigateMemory*>(NodeMemory);
	M->bArrived = false;
	M->LookElapsed = 0.f;

	// Prefer the investigate point, fall back to the last known contact.
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB && BB->GetValueAsBool(BVBlackboardKeys::HasLastKnownLocation))
	{
		M->Target = BB->GetValueAsVector(BVBlackboardKeys::LastKnownLocation);
	}
	else if (BB)
	{
		M->Target = BB->GetValueAsVector(BVBlackboardKeys::NoiseLocation);
	}

	if (M->Target.IsNearlyZero())
	{
		return EBTNodeResult::Failed;
	}

	M->ScanStart = AIC->GetPawn() ? AIC->GetPawn()->GetActorRotation() : FRotator::ZeroRotator;
	AIC->MoveToLocation(M->Target, AcceptanceRadius);
	return EBTNodeResult::InProgress;
}

void UBTTask_Investigate::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABVAIController* AIC = Cast<ABVAIController>(OwnerComp.GetAIOwner());
	if (!AIC || !AIC->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FBTInvestigateMemory* M = reinterpret_cast<FBTInvestigateMemory*>(NodeMemory);

	if (!M->bArrived)
	{
		if (FVector::Dist(AIC->GetPawn()->GetActorLocation(), M->Target) <= AcceptanceRadius)
		{
			M->bArrived = true;
			M->LookElapsed = 0.f;
		}
		return;
	}

	// Sweep the head left and right to visibly "search" the spot.
	M->LookElapsed += DeltaSeconds;
	const float Sweep = FMath::Sin(M->LookElapsed * 1.6f) * 70.f;
	AIC->GetPawn()->SetActorRotation(FRotator(0.f, M->ScanStart.Yaw + Sweep, 0.f));

	if (M->LookElapsed >= LookAroundDuration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
