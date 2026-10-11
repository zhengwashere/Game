// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/BVTypes.h"
#include "BVAudioManager.generated.h"

class USoundBase;
class USoundMix;
class USoundClass;

/**
 * Central audio hub. Owns music state (calm vs. alerted), surface-aware
 * footsteps, gadget/weapon one-shots and radio dialogue playback.
 */
UCLASS()
class BLACKVEIL_API UBVAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void PlayBGM(FName TrackID, float FadeInSeconds = 1.5f);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void StopBGM(float FadeOutSeconds = 1.0f);

	/** Switches the music bed as the mission's alert level changes. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void SetAlertMusicState(EBVGlobalAlertLevel Level);
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Audio") EBVGlobalAlertLevel GetAlertMusicState() const { return CurrentAlertState; }

	/** Surface-aware footstep. Volume scales with movement tier. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio")
	void PlayFootstep(EBVSurfaceType Surface, float VolumeScale, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio")
	void PlayGadgetSound(FName GadgetID, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio")
	void PlayWeaponSound(FName WeaponID, bool bSuppressed, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio")
	void PlayRadioDialogue(FName DialogueID, FName SpeakerID);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void SetMasterVolume(float Value);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void SetMusicVolume(float Value);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void SetSFXVolume(float Value);
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Audio") void SetVoiceVolume(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Audio") float MasterVolume = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Audio") float MusicVolume = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Audio") float SFXVolume = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackVeil|Audio") float VoiceVolume = 1.f;

	/** Soft references so audio content is swappable without recompiling. */
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Audio") TMap<FName, TSoftObjectPtr<USoundBase>> MusicCues;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Audio") TMap<EBVSurfaceType, TSoftObjectPtr<USoundBase>> FootstepCues;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Audio") TMap<FName, TSoftObjectPtr<USoundBase>> GadgetCues;

protected:
	UPROPERTY() TObjectPtr<USoundBase> CurrentMusic;
	UPROPERTY() EBVGlobalAlertLevel CurrentAlertState = EBVGlobalAlertLevel::Normal;
};
