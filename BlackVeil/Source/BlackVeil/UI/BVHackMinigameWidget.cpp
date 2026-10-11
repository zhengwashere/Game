// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVHackMinigameWidget.h"

void UBVHackMinigameWidget::BeginHack(int32 SecurityTier, FName TargetID)
{
	bActive = true;
	CurrentTier = FMath::Clamp(SecurityTier, 1, 5);
	CurrentTargetID = TargetID;

	// Higher tiers shrink the sweet spot and speed the cursor up.
	SweetSpotHalfWidth = FMath::Lerp(0.18f, 0.05f, (CurrentTier - 1) / 4.f);
	CursorSpeed = FMath::Lerp(0.9f, 2.4f, (CurrentTier - 1) / 4.f);
	SweetSpotCenter = FMath::FRandRange(0.25f, 0.75f);
	CursorPosition = 0.f;
	CursorDirection = 1.f;

	OnHackStarted(CurrentTier);
}

void UBVHackMinigameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bActive) { return; }

	CursorPosition += CursorDirection * CursorSpeed * InDeltaTime;
	if (CursorPosition >= 1.f) { CursorPosition = 1.f; CursorDirection = -1.f; }
	else if (CursorPosition <= 0.f) { CursorPosition = 0.f; CursorDirection = 1.f; }
}

void UBVHackMinigameWidget::SubmitHack()
{
	if (!bActive) { return; }

	const bool bSuccess = FMath::Abs(CursorPosition - SweetSpotCenter) <= SweetSpotHalfWidth;
	bActive = false;

	OnHackFinished(bSuccess);
	OnHackResolved.Broadcast(bSuccess);
}

void UBVHackMinigameWidget::CancelHack()
{
	bActive = false;
	OnHackFinished(false);
	OnHackResolved.Broadcast(false);
}
