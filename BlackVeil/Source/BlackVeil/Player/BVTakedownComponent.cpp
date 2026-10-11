// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVTakedownComponent.h"
#include "BVPlayerCharacter.h"
#include "AI/BVGuardCharacter.h"
#include "World/BVHidingSpot.h"
#include "Core/BVGameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BlackVeil.h"

UBVTakedownComponent::UBVTakedownComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void UBVTakedownComponent::EvaluateTakedownTargets()
{
	ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner());
	UWorld* World = GetWorld();
	BestTarget = nullptr;
	if (!Player || !World) { return; }

	const FVector PlayerLoc = Player->GetActorLocation();
	float BestScore = -FLT_MAX;

	for (TActorIterator<ABVGuardCharacter> It(World); It; ++It)
	{
		ABVGuardCharacter* Guard = *It;
		if (!Guard || Guard->IsIncapacitated()) { continue; }

		const float Dist = FVector::Dist(PlayerLoc, Guard->GetActorLocation());
		if (Dist > TakedownReach) { continue; }

		// Guard must not be aware of the player for a silent takedown.
		if (Guard->IsAlertedToPlayer()) { continue; }

		const FVector GuardForward = Guard->GetActorForwardVector();
		const FVector ToPlayer = (PlayerLoc - Guard->GetActorLocation()).GetSafeNormal();
		const float AngleFromGuardFacing = FMath::RadiansToDegrees(
			FMath::Acos(FMath::Clamp(FVector::DotProduct(GuardForward, ToPlayer), -1.f, 1.f)));

		EBVTakedownType Type;
		if (Player->GetActorLocation().Z > Guard->GetActorLocation().Z + 120.f)
		{
			Type = EBVTakedownType::Above;
		}
		else if (AngleFromGuardFacing > BehindAngleDegrees)
		{
			Type = EBVTakedownType::Behind;
		}
		else if (AngleFromGuardFacing > 60.f)
		{
			Type = EBVTakedownType::Corner;
		}
		else
		{
			// Guard is facing the player: no silent takedown available.
			continue;
		}

		const float Score = (180.f - AngleFromGuardFacing) - Dist * 0.2f;
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Guard;
			PendingType = Type;
		}
	}
}

void UBVTakedownComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceEval += DeltaTime;
	if (TimeSinceEval >= EvalInterval)
	{
		TimeSinceEval = 0.f;
		EvaluateTakedownTargets();
	}
}

bool UBVTakedownComponent::TryTakedown(EBVLethality Lethality)
{
	ABVGuardCharacter* Target = BestTarget.Get();
	if (!Target)
	{
		return false;
	}

	Target->ReceiveTakedown(PendingType, Lethality);

	if (UBlackVeilGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UBlackVeilGameInstance>() : nullptr)
	{
		GI->RecordTakedown(Lethality == EBVLethality::Lethal);
	}

	OnTakedownPerformed.Broadcast(Target, Lethality);
	BestTarget = nullptr;
	return true;
}

bool UBVTakedownComponent::PickUpNearestBody()
{
	if (CarriedGuard.IsValid()) { return false; }

	ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner());
	UWorld* World = GetWorld();
	if (!Player || !World) { return false; }

	const FVector PlayerLoc = Player->GetActorLocation();
	ABVGuardCharacter* Best = nullptr;
	float BestDist = BodyPickupReach;

	for (TActorIterator<ABVGuardCharacter> It(World); It; ++It)
	{
		ABVGuardCharacter* Guard = *It;
		if (!Guard || !Guard->IsIncapacitated()) { continue; }
		const float Dist = FVector::Dist(PlayerLoc, Guard->GetActorLocation());
		if (Dist < BestDist)
		{
			BestDist = Dist;
			Best = Guard;
		}
	}

	if (!Best) { return false; }

	CarriedGuard = Best;
	Best->SetBeingCarried(true);
	// Carrying slows the operative and prevents sprinting.
	Player->SetSilentMove(true);
	OnBodyCarryChanged.Broadcast(true);
	return true;
}

void UBVTakedownComponent::DropCarriedBody()
{
	ABVGuardCharacter* Guard = CarriedGuard.Get();
	if (!Guard) { return; }

	Guard->SetBeingCarried(false);
	CarriedGuard = nullptr;

	if (ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(GetOwner()))
	{
		Player->SetSilentMove(false);
	}
	OnBodyCarryChanged.Broadcast(false);
}

bool UBVTakedownComponent::HideCarriedBodyIn(ABVHidingSpot* Spot)
{
	ABVGuardCharacter* Guard = CarriedGuard.Get();
	if (!Guard || !Spot || !Spot->CanAcceptBody()) { return false; }

	Spot->ConcealBody(Guard);
	CarriedGuard = nullptr;
	OnBodyCarryChanged.Broadcast(false);
	return true;
}
