// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/BVTypes.h"
#include "BVNoiseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnNoiseEvent, const FBVNoiseEvent&, NoiseEvent);

/**
 * Central hub for all acoustic events in a level.
 *
 * Anything that makes noise (footsteps, doors, gunshots, gadgets, broken glass)
 * reports here. AI hearing consumes these events through the AI perception system,
 * and the audio manager subscribes for the noise-ring HUD feedback.
 *
 * Occlusion: radius is scaled down through walls/floors so a gunshot on the far
 * side of a thick wall is heard more faintly, matching the "no omniscience" rule.
 */
UCLASS()
class BLACKVEIL_API UBVNoiseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Broadcast a noise event. Called by movement, interaction and gadget systems.
	 * @param Location   World-space origin of the sound.
	 * @param Level      Discrete loudness category.
	 * @param Instigator Actor that produced the noise (may be null for world ambience).
	 * @param bIsGunshot Gunshots trigger higher-priority reactions and travel further.
	 * @param Tag        Optional semantic tag (e.g. Footstep, DoorSlam, GlassBreak).
	 */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Noise")
	void ReportNoise(const FVector& Location, EBVNoiseLevel Level, AActor* Instigator,
		bool bIsGunshot = false, FName Tag = NAME_None);

	/** Base audible radius for a noise level, before surface/occlusion modifiers. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Noise")
	static float GetBaseRadiusForLevel(EBVNoiseLevel Level);

	/** Convert a movement speed tier into a noise level. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Noise")
	static EBVNoiseLevel GetNoiseLevelForSpeed(EBVMovementSpeed Speed, EBVSurfaceType Surface);

	/** True when a listener can hear the event, accounting for distance and occlusion. */
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Noise")
	bool CanHearEvent(const FBVNoiseEvent& Event, const FVector& ListenerLocation, float ListenerMultiplier) const;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Noise")
	FBVOnNoiseEvent OnNoiseReported;

private:
	/** Apply a surface multiplier to how far a footstep/noise carries. */
	static float GetSurfaceNoiseMultiplier(EBVSurfaceType Surface);
};
