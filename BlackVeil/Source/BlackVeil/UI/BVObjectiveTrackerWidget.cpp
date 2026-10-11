// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVObjectiveTrackerWidget.h"
#include "Mission/BVMissionManager.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVObjectiveTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->OnObjectivesRefreshed.AddDynamic(this, &UBVObjectiveTrackerWidget::HandleObjectivesRefreshed);
			MM->OnObjectiveUpdated.AddDynamic(this, &UBVObjectiveTrackerWidget::HandleObjectiveUpdated);

			// Pull whatever is already known.
			HandleObjectivesRefreshed(MM->GetObjectiveDisplayData());
		}
	}
}

void UBVObjectiveTrackerWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		if (UBVMissionManager* MM = World->GetSubsystem<UBVMissionManager>())
		{
			MM->OnObjectivesRefreshed.RemoveDynamic(this, &UBVObjectiveTrackerWidget::HandleObjectivesRefreshed);
			MM->OnObjectiveUpdated.RemoveDynamic(this, &UBVObjectiveTrackerWidget::HandleObjectiveUpdated);
		}
	}
	Super::NativeDestruct();
}

void UBVObjectiveTrackerWidget::HandleObjectivesRefreshed(const TArray<FBVObjectiveDisplayData>& AllObjectives)
{
	Objectives = AllObjectives;
	OnObjectivesChanged(Objectives);
}

void UBVObjectiveTrackerWidget::HandleObjectiveUpdated(const FBVObjectiveDisplayData& Objective)
{
	// Replace or add the entry, then notify the view.
	bool bFound = false;
	for (FBVObjectiveDisplayData& Existing : Objectives)
	{
		if (Existing.ObjectiveID == Objective.ObjectiveID)
		{
			Existing = Objective;
			bFound = true;
			break;
		}
	}
	if (!bFound) { Objectives.Add(Objective); }

	OnObjectiveUpdated(Objective);
	OnObjectivesChanged(Objectives);
}
