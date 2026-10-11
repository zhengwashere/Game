// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVDestructibleLight.h"
#include "World/BVNoiseSubsystem.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVDestructibleLight::ABVDestructibleLight()
{
	PrimaryActorTick.bCanEverTick = true;

	FixtureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FixtureMesh"));
	SetRootComponent(FixtureMesh);

	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComponent"));
	LightComponent->SetupAttachment(FixtureMesh);
	Cast<UPointLightComponent>(LightComponent)->SetIntensity(BaseIntensity);
	Cast<UPointLightComponent>(LightComponent)->SetAttenuationRadius(1600.f);

	bLightOn = bStartsOn;
}

void ABVDestructibleLight::BeginPlay()
{
	Super::BeginPlay();
	bLightOn = bStartsOn;
	ApplyLightState();
}

void ABVDestructibleLight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bFlickers || !bLightOn || bBroken) { return; }

	// Flicker by modulating intensity around the base value.
	FlickerAccumulator += DeltaSeconds * FlickerSpeed;
	const float Noise = 0.72f + 0.28f * FMath::Sin(FlickerAccumulator * 3.1f)
		* FMath::Sin(FlickerAccumulator * 7.7f);

	if (UPointLightComponent* Point = Cast<UPointLightComponent>(LightComponent))
	{
		Point->SetIntensity(BaseIntensity * Noise);
	}
}

void ABVDestructibleLight::ApplyLightState()
{
	if (LightComponent)
	{
		LightComponent->SetVisibility(bLightOn && !bBroken);
		LightComponent->SetIntensity(bLightOn ? BaseIntensity : 0.f);
	}
}

void ABVDestructibleLight::SetLightOn(bool bOn)
{
	if (bBroken && bOn) { return; }
	bLightOn = bOn;
	ApplyLightState();
}

void ABVDestructibleLight::ToggleLight()
{
	SetLightOn(!bLightOn);
}

void ABVDestructibleLight::TakeLightDamage(float Amount)
{
	if (!bDestructible || bBroken) { return; }

	Health -= Amount;
	if (Health <= 0.f)
	{
		bBroken = true;
		bLightOn = false;
		ApplyLightState();

		// Shattering glass is loud - shooting a light is not automatically safe.
		if (UBVNoiseSubsystem* Noise = GetWorld() ? GetWorld()->GetSubsystem<UBVNoiseSubsystem>() : nullptr)
		{
			Noise->ReportNoise(GetActorLocation(), EBVNoiseLevel::Medium, this, false, TEXT("GlassBreak"));
		}

		UE_LOG(LogBlackVeil, Log, TEXT("Light %s destroyed."), *GetName());
	}
}

void ABVDestructibleLight::DisableFor_Implementation(float Duration)
{
	bPowered = false;
	SetLightOn(false);

	if (Duration > 0.f)
	{
		FTimerHandle T;
		GetWorldTimerManager().SetTimer(T, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			bPowered = true;
			if (!bBroken) { SetLightOn(true); }
		}), Duration, false);
	}
}

void ABVDestructibleLight::RestorePower_Implementation()
{
	bPowered = true;
	if (!bBroken) { SetLightOn(true); }
}
