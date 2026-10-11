// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BVTypes.h"
#include "BVThrowableProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

/**
 * Base class for thrown devices. Travels under projectile movement, then fires
 * its effect either on impact or after a fuse.
 */
UCLASS(Abstract)
class BLACKVEIL_API ABVThrowableProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABVThrowableProjectile();

	virtual void BeginPlay() override;

	/** Thrown by a character; sets the initial velocity. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Throwable")
	void Launch(const FVector& Direction, float Speed);

	/** Applied by the subclass: the actual gameplay effect. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Throwable")
	virtual void Detonate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Throwable") float FuseTime = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Throwable") float ImpactSpeedThreshold = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Throwable") float EffectRadius = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Throwable") float NoiseRadius = 1200.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UFUNCTION() void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	bool bDetonated = false;
	FTimerHandle FuseTimer;
};
