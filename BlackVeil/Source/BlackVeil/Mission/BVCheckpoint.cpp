// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVCheckpoint.h"
#include "BVMissionManager.h"
#include "Player/BVPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVCheckpoint::ABVCheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetBoxExtent(FVector(250.f, 250.f, 150.f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetGenerateOverlapEvents(true);
}

void ABVCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABVCheckpoint::HandleBeginOverlap);

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->RegisterCheckpoint(this);
		}
	}
}

void ABVCheckpoint::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(Other);
	if (!Player) { return; }
	if (bHasBeenReached && !bRepeatable) { return; }

	bHasBeenReached = true;
	SavedTransform = Player->GetActorTransform();

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			SavedMissionTime = MM->GetElapsedTime();
			MM->TriggerCheckpoint(CheckpointID, this);
		}
	}
}

void ABVCheckpoint::RestoreCheckpoint()
{
	if (!bHasBeenReached) { return; }

	UWorld* World = GetWorld();
	if (!World) { return; }

	// Re-place the player at the recorded spot.
	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(PC->GetPawn()))
		{
			Player->SetActorTransform(SavedTransform, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
}
