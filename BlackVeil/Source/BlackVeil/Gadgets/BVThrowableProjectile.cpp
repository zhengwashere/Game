// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVThrowableProjectile.h"
#include "World/BVNoiseSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVThrowableProjectile::ABVThrowableProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComponent->SetSimulatePhysics(false);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1400.f;
	ProjectileMovement->MaxSpeed = 1800.f;
	ProjectileMovement->ProjectileGravityScale = 1.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->Friction = 0.4f;
}

void ABVThrowableProjectile::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentHit.AddDynamic(this, &ABVThrowableProjectile::HandleHit);

	if (FuseTime > 0.f)
	{
		GetWorldTimerManager().SetTimer(FuseTimer, this, &ABVThrowableProjectile::Detonate, FuseTime, false);
	}
}

void ABVThrowableProjectile::Launch(const FVector& Direction, float Speed)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction.GetSafeNormal() * Speed;
	}
}

void ABVThrowableProjectile::HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (bDetonated) { return; }

	// Impact-fused devices (flashbang, smoke) pop on a solid hit; others just bounce.
	if (ImpactSpeedThreshold > 0.f && NormalImpulse.Size() > ImpactSpeedThreshold)
	{
		Detonate();
	}
}

void ABVThrowableProjectile::Detonate()
{
	if (bDetonated) { return; }
	bDetonated = true;

	// Every device makes *some* noise; the subclass may override the radius.
	if (UWorld* World = GetWorld())
	{
		if (UBVNoiseSubsystem* Noise = World->GetSubsystem<UBVNoiseSubsystem>())
		{
			Noise->ReportNoise(GetActorLocation(), EBVNoiseLevel::Heavy, this, false, TEXT("GadgetDetonate"));
		}
	}

	Destroy();
}
