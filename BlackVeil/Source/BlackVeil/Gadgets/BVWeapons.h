// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BVWeapons.generated.h"

class ABVPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnAmmoChanged, int32, MagazineAmmo, int32, ReserveAmmo);

/**
 * Base firearm. Suppressed weapons emit a much smaller noise radius, which is
 * the entire tactical reason to carry them.
 */
UCLASS(Abstract, ClassGroup = (BlackVeil), meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVWeaponBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UBVWeaponBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") FName WeaponID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") int32 MagazineSize = 15;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") int32 StartingReserveAmmo = 60;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") float FireInterval = 0.22f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") float Damage = 34.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") float Range = 6000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") bool bIsLethal = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") bool bIsSuppressed = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") bool bIsAutomatic = false;

	/** Noise radius when fired suppressed vs unsuppressed, in cm. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") float SuppressedNoiseRadius = 900.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlackVeil|Weapon") float LoudNoiseRadius = 4000.f;

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Weapon") int32 GetMagazineAmmo() const { return MagazineAmmo; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Weapon") int32 GetReserveAmmo() const { return ReserveAmmo; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Weapon") bool IsSuppressed() const { return bIsSuppressed; }
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Weapon") bool IsLethal() const { return bIsLethal; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Weapon") void StartFire(ABVPlayerCharacter* Shooter);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Weapon") void StopFire();
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Weapon") bool Reload();

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Weapon") FBVOnAmmoChanged OnAmmoChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Performs one shot: trace, damage, noise, feedback. */
	void FireSingleShot(ABVPlayerCharacter* Shooter);

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Weapon") int32 MagazineAmmo = 0;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|Weapon") int32 ReserveAmmo = 0;

	float LastFireTime = -100.f;
	FTimerHandle AutoFireTimer;
	TWeakObjectPtr<ABVPlayerCharacter> CurrentShooter;
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVWeaponSuppressedPistol : public UBVWeaponBase
{
	GENERATED_BODY()
public:
	UBVWeaponSuppressedPistol();
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVWeaponSuppressedRifle : public UBVWeaponBase
{
	GENERATED_BODY()
public:
	UBVWeaponSuppressedRifle();
};

UCLASS(meta = (BlueprintSpawnableComponent))
class BLACKVEIL_API UBVWeaponNonLethal : public UBVWeaponBase
{
	GENERATED_BODY()
public:
	UBVWeaponNonLethal();
};
