// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Investigate.generated.h"

/** Moves to a suspicious location, looks around, then reports done. */
UCLASS()
class BLACKVEIL_API UBTTask_Investigate : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_Investigate();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
protected:
	UPROPERTY(EditAnywhere, Category = "Investigate") float LookAroundDuration = 4.f;
	UPROPERTY(EditAnywhere, Category = "Investigate") float AcceptanceRadius = 120.f;
};
