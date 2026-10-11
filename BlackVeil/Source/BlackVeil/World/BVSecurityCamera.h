// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/BVInteractableActor.h"
#include "BVElectronicDeviceInterface.h"
#include "BVSecurityCamera.generated.h"

class USceneComponent;
class USpotLightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnCameraDetected, ABVSecurityCamera*, Camera, AActor*, DetectedActor);

/**
 * Sweeping surveillance camera. Detects the player inside its cone when they are
 * sufficiently visible, and reports to its alarm group. Can be disabled by
 * hacking, EMP, or shooting it out.
 */
UCLASS()
class BLACKVEIL_API ABVSecurityCamera : public ABVInteractableActor, public IBVElectronicDeviceInterface
{
	GENERATED_BODY()

public:
	ABVSecurityCamera();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") FName AlarmGroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float DetectionRange = 1800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float FieldOfViewDegrees = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float SweepHalfAngle = 55.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float SweepSpeed = 22.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float InitialYaw = 0.f;

	/** Detection accumulates over time; this is the rate per second at full visibility. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float DetectionRate = 55.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") bool bDestroyable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Camera") float Health = 30.f;

	// ---- Electronic device interface ----
	virtual void DisableFor_Implementation(float Duration) override;
	virtual void RestorePower_Implementation() override;
	virtual bool IsDisabled_Implementation() const override { return bDisabled; }
	virtual bool HasPower_Implementation() const override { return bPowered; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Camera") void TakeCameraDamage(float Amount);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Camera") void SetDetectionEnabled(bool bEnabled);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Camera") bool IsDestroyed() const { return bDestroyed; }

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Camera") FBVOnCameraDetected OnCameraDetected;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<USceneComponent> SweepPivot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UStaticMeshComponent> CameraBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<USpotLightComponent> VisionCone;

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Camera") float SweepAlpha = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Camera") float DetectionProgress = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Camera") bool bDisabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Camera") bool bPowered = true;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Camera") bool bDestroyed = false;

	FTimerHandle ReenableTimer;

	void EvaluateDetection(float DeltaSeconds);
	void TriggerAlarm(AActor* Detected);
};
