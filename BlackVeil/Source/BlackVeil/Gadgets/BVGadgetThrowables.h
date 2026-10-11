// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BVGadgetBase.h"
#include "BVGadgetThrowables.generated.h"

class ABVThrowableProjectile;

/**
 * Base for aimed, thrown gadgets. Spawns the configured projectile from the
 * player's hand toward the aim point.
 */
UCLASS(Abstract)
class BLACKVEIL_API UBVGadgetThrowable : public UBVGadgetBase
{
	GENERATED_BODY()

public:
	UBVGadgetThrowable();

	virtual bool UseGadget(ABVPlayerCharacter* User, const FVector& AimLocation) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Gadget") TSubclassOf<ABVThrowableProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Gadget") float ThrowSpeed = 1500.f;
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetNoiseMaker : public UBVGadgetThrowable
{
	GENERATED_BODY()
public:
	UBVGadgetNoiseMaker();
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetFlashbang : public UBVGadgetThrowable
{
	GENERATED_BODY()
public:
	UBVGadgetFlashbang();
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetSmokeGrenade : public UBVGadgetThrowable
{
	GENERATED_BODY()
public:
	UBVGadgetSmokeGrenade();
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetEMP : public UBVGadgetThrowable
{
	GENERATED_BODY()
public:
	UBVGadgetEMP();
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVGadgetStickyCamera : public UBVGadgetThrowable
{
	GENERATED_BODY()
public:
	UBVGadgetStickyCamera();
};
