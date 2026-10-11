// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdatePerception.generated.h"

/** Keeps the blackboard in sync with the controller's perception each tick. */
UCLASS()
class BLACKVEIL_API UBTService_UpdatePerception : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_UpdatePerception();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
