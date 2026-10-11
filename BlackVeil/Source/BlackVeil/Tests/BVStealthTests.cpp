// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
//
// Automated tests for the pure-logic parts of the stealth systems. These do not
// need a world, so they run in the editor's Session Frontend > Automation tab
// (filter: BlackVeil) without loading a map.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "World/BVNoiseSubsystem.h"
#include "Core/BlackVeilGameInstance.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBVNoiseRadiusTest,
	"BlackVeil.Stealth.NoiseRadiusTiers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBVNoiseRadiusTest::RunTest(const FString& Parameters)
{
	// Silent movement must produce no audible radius at all.
	TestEqual(TEXT("Silent noise radius is zero"),
		UBVNoiseSubsystem::GetBaseRadiusForLevel(EBVNoiseLevel::Silent), 0.f);

	// Tiers must be strictly increasing so louder really is louder.
	const float Light = UBVNoiseSubsystem::GetBaseRadiusForLevel(EBVNoiseLevel::Light);
	const float Medium = UBVNoiseSubsystem::GetBaseRadiusForLevel(EBVNoiseLevel::Medium);
	const float Heavy = UBVNoiseSubsystem::GetBaseRadiusForLevel(EBVNoiseLevel::Heavy);
	const float Deafening = UBVNoiseSubsystem::GetBaseRadiusForLevel(EBVNoiseLevel::Deafening);

	TestTrue(TEXT("Light < Medium"), Light < Medium);
	TestTrue(TEXT("Medium < Heavy"), Medium < Heavy);
	TestTrue(TEXT("Heavy < Deafening"), Heavy < Deafening);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBVSurfaceNoiseTest,
	"BlackVeil.Stealth.SurfaceAffectsNoise",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBVSurfaceNoiseTest::RunTest(const FString& Parameters)
{
	// Jogging on carpet must be quieter than jogging on gravel.
	const EBVNoiseLevel Carpet = UBVNoiseSubsystem::GetNoiseLevelForSpeed(EBVMovementSpeed::Jog, EBVSurfaceType::Carpet);
	const EBVNoiseLevel Gravel = UBVNoiseSubsystem::GetNoiseLevelForSpeed(EBVMovementSpeed::Jog, EBVSurfaceType::Gravel);

	TestTrue(TEXT("Carpet is quieter than gravel"), static_cast<uint8>(Carpet) < static_cast<uint8>(Gravel));

	// Prone movement is silent on soft surfaces and quiet even on hard ones.
	const EBVNoiseLevel ProneConcrete = UBVNoiseSubsystem::GetNoiseLevelForSpeed(EBVMovementSpeed::Prone, EBVSurfaceType::Concrete);
	TestEqual(TEXT("Prone on concrete is silent"), static_cast<uint8>(ProneConcrete), static_cast<uint8>(EBVNoiseLevel::Silent));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBVMissionGradeTest,
	"BlackVeil.Mission.GradeCalculation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBVMissionGradeTest::RunTest(const FString& Parameters)
{
	UBlackVeilGameInstance* GI = NewObject<UBlackVeilGameInstance>();

	// A flawless run must earn the top grade.
	FMissionScore Perfect;
	Perfect.Detections = 0;
	Perfect.Alarms = 0;
	Perfect.Kills = 0;
	Perfect.IntelCollected = 3;
	Perfect.OptionalObjectivesCompleted = 2;
	TestEqual(TEXT("Flawless run grades S"), GI->CalculateGrade(Perfect), FString(TEXT("S")));

	// A loud, lethal run must grade at the bottom.
	FMissionScore Loud;
	Loud.Detections = 6;
	Loud.Alarms = 4;
	Loud.Kills = 8;
	Loud.IntelCollected = 0;
	Loud.OptionalObjectivesCompleted = 0;
	TestEqual(TEXT("Loud run grades D"), GI->CalculateGrade(Loud), FString(TEXT("D")));

	// A single alarm must not be able to produce a top grade.
	FMissionScore OneAlarm;
	OneAlarm.Alarms = 1;
	OneAlarm.IntelCollected = 3;
	TestTrue(TEXT("One alarm cannot grade S"), GI->CalculateGrade(OneAlarm) != TEXT("S"));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBVCampaignProgressionTest,
	"BlackVeil.Campaign.ProgressionUnlocks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBVCampaignProgressionTest::RunTest(const FString& Parameters)
{
	UBlackVeilGameInstance* GI = NewObject<UBlackVeilGameInstance>();

	TestEqual(TEXT("Campaign starts with only mission 1 unlocked"), GI->CampaignData.HighestMissionUnlocked, 1);

	TestTrue(TEXT("Lockpick is available from the start"), GI->IsGadgetUnlocked(TEXT("Lockpick")));
	TestFalse(TEXT("EMP is not available from the start"), GI->IsGadgetUnlocked(TEXT("EMP")));

	GI->UnlockGadget(TEXT("EMP"));
	TestTrue(TEXT("EMP unlocks when awarded"), GI->IsGadgetUnlocked(TEXT("EMP")));

	// Completing mission 1 must unlock mission 2.
	FMissionScore Score;
	Score.MissionIndex = 1;
	Score.Grade = TEXT("A");
	GI->RecordMissionScore(Score);
	TestTrue(TEXT("Completing mission 1 unlocks mission 2"), GI->CampaignData.HighestMissionUnlocked >= 2);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
