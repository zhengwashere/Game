// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGadgetThrowables.h"
#include "BVThrowableEffects.h"
#include "Player/BVPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

UBVGadgetThrowable::UBVGadgetThrowable()
{
	bIsThrowable = true;
	MaxCharges = 3;
}

bool UBVGadgetThrowable::UseGadget(ABVPlayerCharacter* User, const FVector& AimLocation)
{
	if (!CanUse() || !User || !ProjectileClass)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World) { return false; }

	// Spawn from roughly the hand position, thrown toward the aim point.
	const FVector SpawnLocation = User->GetActorLocation()
		+ User->GetActorForwardVector() * 60.f + FVector(0.f, 0.f, 80.f);
	const FVector Direction = (AimLocation - SpawnLocation).GetSafeNormal();

	FActorSpawnParameters Params;
	Params.Owner = User;
	Params.Instigator = User;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABVThrowableProjectile* Projectile = World->SpawnActor<ABVThrowableProjectile>(
		ProjectileClass, SpawnLocation, Direction.Rotation(), Params);

	if (!Projectile) { return false; }

	Projectile->Launch(Direction, ThrowSpeed);

	return Super::UseGadget(User, AimLocation);
}

UBVGadgetNoiseMaker::UBVGadgetNoiseMaker()
{
	GadgetID = TEXT("NoiseMaker");
	DisplayName = NSLOCTEXT("BlackVeil", "NoiseMaker", "Noise Maker");
	Description = NSLOCTEXT("BlackVeil", "NoiseMakerDesc", "Thrown device that rattles loudly, luring guards to investigate.");
	IconTag = TEXT("Gadget_NoiseMaker");
	MaxCharges = 4;
	ProjectileClass = ABVProjectile_NoiseMaker::StaticClass();
	SoundEmissionRadius = 1200.f;
}

UBVGadgetFlashbang::UBVGadgetFlashbang()
{
	GadgetID = TEXT("Flashbang");
	DisplayName = NSLOCTEXT("BlackVeil", "Flashbang", "Flashbang");
	Description = NSLOCTEXT("BlackVeil", "FlashbangDesc", "Blinds and deafens guards facing the burst. Loud.");
	IconTag = TEXT("Gadget_Flashbang");
	MaxCharges = 2;
	ProjectileClass = ABVProjectile_Flashbang::StaticClass();
	SoundEmissionRadius = 2000.f;
}

UBVGadgetSmokeGrenade::UBVGadgetSmokeGrenade()
{
	GadgetID = TEXT("SmokeGrenade");
	DisplayName = NSLOCTEXT("BlackVeil", "Smoke", "Smoke Grenade");
	Description = NSLOCTEXT("BlackVeil", "SmokeDesc", "Deploys a concealing cloud that breaks enemy sight lines.");
	IconTag = TEXT("Gadget_Smoke");
	MaxCharges = 2;
	ProjectileClass = ABVProjectile_Smoke::StaticClass();
	SoundEmissionRadius = 900.f;
}

UBVGadgetEMP::UBVGadgetEMP()
{
	GadgetID = TEXT("EMP");
	DisplayName = NSLOCTEXT("BlackVeil", "EMP", "EMP Device");
	Description = NSLOCTEXT("BlackVeil", "EMPDesc", "Disables cameras, terminals and lights in a wide radius for a short time.");
	IconTag = TEXT("Gadget_EMP");
	MaxCharges = 1;
	ProjectileClass = ABVProjectile_EMP::StaticClass();
	SoundEmissionRadius = 800.f;
}

UBVGadgetStickyCamera::UBVGadgetStickyCamera()
{
	GadgetID = TEXT("StickyCamera");
	DisplayName = NSLOCTEXT("BlackVeil", "StickyCamera", "Sticky Camera");
	Description = NSLOCTEXT("BlackVeil", "StickyCameraDesc", "Adheres to any surface and streams a remote feed; can emit noise to distract.");
	IconTag = TEXT("Gadget_StickyCamera");
	MaxCharges = 3;
	ProjectileClass = ABVProjectile_StickyCamera::StaticClass();
	bIsPersistent = true;
	SoundEmissionRadius = 0.f;
}
