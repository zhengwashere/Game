// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVAlertIndicatorWidget.h"
#include "AI/BVGuardCharacter.h"
#include "Player/BVPlayerCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BlackVeil.h"

void UBVAlertIndicatorWidget::SetGlobalAlertLevel(EBVGlobalAlertLevel Level)
{
	if (Level == AlertLevel) { return; }
	AlertLevel = Level;
	OnAlertLevelChanged(AlertLevel);
}

FText UBVAlertIndicatorWidget::GetAlertText() const
{
	switch (AlertLevel)
	{
	case EBVGlobalAlertLevel::Normal:     return NSLOCTEXT("BlackVeil", "AlertNormal", "CLEAR");
	case EBVGlobalAlertLevel::Suspicious: return NSLOCTEXT("BlackVeil", "AlertSusp", "SUSPICIOUS");
	case EBVGlobalAlertLevel::Searching:  return NSLOCTEXT("BlackVeil", "AlertSearch", "SEARCHING");
	case EBVGlobalAlertLevel::Alarmed:    return NSLOCTEXT("BlackVeil", "AlertAlarm", "ALARMED");
	case EBVGlobalAlertLevel::Combat:     return NSLOCTEXT("BlackVeil", "AlertCombat", "COMBAT");
	default:                              return FText::GetEmpty();
	}
}

void UBVAlertIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* World = GetWorld();
	if (!World) { return; }

	ABVPlayerCharacter* Player = World->GetFirstPlayerController()
		? Cast<ABVPlayerCharacter>(World->GetFirstPlayerController()->GetPawn()) : nullptr;
	if (!Player) { return; }

	// Count guards that are actively hunting, and work out the bearing to the
	// closest one so the HUD can show a directional threat pip.
	int32 Count = 0;
	float ClosestDist = FLT_MAX;
	FVector ClosestLoc = FVector::ZeroVector;

	for (TActorIterator<ABVGuardCharacter> It(World); It; ++It)
	{
		ABVGuardCharacter* Guard = *It;
		if (!Guard || Guard->IsIncapacitated()) { continue; }
		if (!Guard->IsAlertedToPlayer()) { continue; }

		++Count;
		const float Dist = FVector::Dist(Player->GetActorLocation(), Guard->GetActorLocation());
		if (Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestLoc = Guard->GetActorLocation();
		}
	}

	if (Count != NearbyGuardCount)
	{
		NearbyGuardCount = Count;
	}

	if (Count > 0)
	{
		const FVector ToGuard = (ClosestLoc - Player->GetActorLocation()).GetSafeNormal();
		const FVector Forward = Player->GetControlRotation().Vector();
		const FVector ForwardFlat = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
		const FVector ToGuardFlat = FVector(ToGuard.X, ToGuard.Y, 0.f).GetSafeNormal();

		const float Dot = FVector::DotProduct(ForwardFlat, ToGuardFlat);
		const float Cross = ForwardFlat.X * ToGuardFlat.Y - ForwardFlat.Y * ToGuardFlat.X;
		NearestThreatBearing = FMath::RadiansToDegrees(FMath::Atan2(Cross, Dot));
	}

	OnThreatsUpdated(NearbyGuardCount, NearestThreatBearing);
}
