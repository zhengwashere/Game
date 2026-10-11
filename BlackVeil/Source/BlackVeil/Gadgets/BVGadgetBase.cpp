// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGadgetBase.h"
#include "Player/BVPlayerCharacter.h"
#include "World/BVNoiseSubsystem.h"
#include "Engine/World.h"
#include "BlackVeil.h"

UBVGadgetBase::UBVGadgetBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBVGadgetBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentCharges = MaxCharges;
}

void UBVGadgetBase::SetCharges(int32 NewCharges)
{
	CurrentCharges = FMath::Clamp(NewCharges, 0, MaxCharges);
	OnChargesChanged.Broadcast(GadgetID, CurrentCharges);
}

void UBVGadgetBase::AddCharges(int32 Delta)
{
	SetCharges(CurrentCharges + Delta);
}

bool UBVGadgetBase::UseGadget(ABVPlayerCharacter* User, const FVector& AimLocation)
{
	if (!CanUse())
	{
		return false;
	}

	SetCharges(CurrentCharges - 1);
	EmitGadgetNoise(User);
	OnGadgetUsed.Broadcast(GadgetID);
	return true;
}

void UBVGadgetBase::EmitGadgetNoise(ABVPlayerCharacter* User)
{
	if (SoundEmissionRadius <= 0.f || !User)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UBVNoiseSubsystem* Noise = World->GetSubsystem<UBVNoiseSubsystem>())
		{
			Noise->ReportNoise(User->GetActorLocation(), EBVNoiseLevel::Light, User, false, GadgetID);
		}
	}
}
