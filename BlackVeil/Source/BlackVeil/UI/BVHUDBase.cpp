// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVHUDBase.h"
#include "BVStealthHUDWidget.h"
#include "BVObjectiveTrackerWidget.h"
#include "BVAlertIndicatorWidget.h"
#include "BVEquipmentHUDWidget.h"
#include "BVSubtitleWidget.h"
#include "BVMissionCompleteWidget.h"
#include "Mission/BVMissionManager.h"
#include "Player/BVPlayerCharacter.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "BlackVeil.h"

ABVHUDBase::ABVHUDBase()
{
}

void ABVHUDBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC) { return; }

	auto CreateAndAdd = [&](TSubclassOf<UUserWidget> Class, int32 ZOrder) -> UUserWidget*
	{
		if (!Class) { return nullptr; }
		UUserWidget* Widget = CreateWidget<UUserWidget>(PC, Class);
		if (Widget) { Widget->AddToViewport(ZOrder); }
		return Widget;
	};

	StealthWidget = Cast<UBVStealthHUDWidget>(CreateAndAdd(StealthWidgetClass, 10));
	ObjectiveWidget = Cast<UBVObjectiveTrackerWidget>(CreateAndAdd(ObjectiveWidgetClass, 9));
	AlertWidget = Cast<UBVAlertIndicatorWidget>(CreateAndAdd(AlertWidgetClass, 9));
	EquipmentWidget = Cast<UBVEquipmentHUDWidget>(CreateAndAdd(EquipmentWidgetClass, 8));
	SubtitleWidget = Cast<UBVSubtitleWidget>(CreateAndAdd(SubtitleWidgetClass, 12));

	// Listen for mission completion to raise the results screen.
	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->OnMissionCompleted.AddDynamic(this, &ABVHUDBase::HandleMissionCompleted);
		}
	}
}

void ABVHUDBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->OnMissionCompleted.RemoveDynamic(this, &ABVHUDBase::HandleMissionCompleted);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ABVHUDBase::ShowGameplayHUD(bool bShow)
{
	auto SetVis = [bShow](UUserWidget* W)
	{
		if (!W) { return; }
		W->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	};

	SetVis(StealthWidget);
	SetVis(ObjectiveWidget);
	SetVis(AlertWidget);
	SetVis(EquipmentWidget);
}

void ABVHUDBase::NotifyPlayerDetected()
{
	if (StealthWidget)
	{
		StealthWidget->NotifyDetected();
	}
}

void ABVHUDBase::SetAlertLevel(EBVGlobalAlertLevel Level)
{
	CurrentAlertLevel = Level;
	if (AlertWidget)
	{
		AlertWidget->SetGlobalAlertLevel(Level);
	}
}

void ABVHUDBase::ShowMissionCompleteScreen(const FMissionScore& Score)
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !MissionCompleteWidgetClass) { return; }

	if (!MissionCompleteWidget)
	{
		MissionCompleteWidget = CreateWidget<UBVMissionCompleteWidget>(PC, MissionCompleteWidgetClass);
	}

	if (MissionCompleteWidget)
	{
		MissionCompleteWidget->SetupFromScore(Score);
		MissionCompleteWidget->AddToViewport(50);
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void ABVHUDBase::HandleMissionCompleted(const FMissionScore& Score)
{
	ShowGameplayHUD(false);
	ShowMissionCompleteScreen(Score);
}
