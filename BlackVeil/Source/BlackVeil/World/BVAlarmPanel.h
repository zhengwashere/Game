// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/BVInteractableActor.h"
#include "BVElectronicDeviceInterface.h"
#include "BVAlarmPanel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnAlarmTriggered, FName, AlarmGroupID, AActor*, Causer);

/**
 * A wall-mounted alarm control. Sabotaging it prevents the group from being
 * raised; deliberately triggering it is the loud option.
 */
UCLASS()
class BLACKVEIL_API ABVAlarmPanel : public ABVInteractableActor, public IBVElectronicDeviceInterface
{
	GENERATED_BODY()

public:
	ABVAlarmPanel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Alarm") FName AlarmGroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Alarm") bool bSabotaged = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Alarm") bool bAlarmActive = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Alarm") float ReinforcementsDelay = 25.f;

	virtual TArray<FName> GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual bool ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Alarm") void TriggerAlarm(AActor* Causer, AActor* Source);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Alarm") void ClearAlarm();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Alarm") void SabotagePanel();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Alarm") bool IsAlarmActive() const { return bAlarmActive; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Alarm") bool IsSabotaged() const { return bSabotaged; }

	virtual void DisableFor_Implementation(float Duration) override;
	virtual void RestorePower_Implementation() override;
	virtual bool IsDisabled_Implementation() const override { return bSabotaged; }
	virtual bool HasPower_Implementation() const override { return !bSabotaged; }

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Alarm") FBVOnAlarmTriggered OnAlarmTriggered;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<class UPointLightComponent> StatusLight;
};
