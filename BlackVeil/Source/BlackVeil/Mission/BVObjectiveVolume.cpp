// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVObjectiveVolume.h"
#include "BVMissionManager.h"
#include "Player/BVPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVObjectiveVolume::ABVObjectiveVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetBoxExtent(FVector(200.f, 200.f, 150.f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetGenerateOverlapEvents(true);

	EditorIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIcon"));
	EditorIcon->SetupAttachment(TriggerVolume);
}

void ABVObjectiveVolume::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABVObjectiveVolume::HandleBeginOverlap);

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->RegisterObjectiveVolume(this);
		}
	}
}

void ABVObjectiveVolume::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Other || !Other->IsA<ABVPlayerCharacter>()) { return; }
	if (bRequiresInteraction) { return; }
	TriggerObjective(Other);
}

void ABVObjectiveVolume::TriggerObjective(AActor* Triggerer)
{
	if (bSingleUse && bHasFired) { return; }
	bHasFired = true;

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->AddObjectiveProgress(ObjectiveID, ProgressAmount);
		}
	}
}
