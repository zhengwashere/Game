// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsAlertState.generated.h"

/**
 * Passes when the guard's alert state is at least the configured minimum.
 * Used to gate combat/search branches without duplicating state logic in the tree.
 */
UCLASS()
class BLACKVEIL_API UBTDecorator_IsAlertState : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_IsAlertState();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
protected:
	/** Minimum alert state ordinal (see EBVAlertState). */
	UPROPERTY(EditAnywhere, Category = "Condition") int32 MinAlertState = 3;
	UPROPERTY(EditAnywhere, Category = "Condition") bool bRequireExactMatch = false;
};
