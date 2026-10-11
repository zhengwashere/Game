// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Search.generated.h"

/**
 * Generates a ring of search points around the last known contact and visits
 * them one at a time. Combined with the squad manager's sector assignment this
 * produces a spreading sweep rather than a pile-up.
 */
UCLASS()
class BLACKVEIL_API UBTTask_Search : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_Search();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
protected:
	UPROPERTY(EditAnywhere, Category = "Search") int32 SearchPoints = 6;
	UPROPERTY(EditAnywhere, Category = "Search") float SearchRadius = 1400.f;
	UPROPERTY(EditAnywhere, Category = "Search") float AcceptanceRadius = 130.f;
	UPROPERTY(EditAnywhere, Category = "Search") float PauseAtPoint = 1.5f;
};
