// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVWeapons.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVPlayerController.h"
#include "AI/BVGuardCharacter.h"
#include "World/BVNoiseSubsystem.h"
#include "Core/BlackVeilGameInstance.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "BlackVeil.h"

UBVWeaponBase::UBVWeaponBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBVWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	MagazineAmmo = MagazineSize;
	ReserveAmmo = StartingReserveAmmo;
	OnAmmoChanged.Broadcast(MagazineAmmo, ReserveAmmo);
}

void UBVWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFire();
	Super::EndPlay(EndPlayReason);
}

void UBVWeaponBase::StartFire(ABVPlayerCharacter* Shooter)
{
	CurrentShooter = Shooter;
	FireSingleShot(Shooter);

	if (bIsAutomatic && Shooter)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (ABVPlayerCharacter* S = CurrentShooter.Get())
			{
				FireSingleShot(S);
			}
		}), FireInterval, true);
	}
}

void UBVWeaponBase::StopFire()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
	}
	CurrentShooter = nullptr;
}

bool UBVWeaponBase::Reload()
{
	if (MagazineAmmo >= MagazineSize || ReserveAmmo <= 0)
	{
		return false;
	}

	const int32 Needed = MagazineSize - MagazineAmmo;
	const int32 Taken = FMath::Min(Needed, ReserveAmmo);
	MagazineAmmo += Taken;
	ReserveAmmo -= Taken;

	OnAmmoChanged.Broadcast(MagazineAmmo, ReserveAmmo);
	return true;
}

void UBVWeaponBase::FireSingleShot(ABVPlayerCharacter* Shooter)
{
	UWorld* World = GetWorld();
	if (!World || !Shooter) { return; }

	if (MagazineAmmo <= 0)
	{
		// Dry click; still makes a little noise, but no shot.
		StopFire();
		return;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastFireTime < FireInterval) { return; }
	LastFireTime = Now;

	--MagazineAmmo;
	OnAmmoChanged.Broadcast(MagazineAmmo, ReserveAmmo);

	// Trace from the camera through the aim point so shots go where the player
	// is looking rather than where the body faces.
	const FVector Start = Shooter->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	const FVector AimDir = Shooter->GetControlRotation().Vector();
	const FVector End = Start + AimDir * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVWeaponFire), false, Shooter);
	FHitResult Hit;

	if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		if (ABVGuardCharacter* Guard = Cast<ABVGuardCharacter>(Hit.GetActor()))
		{
			if (bIsLethal)
			{
				Guard->ApplyDamageToGuard(Damage, Shooter);
			}
			else
			{
				// Non-lethal: immediate incapacitation without a kill.
				Guard->ReceiveTakedown(EBVTakedownType::Behind, EBVLethality::NonLethal);
			}

			if (UBlackVeilGameInstance* GI = World->GetGameInstance<UBlackVeilGameInstance>())
			{
				if (bIsLethal) { GI->RecordTakedown(true); }
			}
		}
		else if (ABVPlayerCharacter* HitPlayer = Cast<ABVPlayerCharacter>(Hit.GetActor()))
		{
			// Friendly fire is possible but rare; guards do not shoot each other by
			// accident because their traces exclude other guards.
		}
	}

	// Report the shot. Suppressed shots travel far less, and only a gunshot is
	// tagged as such so AI reacts with the appropriate urgency.
	if (UBVNoiseSubsystem* Noise = World->GetSubsystem<UBVNoiseSubsystem>())
	{
		const float Radius = bIsSuppressed ? SuppressedNoiseRadius : LoudNoiseRadius;
		const EBVNoiseLevel Level = bIsSuppressed ? EBVNoiseLevel::Medium : EBVNoiseLevel::Deafening;
		Noise->ReportNoise(Start, Level, Shooter, true, TEXT("Gunshot"));
	}
}

UBVWeaponSuppressedPistol::UBVWeaponSuppressedPistol()
{
	WeaponID = TEXT("SuppressedPistol");
	DisplayName = NSLOCTEXT("BlackVeil", "SuppPistol", "SC-9 Suppressed Pistol");
	MagazineSize = 15;
	StartingReserveAmmo = 60;
	FireInterval = 0.25f;
	Damage = 34.f;
	Range = 5000.f;
	bIsLethal = true;
	bIsSuppressed = true;
	bIsAutomatic = false;
	SuppressedNoiseRadius = 700.f;
}

UBVWeaponSuppressedRifle::UBVWeaponSuppressedRifle()
{
	WeaponID = TEXT("SuppressedRifle");
	DisplayName = NSLOCTEXT("BlackVeil", "SuppRifle", "MK-4 Suppressed Rifle");
	MagazineSize = 30;
	StartingReserveAmmo = 120;
	FireInterval = 0.11f;
	Damage = 26.f;
	Range = 8000.f;
	bIsLethal = true;
	bIsSuppressed = true;
	bIsAutomatic = true;
	SuppressedNoiseRadius = 1100.f;
}

UBVWeaponNonLethal::UBVWeaponNonLethal()
{
	WeaponID = TEXT("NonLethalProjector");
	DisplayName = NSLOCTEXT("BlackVeil", "NonLethal", "TR-2 Non-Lethal Projector");
	MagazineSize = 6;
	StartingReserveAmmo = 24;
	FireInterval = 0.9f;
	Damage = 0.f;
	Range = 3500.f;
	bIsLethal = false;
	bIsSuppressed = true;
	bIsAutomatic = false;
	SuppressedNoiseRadius = 400.f;
}
