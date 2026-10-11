// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/BlackVeilGameInstance.h"
#include "BlackVeilSaveGame.generated.h"

/** Persisted campaign state: unlocks, mission grades, collected intel. */
UCLASS()
class BLACKVEIL_API UBlackVeilSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "BlackVeil|Save")
	FCampaignSaveData CampaignData;

	UPROPERTY(BlueprintReadWrite, Category = "BlackVeil|Save")
	FString SaveVersion = TEXT("1.0");

	/** Wall-clock time the save was written, for the Continue screen. */
	UPROPERTY(BlueprintReadWrite, Category = "BlackVeil|Save")
	FDateTime SaveTimestamp;
};
