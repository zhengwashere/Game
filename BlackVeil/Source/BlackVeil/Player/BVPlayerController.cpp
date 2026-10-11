// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVPlayerController.h"
#include "BVPlayerCharacter.h"
#include "UI/BVHUDBase.h"
#include "UI/BVPauseMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BlackVeil.h"

ABVPlayerController::ABVPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void ABVPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	OnMissionIntro();
}

void ABVPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	BVCharacter = Cast<ABVPlayerCharacter>(InPawn);
	if (ABVPlayerCharacter* Character = BVCharacter.Get())
	{
		Character->OnSpeedChanged.AddDynamic(this, &ABVPlayerController::HandleSpeedChanged);
	}
}

void ABVPlayerController::OnUnPossess()
{
	if (ABVPlayerCharacter* Character = BVCharacter.Get())
	{
		Character->OnSpeedChanged.RemoveDynamic(this, &ABVPlayerController::HandleSpeedChanged);
	}
	BVCharacter = nullptr;
	Super::OnUnPossess();
}

void ABVPlayerController::HandleSpeedChanged(EBVMovementSpeed NewSpeed, EBVNoiseLevel Noise)
{
	// Forwarded to the HUD's noise indicator; the HUD pulls stealth data each tick,
	// this hook lets audio respond immediately to a tier change.
}

ABVHUDBase* ABVPlayerController::GetBlackVeilHUD() const
{
	return Cast<ABVHUDBase>(GetHUD());
}

void ABVPlayerController::TogglePauseMenu()
{
	ShowPauseMenu(!bPauseMenuVisible);
}

void ABVPlayerController::ShowPauseMenu(bool bShow)
{
	bPauseMenuVisible = bShow;

	if (bShow)
	{
		if (!PauseMenuWidget && PauseMenuClass)
		{
			PauseMenuWidget = CreateWidget<UBVPauseMenuWidget>(this, PauseMenuClass);
		}
		if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
		{
			PauseMenuWidget->AddToViewport(100);
		}
		SetPause(true);
		SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
		bShowMouseCursor = true;
	}
	else
	{
		if (PauseMenuWidget)
		{
			PauseMenuWidget->RemoveFromParent();
		}
		SetPause(false);
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}
