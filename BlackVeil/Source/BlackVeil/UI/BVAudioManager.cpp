// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVAudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "BlackVeil.h"

void UBVAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogBlackVeil, Log, TEXT("Audio manager online."));
}

void UBVAudioManager::Deinitialize()
{
	Super::Deinitialize();
}

void UBVAudioManager::PlayBGM(FName TrackID, float FadeInSeconds)
{
	const TSoftObjectPtr<USoundBase>* Found = MusicCues.Find(TrackID);
	if (!Found || Found->IsNull())
	{
		UE_LOG(LogBlackVeil, Verbose, TEXT("No music cue registered for %s"), *TrackID.ToString());
		return;
	}

	USoundBase* Cue = Found->LoadSynchronous();
	if (!Cue) { return; }

	CurrentMusic = Cue;
	UGameplayStatics::PlaySound2D(GetGameInstance(), Cue, MasterVolume * MusicVolume);
}

void UBVAudioManager::StopBGM(float FadeOutSeconds)
{
	CurrentMusic = nullptr;
}

void UBVAudioManager::SetAlertMusicState(EBVGlobalAlertLevel Level)
{
	if (Level == CurrentAlertState) { return; }
	CurrentAlertState = Level;

	// Each alert tier has its own music bed; the calm bed is used for Normal
	// and Suspicious, the tension bed for Searching, the combat bed above that.
	FName Track = TEXT("BGM_Calm");
	switch (Level)
	{
	case EBVGlobalAlertLevel::Searching: Track = TEXT("BGM_Tension"); break;
	case EBVGlobalAlertLevel::Alarmed:   Track = TEXT("BGM_Tension"); break;
	case EBVGlobalAlertLevel::Combat:    Track = TEXT("BGM_Combat");  break;
	default: break;
	}
	PlayBGM(Track, 1.2f);
}

void UBVAudioManager::PlayFootstep(EBVSurfaceType Surface, float VolumeScale, const FVector& Location)
{
	const TSoftObjectPtr<USoundBase>* Found = FootstepCues.Find(Surface);
	if (!Found || Found->IsNull())
	{
		return;
	}

	if (USoundBase* Cue = Found->LoadSynchronous())
	{
		// Attenuated 3D so guards and the player hear footsteps spatially.
		UGameplayStatics::PlaySoundAtLocation(GetGameInstance(), Cue, Location,
			FMath::Clamp(VolumeScale, 0.05f, 1.f) * MasterVolume * SFXVolume);
	}
}

void UBVAudioManager::PlayGadgetSound(FName GadgetID, const FVector& Location)
{
	const TSoftObjectPtr<USoundBase>* Found = GadgetCues.Find(GadgetID);
	if (!Found || Found->IsNull()) { return; }

	if (USoundBase* Cue = Found->LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(GetGameInstance(), Cue, Location, MasterVolume * SFXVolume);
	}
}

void UBVAudioManager::PlayWeaponSound(FName WeaponID, bool bSuppressed, const FVector& Location)
{
	// Suppressed weapons use a different (quieter) cue; indoors vs outdoors is
	// handled by the attenuation settings on each cue asset.
	const FName CueID = bSuppressed ? FName(*(WeaponID.ToString() + TEXT("_Suppressed"))) : WeaponID;
	PlayGadgetSound(CueID, Location);
}

void UBVAudioManager::PlayRadioDialogue(FName DialogueID, FName SpeakerID)
{
	// Dialogue lines are data-driven; the subtitle widget displays the text while
	// this plays the matching voice cue.
	PlayGadgetSound(DialogueID, FVector::ZeroVector);
}

void UBVAudioManager::SetMasterVolume(float Value) { MasterVolume = FMath::Clamp(Value, 0.f, 1.f); }
void UBVAudioManager::SetMusicVolume(float Value)  { MusicVolume = FMath::Clamp(Value, 0.f, 1.f); }
void UBVAudioManager::SetSFXVolume(float Value)    { SFXVolume = FMath::Clamp(Value, 0.f, 1.f); }
void UBVAudioManager::SetVoiceVolume(float Value)  { VoiceVolume = FMath::Clamp(Value, 0.f, 1.f); }
