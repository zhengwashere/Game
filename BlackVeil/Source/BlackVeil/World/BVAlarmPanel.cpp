// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVAlarmPanel.h"
#include "AI/BVSquadManager.h"
#include "Mission/BVMissionManager.h"
#include "Core/BlackVeilGameInstance.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BlackVeil.h"

ABVAlarmPanel::ABVAlarmPanel()
{
	InteractionType = EBVInteractionType::AlarmPanel;
	PromptText = NSLOCTEXT("BlackVeil", "AlarmPanel", "Alarm Panel");
	IconTag = TEXT("Alarm");
	HoldDuration = 1.5f;
	AvailableActions = { TEXT("Alarm.Sabotage") };

	StatusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StatusLight"));
	StatusLight->SetupAttachment(MeshComponent);
	StatusLight->SetIntensity(250.f);
	StatusLight->SetAttenuationRadius(250.f);
	StatusLight->SetLightColor(FLinearColor(0.1f, 1.f, 0.2f));
}

TArray<FName> ABVAlarmPanel::GetInteractionActions_Implementation(ABVPlayerCharacter* Interactor) const
{
	TArray<FName> Actions;
	if (!bSabotaged)
	{
		Actions.Add(TEXT("Alarm.Sabotage"));
	}
	return Actions;
}

bool ABVAlarmPanel::ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor)
{
	if (ActionID == TEXT("Alarm.Sabotage"))
	{
		SabotagePanel();
		return Super::ExecuteInteraction_Implementation(ActionID, Interactor);
	}
	return false;
}

void ABVAlarmPanel::SabotagePanel()
{
	bSabotaged = true;
	bAlarmActive = false;

	if (StatusLight)
	{
		StatusLight->SetLightColor(FLinearColor(0.15f, 0.15f, 0.15f));
		StatusLight->SetIntensity(60.f);
	}

	// Clear any active alarm for this group and tell the squad the panel is dead.
	if (UWorld* World = GetWorld())
	{
		if (UBVSquadManager* Squad = World->GetSubsystem<UBVSquadManager>())
		{
			Squad->SetSquadAlarm(AlarmGroupID, false, GetActorLocation());
		}
	}

	UE_LOG(LogBlackVeil, Log, TEXT("Alarm panel %s sabotaged."), *GetName());
}

void ABVAlarmPanel::TriggerAlarm(AActor* Causer, AActor* Source)
{
	// A sabotaged panel cannot raise its group.
	if (bSabotaged || bAlarmActive) { return; }

	bAlarmActive = true;
	if (StatusLight)
	{
		StatusLight->SetLightColor(FLinearColor(1.f, 0.05f, 0.05f));
		StatusLight->SetIntensity(900.f);
	}

	OnAlarmTriggered.Broadcast(AlarmGroupID, Causer);

	UWorld* World = GetWorld();
	if (!World) { return; }

	// Alert every guard in the group and request reinforcements.
	if (UBVSquadManager* Squad = World->GetSubsystem<UBVSquadManager>())
	{
		Squad->SetSquadAlarm(AlarmGroupID, true, GetActorLocation());
		Squad->CallReinforcements(AlarmGroupID, GetActorLocation());
	}

	if (UBlackVeilGameInstance* GI = World->GetGameInstance<UBlackVeilGameInstance>())
	{
		GI->RecordAlarm();
	}

	UE_LOG(LogBlackVeil, Warning, TEXT("ALARM RAISED in group %s"), *AlarmGroupID.ToString());
}

void ABVAlarmPanel::ClearAlarm()
{
	bAlarmActive = false;
	if (StatusLight)
	{
		StatusLight->SetLightColor(FLinearColor(0.1f, 1.f, 0.2f));
		StatusLight->SetIntensity(250.f);
	}

	if (UWorld* World = GetWorld())
	{
		if (UBVSquadManager* Squad = World->GetSubsystem<UBVSquadManager>())
		{
			Squad->SetSquadAlarm(AlarmGroupID, false, GetActorLocation());
		}
	}
}

void ABVAlarmPanel::DisableFor_Implementation(float Duration)
{
	// An EMP effectively sabotages the panel for its duration.
	SabotagePanel();
}

void ABVAlarmPanel::RestorePower_Implementation()
{
	// Power restoration does not un-sabotage a physically damaged panel.
}
