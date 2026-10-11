// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BTDecorator_IsAlertState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AI/BVBlackboardKeys.h"

UBTDecorator_IsAlertState::UBTDecorator_IsAlertState()
{
	NodeName = TEXT("Is Alert State");
}

bool UBTDecorator_IsAlertState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) { return false; }

	const int32 Current = BB->GetValueAsInt(BVBlackboardKeys::AlertState);
	return bRequireExactMatch ? (Current == MinAlertState) : (Current >= MinAlertState);
}
