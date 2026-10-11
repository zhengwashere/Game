// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/BVInteractableActor.h"
#include "BVIntelActor.generated.h"

/** Collectible intelligence: a document, drive or recording the player grabs. */
UCLASS()
class BLACKVEIL_API ABVIntelActor : public ABVInteractableActor
{
	GENERATED_BODY()

public:
	ABVIntelActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Intel") FName IntelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Intel") FText IntelTitle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Intel", meta = (MultiLine = true)) FText IntelBody;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Intel") FName IntelCategory;

	virtual bool ExecuteInteraction_Implementation(FName ActionID, ABVPlayerCharacter* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components") TObjectPtr<class UPointLightComponent> GlowLight;
};
