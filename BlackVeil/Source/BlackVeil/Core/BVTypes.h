// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BVTypes.generated.h"

/** Movement speed tiers with associated noise emission. */
UENUM(BlueprintType)
enum class EBVMovementSpeed : uint8
{
	Prone		UMETA(DisplayName = "Prone"),
	Crouch		UMETA(DisplayName = "Crouch"),
	Silent		UMETA(DisplayName = "Silent Walk"),
	Walk		UMETA(DisplayName = "Walk"),
	Jog			UMETA(DisplayName = "Jog"),
	Sprint		UMETA(DisplayName = "Sprint")
};

/** Discrete noise levels used by the sound propagation system and AI hearing. */
UENUM(BlueprintType)
enum class EBVNoiseLevel : uint8
{
	Silent		UMETA(DisplayName = "Silent"),
	Light		UMETA(DisplayName = "Light"),
	Medium		UMETA(DisplayName = "Medium"),
	Heavy		UMETA(DisplayName = "Heavy"),
	Deafening	UMETA(DisplayName = "Deafening")
};

/** Ground surface types for footstep audio and noise. */
UENUM(BlueprintType)
enum class EBVSurfaceType : uint8
{
	Concrete	UMETA(DisplayName = "Concrete"),
	Metal		UMETA(DisplayName = "Metal"),
	Wood		UMETA(DisplayName = "Wood"),
	Carpet		UMETA(DisplayName = "Carpet"),
	Grass		UMETA(DisplayName = "Grass"),
	Water		UMETA(DisplayName = "Water"),
	Gravel		UMETA(DisplayName = "Gravel"),
	Glass		UMETA(DisplayName = "Glass"),
	Mud			UMETA(DisplayName = "Mud")
};

/** Player posture, affects visibility, silhouette and takedown availability. */
UENUM(BlueprintType)
enum class EBVPosture : uint8
{
	Standing	UMETA(DisplayName = "Standing"),
	Crouching	UMETA(DisplayName = "Crouching"),
	Prone		UMETA(DisplayName = "Prone"),
	Hanging		UMETA(DisplayName = "Hanging"),
	InCover		UMETA(DisplayName = "In Cover")
};

/** Takedown classifications. */
UENUM(BlueprintType)
enum class EBVTakedownType : uint8
{
	Behind		UMETA(DisplayName = "From Behind"),
	Above		UMETA(DisplayName = "From Above"),
	Corner		UMETA(DisplayName = "Around Corner"),
	Choke		UMETA(DisplayName = "Choke Hold"),
	Ledge		UMETA(DisplayName = "Ledge Pull")
};

/** Lethality of a takedown or weapon hit. */
UENUM(BlueprintType)
enum class EBVLethality : uint8
{
	NonLethal	UMETA(DisplayName = "Non-Lethal"),
	Lethal		UMETA(DisplayName = "Lethal")
};

/** High-level player alert awareness used to drive HUD and music. */
UENUM(BlueprintType)
enum class EBVGlobalAlertLevel : uint8
{
	Normal		UMETA(DisplayName = "Normal"),
	Suspicious	UMETA(DisplayName = "Suspicious"),
	Searching	UMETA(DisplayName = "Searching"),
	Alarmed		UMETA(DisplayName = "Alarmed"),
	Combat		UMETA(DisplayName = "Combat")
};

/** Objective lifecycle state. */
UENUM(BlueprintType)
enum class EBVObjectiveState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Active		UMETA(DisplayName = "Active"),
	Completed	UMETA(DisplayName = "Completed"),
	Failed		UMETA(DisplayName = "Failed")
};

/** Objective classification drives HUD display and completion requirements. */
UENUM(BlueprintType)
enum class EBVObjectiveType : uint8
{
	ReachLocation	UMETA(DisplayName = "Reach Location"),
	Interact		UMETA(DisplayName = "Interact With"),
	Eliminate		UMETA(DisplayName = "Eliminate Target"),
	Incapacitate	UMETA(DisplayName = "Incapacitate Target"),
	Retrieve		UMETA(DisplayName = "Retrieve Item"),
	Hack			UMETA(DisplayName = "Hack System"),
	Sabotage		UMETA(DisplayName = "Sabotage"),
	Escort			UMETA(DisplayName = "Escort Asset"),
	Custom			UMETA(DisplayName = "Custom")
};

/** Reason a mission ends in failure. */
UENUM(BlueprintType)
enum class EBVFailureReason : uint8
{
	None			UMETA(DisplayName = "None"),
	PlayerKilled	UMETA(DisplayName = "Player Killed"),
	ObjectiveFailed	UMETA(DisplayName = "Objective Failed"),
	AssetLost		UMETA(DisplayName = "Asset Lost"),
	TimerExpired	UMETA(DisplayName = "Timer Expired"),
	Detected		UMETA(DisplayName = "Detected (Mission Critical)")
};

/** Status effects applied to characters (player or AI). */
UENUM(BlueprintType)
enum class EBVStatusEffect : uint8
{
	None		UMETA(DisplayName = "None"),
	Blinded		UMETA(DisplayName = "Blinded"),
	Deafened	UMETA(DisplayName = "Deafened"),
	Stunned		UMETA(DisplayName = "Stunned"),
	Disrupted	UMETA(DisplayName = "Disrupted"),
	Unconscious	UMETA(DisplayName = "Unconscious"),
	Dead		UMETA(DisplayName = "Dead")
};

/** Interaction categories used by the interaction framework for prioritization. */
UENUM(BlueprintType)
enum class EBVInteractionType : uint8
{
	None		UMETA(DisplayName = "None"),
	Door		UMETA(DisplayName = "Door"),
	Lock		UMETA(DisplayName = "Lock"),
	Terminal	UMETA(DisplayName = "Terminal"),
	Camera		UMETA(DisplayName = "Camera"),
	Body		UMETA(DisplayName = "Body"),
	Intel		UMETA(DisplayName = "Intel"),
	AlarmPanel	UMETA(DisplayName = "Alarm Panel"),
	Switch		UMETA(DisplayName = "Switch"),
	HidingSpot	UMETA(DisplayName = "Hiding Spot"),
	Pickup		UMETA(DisplayName = "Pickup"),
	Ladder		UMETA(DisplayName = "Ladder"),
	Vent		UMETA(DisplayName = "Vent")
};

/** Cover stance relative to a cover surface. */
UENUM(BlueprintType)
enum class EBVCoverStance : uint8
{
	None		UMETA(DisplayName = "None"),
	Low			UMETA(DisplayName = "Low Cover"),
	High		UMETA(DisplayName = "High Cover"),
	CornerLeft	UMETA(DisplayName = "Corner Left"),
	CornerRight	UMETA(DisplayName = "Corner Right")
};

/** A single objective's runtime display data for the HUD. */
USTRUCT(BlueprintType)
struct FBVObjectiveDisplayData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName ObjectiveID;
	UPROPERTY(BlueprintReadOnly) FText Description;
	UPROPERTY(BlueprintReadOnly) EBVObjectiveType Type = EBVObjectiveType::Custom;
	UPROPERTY(BlueprintReadOnly) EBVObjectiveState State = EBVObjectiveState::Inactive;
	UPROPERTY(BlueprintReadOnly) bool bIsOptional = false;
	UPROPERTY(BlueprintReadOnly) int32 CurrentCount = 0;
	UPROPERTY(BlueprintReadOnly) int32 RequiredCount = 1;
	UPROPERTY(BlueprintReadOnly) bool bHasLocationMarker = false;
	UPROPERTY(BlueprintReadOnly) FVector WorldLocation = FVector::ZeroVector;
};

/** Structured payload for a noise event broadcast through the world subsystem. */
USTRUCT(BlueprintType)
struct FBVNoiseEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) float Radius = 0.f;
	UPROPERTY(BlueprintReadOnly) EBVNoiseLevel Level = EBVNoiseLevel::Light;
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> Instigator;
	UPROPERTY(BlueprintReadOnly) bool bIsGunshot = false;
	UPROPERTY(BlueprintReadOnly) FName Tag;
};

/** Interaction prompt presented to the player. */
USTRUCT(BlueprintType)
struct FBVInteractionPrompt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FText ActionText;
	UPROPERTY(BlueprintReadOnly) FName IconTag;
	UPROPERTY(BlueprintReadOnly) float HoldDuration = 0.f;
	UPROPERTY(BlueprintReadOnly) EBVInteractionType InteractionType = EBVInteractionType::None;
	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> TargetActor;
	UPROPERTY(BlueprintReadOnly) bool bIsAvailable = false;
	UPROPERTY(BlueprintReadOnly) FText UnavailableReason;
};

/** Per-guard personality config that biases AI decision-making. */
USTRUCT(BlueprintType)
struct FBVGuardPersonality
{
	GENERATED_BODY()

	/** 0 = oblivious, 1 = hyper-vigilant. Scales suspicion gain. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Vigilance = 0.5f;

	/** 0 = cowardly, 1 = fearless. Scales how aggressively they push into combat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Bravery = 0.5f;

	/** 0 = lone wolf, 1 = team player. Scales how often they communicate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Teamwork = 0.7f;

	/** Reaction delay in seconds before acting on a sighting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.05", ClampMax = "3.0"))
	float ReactionDelay = 0.5f;

	/** How long a sighting is remembered, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "2.0", ClampMax = "120.0"))
	float MemoryDuration = 25.f;

	/** Chance per second the guard gives up a search and returns to patrol. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SearchPatience = 0.35f;
};
