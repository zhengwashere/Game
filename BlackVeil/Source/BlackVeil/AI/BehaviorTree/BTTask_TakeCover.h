// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TakeCover.generated.h"

/** Finds a point that breaks line of sight to the threat and moves there. */
UCLASS()
class BLACKVEIL_API UBTTask_TakeCover : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_TakeCover();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Cover") float SearchRadius = 900.f;
	UPROPERTY(EditAnywhere, Category = "Cover") int32 Samples = 16;
};
