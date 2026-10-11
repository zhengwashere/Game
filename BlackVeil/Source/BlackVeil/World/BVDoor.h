// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/BVInteractableActor.h"
#include "BVDoor.generated.h"

/** How a door is secured. Drives which gadgets or items can defeat it. */
UENUM(BlueprintType)
enum class EBVLockType : uint8
{
	Unlocked   UMETA(DisplayName = "Unlocked"),
	Mechanical UMETA(DisplayName = "Mechanical Lock"),
	Electronic UMETA(DisplayName = "Electronic Lock"),
	Keycard    UMETA(DisplayName = "Keycard Reader"),
	Biometric  UMETA(DisplayName = "Biometric Scanner"),
	Barred     UMETA(DisplayName = "Barred (Force Only)")
};

/**
 * An openable door with several interaction verbs.
 *
 * Players can crack it open quietly (slow, minimal noise), open it normally
 * (fast, noisy) or force it (loud, only for weak locks). Locked doors require
 * the matching tool, keycard or access code.
 */
UCLASS()
class BLACKVEIL_API ABVDoor : public ABVInteractableActor
{
	GENERATED_BODY()

public:
	ABVDoor();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") EBVLockType LockType = EBVLockType::Unlocked;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") int32 LockDifficulty = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") FName RequiredKeycardID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") bool bIsOpen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") bool bOpenInward = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") float OpenAngle = 92.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") float OpenSpeed = 110.f;

	/** When true the door is a two-leaf double door. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Door") bool bDoubleDoor = false;

	virtual FBVInteractionPrompt GetInteractionPrompt_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual TArray<FName> GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const override;
	virtual bool ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor) override;
	virtual bool CanInteract_Implementation(ABVPlayerCharacter* Interactor) const override;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Door") bool IsLocked() const { return LockType != EBVLockType::Unlocked; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Door") bool IsOpen() const { return bIsOpen; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void SetLocked(EBVLockType NewLockType);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void Unlock();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void OpenQuietly();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void OpenNormally();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void ForceOpen();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void CloseDoor();

	/** Called by EMP / power sabotage on electronic doors. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Door") void SetPowerEnabled(bool bEnabled);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<USceneComponent> Pivot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UStaticMeshComponent> SecondaryLeaf;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Door") float CurrentAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Door") float TargetAngle = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Door") bool bPowered = true;

	/** Reports the noise made by the last interaction. */
	void ReportDoorNoise(EBVNoiseLevel Level);
};
