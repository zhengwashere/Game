// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVStealthHUDWidget.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVStealthComponent.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVStealthHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

ABVPlayerCharacter* UBVStealthHUDWidget::GetPlayer() const
{
	UWorld* World = GetWorld();
	return World && World->GetFirstPlayerController()
		? Cast<ABVPlayerCharacter>(World->GetFirstPlayerController()->GetPawn())
		: nullptr;
}

void UBVStealthHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ABVPlayerCharacter* Player = GetPlayer();
	UBVStealthComponent* Stealth = Player ? Player->GetStealthComponent() : nullptr;

	if (Stealth)
	{
		const float NewPercent = Stealth->GetVisibilityLevel() * 100.f;
		const bool bNewShadow = Stealth->IsInShadow();

		if (!FMath::IsNearlyEqual(NewPercent, VisibilityPercent, 0.5f) || bNewShadow != bInShadow)
		{
			VisibilityPercent = NewPercent;
			bInShadow = bNewShadow;
			OnVisibilityUpdated(VisibilityPercent, bInShadow);
		}

		const float NewNoise = Stealth->GetNoiseRadius();
		const EBVNoiseLevel NewLevel = Stealth->GetCurrentNoiseLevel();
		if (!FMath::IsNearlyEqual(NewNoise, NoiseRadius, 10.f) || NewLevel != NoiseLevel)
		{
			NoiseRadius = NewNoise;
			NoiseLevel = NewLevel;
			OnNoiseUpdated(NoiseRadius, NoiseLevel);
		}
	}

	// Detection flash decays over about a second.
	if (DetectionFlashTimer > 0.f)
	{
		DetectionFlashTimer -= InDeltaTime;
		DetectionFlashAlpha = FMath::Clamp(DetectionFlashTimer, 0.f, 1.f);
		OnDetectionFlash(DetectionFlashAlpha);
		if (DetectionFlashTimer <= 0.f)
		{
			bDetected = false;
		}
	}
}

void UBVStealthHUDWidget::NotifyDetected()
{
	bDetected = true;
	DetectionFlashTimer = 1.f;
	DetectionFlashAlpha = 1.f;
	OnDetectionFlash(DetectionFlashAlpha);
}
