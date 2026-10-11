// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVAIController.h"
#include "BVGuardCharacter.h"
#include "BVPatrolComponent.h"
#include "BVAlertStateComponent.h"
#include "BVSquadManager.h"
#include "BVBlackboardKeys.h"
#include "Player/BVPlayerCharacter.h"
#include "Player/BVStealthComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BlackVeil.h"

ABVAIController::ABVAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("BVPerception"));
	SetPerceptionComponent(*PerceptionComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void ABVAIController::ConfigurePerception()
{
	if (!PerceptionComponent) { return; }

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = BaseSightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
	SightConfig->SetMaxAge(12.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = HearingRange;
	HearingConfig->SetMaxAge(8.f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABVAIController::HandlePerceptionUpdated);
}

void ABVAIController::BeginPlay()
{
	Super::BeginPlay();

	ConfigurePerception();

	if (UWorld* World = GetWorld())
	{
		SquadManager = World->GetSubsystem<UBVSquadManager>();
	}
}

void ABVAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABVGuardCharacter* Guard = Cast<ABVGuardCharacter>(InPawn))
	{
		if (BehaviorTreeAsset)
		{
			// Initialise the blackboard and start the tree.
			if (BehaviorTreeAsset->BlackboardAsset)
			{
				UseBlackboard(BehaviorTreeAsset->BlackboardAsset, nullptr);
			}
			RunBehaviorTree(BehaviorTreeAsset);
		}

		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->SetValueAsVector(BVBlackboardKeys::HomeLocation, Guard->GetActorLocation());
			BB->SetValueAsObject(BVBlackboardKeys::SelfActor, Guard);

			if (UBVPatrolComponent* Patrol = Guard->GetPatrolComponent())
			{
				BB->SetValueAsBool(BVBlackboardKeys::HasPatrolRoute, Patrol->HasRoute());
				BB->SetValueAsVector(BVBlackboardKeys::PatrolLocation, Patrol->GetCurrentWaypointLocation());
			}
		}

		if (UBVSquadManager* Squad = SquadManager.Get())
		{
			Squad->RegisterGuard(this);
		}
	}
}

ABVGuardCharacter* ABVAIController::GetGuard() const
{
	return Cast<ABVGuardCharacter>(GetPawn());
}

UBVAlertStateComponent* ABVAIController::GetAlertState() const
{
	const ABVGuardCharacter* Guard = GetGuard();
	return Guard ? Guard->GetAlertStateComponent() : nullptr;
}

float ABVAIController::GetEffectiveSightRange() const
{
	float Range = BaseSightRange;

	// Lighting and posture shrink how far the guard can actually pick the player
	// out, which is what makes shadows protective.
	if (const ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(
		GetWorld() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr))
	{
		if (const UBVStealthComponent* Stealth = Player->GetStealthComponent())
		{
			Range *= Stealth->GetDetectionMultiplier();
		}
	}

	// Guard archetype modifiers: surveillance operators see further, techs less.
	if (const ABVGuardCharacter* Guard = GetGuard())
	{
		switch (Guard->GuardType)
		{
		case EBVGuardType::Surveillance: Range *= 1.25f; break;
		case EBVGuardType::Elite:        Range *= 1.15f; break;
		case EBVGuardType::Tech:         Range *= 0.8f;  break;
		default: break;
		}
	}

	return Range;
}

bool ABVAIController::ValidateSight(AActor* SeenActor, const FVector& SeenLocation) const
{
	const ABVPlayerCharacter* Player = Cast<ABVPlayerCharacter>(SeenActor);
	if (!Player) { return true; }

	const float Dist = FVector::Dist(GetPawn() ? GetPawn()->GetActorLocation() : SeenLocation, SeenLocation);

	// Beyond the light-adjusted range the guard simply cannot resolve the target.
	if (Dist > GetEffectiveSightRange())
	{
		return false;
	}

	// In deep shadow the player is only detectable at knife range.
	if (const UBVStealthComponent* Stealth = Player->GetStealthComponent())
	{
		if (Stealth->IsInShadow() && Dist > BaseSightRange * DarkSightPenalty)
		{
			return false;
		}
	}
	return true;
}

void ABVAIController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) { return; }

	UBVAlertStateComponent* Alert = GetAlertState();
	if (!Alert) { return; }

	const bool bIsPlayer = Actor->IsA<ABVPlayerCharacter>();
	if (!bIsPlayer) { return; }

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			if (!ValidateSight(Actor, Stimulus.StimulusLocation))
			{
				return;
			}

			// Reaction delay: the guard needs a moment to register what they saw.
			PendingSightTarget = Actor;
			LastSightTime = GetWorld()->GetTimeSeconds();

			const float Dist = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
			const float Confidence = FMath::Clamp(1.f - (Dist / BaseSightRange), 0.25f, 1.f);
			Alert->ReportSighting(Stimulus.StimulusLocation, Confidence);

			OnSpottedPlayer.Broadcast(Actor, Stimulus.StimulusLocation);

			// Share the contact, then decide whether this is close enough for combat.
			if (UBVSquadManager* Squad = SquadManager.Get())
		{
				if (const ABVGuardCharacter* Guard = GetGuard())
				{
					Squad->BroadcastContact(Guard->SquadID, Stimulus.StimulusLocation,
						static_cast<uint8>(EBVAlertState::Alerted), GetPawn());
				}
			}

			if (Dist < BaseSightRange * 0.4f && Alert->GetSuspicion() > 60.f)
			{
				EnterCombat(Actor);
			}
		}
		else
		{
			// Lost sight: the guard keeps the last known point and searches there.
			PendingSightTarget = nullptr;
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			const float Loudness = FMath::Clamp(Stimulus.StimulusStrength, 0.2f, 1.5f);
			Alert->ReportNoise(Stimulus.StimulusLocation, Loudness);
		}
	}
}

void ABVAIController::InvestigateLocation(const FVector& Location)
{
	UBVAlertStateComponent* Alert = GetAlertState();
	if (!Alert) { return; }

	Alert->ReceiveCommunicatedContact(Location, EBVAlertState::Investigating);

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsVector(BVBlackboardKeys::InvestigateLocation, Location);
		BB->SetValueAsVector(BVBlackboardKeys::LastKnownLocation, Location);
		BB->SetValueAsBool(BVBlackboardKeys::HasLastKnownLocation, true);
	}
}

void ABVAIController::ReceiveSquadContact(const FVector& ApproxLocation, uint8 SuggestedAlertState, AActor* SharedBy)
{
	UBVAlertStateComponent* Alert = GetAlertState();
	if (!Alert) { return; }

	Alert->ReceiveCommunicatedContact(ApproxLocation, static_cast<EBVAlertState>(SuggestedAlertState));

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsVector(BVBlackboardKeys::LastKnownLocation, ApproxLocation);
		BB->SetValueAsBool(BVBlackboardKeys::HasLastKnownLocation, true);
	}
}

void ABVAIController::EnterCombat(AActor* Threat)
{
	UBVAlertStateComponent* Alert = GetAlertState();
	if (!Alert) { return; }

	Alert->SetAlertState(EBVAlertState::Combat);

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsObject(BVBlackboardKeys::TargetActor, Threat);
		BB->SetValueAsBool(BVBlackboardKeys::IsInCombat, true);
		if (Threat)
		{
			BB->SetValueAsVector(BVBlackboardKeys::LastKnownLocation, Threat->GetActorLocation());
			BB->SetValueAsBool(BVBlackboardKeys::HasLastKnownLocation, true);
		}
	}

	// Combat is loud: everyone in the squad is told.
	if (UBVSquadManager* Squad = SquadManager.Get())
	{
		if (const ABVGuardCharacter* Guard = GetGuard())
		{
			Squad->BroadcastContact(Guard->SquadID,
				Threat ? Threat->GetActorLocation() : GetPawn()->GetActorLocation(),
				static_cast<uint8>(EBVAlertState::Alerted), GetPawn());
		}
	}
}

void ABVAIController::ReportBodyFound(AActor* Body)
{
	UBVAlertStateComponent* Alert = GetAlertState();
	if (!Alert || !Body) { return; }

	Alert->SetAlertState(EBVAlertState::Searching);
	Alert->ReportSighting(Body->GetActorLocation(), 0.8f);

	if (UBVSquadManager* Squad = SquadManager.Get())
	{
		if (ABVGuardCharacter* Guard = GetGuard())
		{
			Squad->BroadcastContact(Guard->SquadID, Body->GetActorLocation(),
				static_cast<uint8>(EBVAlertState::Searching), GetPawn());
			Squad->CoordinateSearch(Guard->SquadID, Body->GetActorLocation(), 1200.f);
		}
	}
}

void ABVAIController::SyncBlackboard()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	UBVAlertStateComponent* Alert = GetAlertState();
	if (!BB || !Alert) { return; }

	BB->SetValueAsFloat(BVBlackboardKeys::Suspicion, Alert->GetSuspicion());
	BB->SetValueAsInt(BVBlackboardKeys::AlertState, static_cast<int32>(Alert->GetAlertState()));
	BB->SetValueAsBool(BVBlackboardKeys::HasLastKnownLocation, Alert->HasLastKnownLocation());
	if (Alert->HasLastKnownLocation())
	{
		BB->SetValueAsVector(BVBlackboardKeys::LastKnownLocation, Alert->GetLastKnownPlayerLocation());
	}
	BB->SetValueAsBool(BVBlackboardKeys::IsInCombat, Alert->GetAlertState() == EBVAlertState::Combat);
}

void ABVAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SyncBlackboard();

	ABVGuardCharacter* Guard = GetGuard();
	if (!Guard || Guard->IsIncapacitated()) { return; }

	// Periodically check for nearby bodies the guard can see. Uses the same
	// lighting-aware line of sight as live target detection.
	static const float BodyCheckInterval = 1.5f;
	if (FMath::Fmod(GetWorld()->GetTimeSeconds(), BodyCheckInterval) < DeltaSeconds)
	{
		for (TActorIterator<ABVGuardCharacter> It(GetWorld()); It; ++It)
		{
			ABVGuardCharacter* Other = *It;
			if (Other == Guard || !Other->IsIncapacitated() || Other->IsConcealed()) { continue; }

			const float Dist = FVector::Dist(Guard->GetActorLocation(), Other->GetActorLocation());
			if (Dist > BaseSightRange * 0.6f) { continue; }

			FCollisionQueryParams Params(SCENE_QUERY_STAT(BVBodySight), false, Guard);
			FHitResult Hit;
			const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit,
				Guard->GetActorLocation() + FVector(0.f, 0.f, 60.f),
				Other->GetActorLocation() + FVector(0.f, 0.f, 30.f), ECC_Visibility, Params);

			if (!bBlocked || Hit.GetActor() == Other)
			{
				ReportBodyFound(Other);
				break;
			}
		}
	}
}
