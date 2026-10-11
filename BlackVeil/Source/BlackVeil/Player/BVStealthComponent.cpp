// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVStealthComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/Level.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "GameFramework/Character.h"
#include "CollisionQueryParams.h"
#include "World/BVNoiseSubsystem.h"
#include "BlackVeil.h"

UBVStealthComponent::UBVStealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void UBVStealthComponent::BeginPlay()
{
	Super::BeginPlay();
	RefreshLightCache();
}

void UBVStealthComponent::SetPlayerPosture(EBVPosture NewPosture) { Posture = NewPosture; }
void UBVStealthComponent::SetMovementSpeed(EBVMovementSpeed NewSpeed) { Speed = NewSpeed; }
void UBVStealthComponent::SetLeaning(bool bNewLeaning) { bLeaning = bNewLeaning; }

void UBVStealthComponent::RefreshLightCache()
{
	CachedLights.Reset();
	UWorld* World = GetWorld();
	if (!World) { return; }

	const FVector Origin = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;

	// Collect movable/static lights near the player. We iterate actors rather
	// than using a query channel because lights have no collision.
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor == GetOwner()) { continue; }

		ULightComponent* LightComp = Actor->FindComponentByClass<ULightComponent>();
		if (!LightComp || !LightComp->IsVisible()) { continue; }

		const float DistSq = FVector::DistSquared(Origin, Actor->GetActorLocation());
		if (DistSq <= FMath::Square(LightSampleRadius))
		{
			CachedLights.Add(Actor);
		}
	}
	LightCacheAge = 0.f;
}

float UBVStealthComponent::PostureMultiplier() const
{
	switch (Posture)
	{
	case EBVPosture::Prone:     return 0.55f;
	case EBVPosture::Crouching: return 0.72f;
	case EBVPosture::Hanging:   return 0.62f;
	case EBVPosture::InCover:   return 0.65f;
	case EBVPosture::Standing:
	default:                    return 1.0f;
	}
}

float UBVStealthComponent::MovementMultiplier() const
{
	switch (Speed)
	{
	case EBVMovementSpeed::Prone:  return 0.85f;
	case EBVMovementSpeed::Crouch: return 0.9f;
	case EBVMovementSpeed::Silent: return 0.95f;
	case EBVMovementSpeed::Walk:   return 1.0f;
	case EBVMovementSpeed::Jog:    return 1.12f;
	case EBVMovementSpeed::Sprint: return 1.3f;
	default:                       return 1.0f;
	}
}

void UBVStealthComponent::SampleLighting()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World) { return; }

	const FVector SampleLocation = Owner->GetActorLocation() + FVector(0.f, 0.f, 40.f);
	float Exposure = AmbientBaseline;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BVLightSample), false, Owner);

	for (const TWeakObjectPtr<AActor>& Weak : CachedLights)
	{
		AActor* LightActor = Weak.Get();
		if (!LightActor) { continue; }

		ULightComponent* LightComp = LightActor->FindComponentByClass<ULightComponent>();
		if (!LightComp || !LightComp->IsVisible() || LightComp->Intensity <= 0.f) { continue; }

		const FVector LightLocation = LightActor->GetActorLocation();
		const float Dist = FVector::Dist(SampleLocation, LightLocation);
		if (Dist > LightSampleRadius) { continue; }

		// Normalised distance falloff.
		const float DistFalloff = FMath::Clamp(1.f - (Dist / LightSampleRadius), 0.f, 1.f);

		// Shadowing: any blocking geometry between light and player removes most
		// of this light's contribution.
		FHitResult Hit;
		const bool bBlocked = World->LineTraceSingleByChannel(
			Hit, LightLocation, SampleLocation, ECC_Visibility, Params);
		const float ShadowFactor = bBlocked ? 0.08f : 1.f;

		// Spot lights only illuminate inside their cone.
		float ConeFactor = 1.f;
		if (const USpotLightComponent* Spot = Cast<USpotLightComponent>(LightComp))
		{
			const FVector ToPlayer = (SampleLocation - LightLocation).GetSafeNormal();
			const float CosAngle = FVector::DotProduct(LightActor->GetActorForwardVector(), ToPlayer);
			const float CosOuter = FMath::Cos(FMath::DegreesToRadians(Spot->OuterConeAngle));
			ConeFactor = FMath::Clamp((CosAngle - CosOuter) / FMath::Max(1.f - CosOuter, KINDA_SMALL_NUMBER), 0.f, 1.f);
		}

		const float IntensityScale = FMath::Clamp(LightComp->Intensity / 3000.f, 0.f, 1.5f);
		Exposure += IntensityScale * DistFalloff * ShadowFactor * ConeFactor * 0.5f;
	}

	LightExposure = FMath::Clamp(Exposure, 0.f, 1.f);
}

void UBVStealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LightCacheAge += DeltaTime;
	if (LightCacheAge > 1.0f)
	{
		RefreshLightCache();
	}

	TimeSinceSample += DeltaTime;
	if (TimeSinceSample < SampleInterval) { return; }
	TimeSinceSample = 0.f;

	SampleLighting();

	const float Prev = VisibilityLevel;

	// Visibility = light exposure scaled by how large/easy a target the player is.
	float Target = LightExposure * PostureMultiplier() * MovementMultiplier();
	if (bLeaning) { Target *= 1.15f; }

	// Deep shadow clamps visibility hard so darkness is meaningful.
	if (IsInShadow()) { Target *= 0.45f; }

	VisibilityLevel = FMath::Clamp(Target, 0.f, 1.f);

	// Noise from the current speed tier and surface.
	const EBVSurfaceType Surface = EBVSurfaceType::Concrete;
	CurrentNoise = UBVNoiseSubsystem::GetNoiseLevelForSpeed(Speed, Surface);

	if (!FMath::IsNearlyEqual(Prev, VisibilityLevel, 0.01f))
	{
		OnVisibilityChanged.Broadcast(VisibilityLevel, IsInShadow());
	}
}

float UBVStealthComponent::GetNoiseRadius() const
{
	return UBVNoiseSubsystem::GetBaseRadiusForLevel(CurrentNoise);
}

float UBVStealthComponent::GetDetectionMultiplier() const
{
	// 0.25 in full shadow, up to ~1.3 when fully lit and sprinting.
	return FMath::Lerp(0.25f, 1.35f, VisibilityLevel);
}
