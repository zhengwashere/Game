// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVBriefingWidget.h"
#include "Core/BlackVeilGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVBriefingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	int32 Index = 1;
	if (const UBlackVeilGameInstance* GI = GetGameInstance<UBlackVeilGameInstance>())
	{
		Index = GI->GetCurrentMissionIndex();
	}
	LoadBriefingForMission(Index);
}

void UBVBriefingWidget::LoadBriefingForMission(int32 MissionIndex)
{
	const FString Path = FString::Printf(TEXT("/Game/Data/Missions/DA_Mission_%02d.DA_Mission_%02d"), MissionIndex, MissionIndex);
	MissionData = LoadObject<UBVMissionData>(nullptr, *Path);

	if (!MissionData)
	{
		UE_LOG(LogBlackVeil, Warning, TEXT("Briefing could not load mission %d"), MissionIndex);
		return;
	}

	MissionName = MissionData->MissionName;
	LocationName = MissionData->LocationName;
	BriefingText = MissionData->BriefingText;
	Objectives = MissionData->Objectives;

	IntelIDs.Reset();
	for (const FBVIntelItem& Item : MissionData->IntelItems)
	{
		IntelIDs.Add(Item.IntelID);
	}

	OnBriefingLoaded(MissionName, LocationName, BriefingText);
}

void UBVBriefingWidget::LaunchMission()
{
	const int32 Index = MissionData ? MissionData->MissionIndex
		: (GetGameInstance<UBlackVeilGameInstance>() ? GetGameInstance<UBlackVeilGameInstance>()->GetCurrentMissionIndex() : 1);

	const FString LevelName = FString::Printf(TEXT("M%02d"), Index);
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}

void UBVBriefingWidget::OpenEquipmentSelect()
{
	// Handled by the menu Blueprint swapping to the equipment widget.
}
