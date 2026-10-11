// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVGameplayTags.h"

namespace BVTags
{
	// Alert states
	UE_DEFINE_GAMEPLAY_TAG(Alert_Unaware, "BlackVeil.Alert.Unaware");
	UE_DEFINE_GAMEPLAY_TAG(Alert_Suspicious, "BlackVeil.Alert.Suspicious");
	UE_DEFINE_GAMEPLAY_TAG(Alert_Investigating, "BlackVeil.Alert.Investigating");
	UE_DEFINE_GAMEPLAY_TAG(Alert_Searching, "BlackVeil.Alert.Searching");
	UE_DEFINE_GAMEPLAY_TAG(Alert_Alerted, "BlackVeil.Alert.Alerted");
	UE_DEFINE_GAMEPLAY_TAG(Alert_Combat, "BlackVeil.Alert.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Alert_Returning, "BlackVeil.Alert.Returning");

	// Player states
	UE_DEFINE_GAMEPLAY_TAG(Player_Visible, "BlackVeil.Player.Visible");
	UE_DEFINE_GAMEPLAY_TAG(Player_InShadow, "BlackVeil.Player.InShadow");
	UE_DEFINE_GAMEPLAY_TAG(Player_Silent, "BlackVeil.Player.Silent");
	UE_DEFINE_GAMEPLAY_TAG(Player_Moving, "BlackVeil.Player.Moving");
	UE_DEFINE_GAMEPLAY_TAG(Player_Sprinting, "BlackVeil.Player.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(Player_InCover, "BlackVeil.Player.InCover");
	UE_DEFINE_GAMEPLAY_TAG(Player_CarryingBody, "BlackVeil.Player.CarryingBody");
	UE_DEFINE_GAMEPLAY_TAG(Player_Hidden, "BlackVeil.Player.Hidden");

	// Noise levels
	UE_DEFINE_GAMEPLAY_TAG(Noise_None, "BlackVeil.Noise.None");
	UE_DEFINE_GAMEPLAY_TAG(Noise_Light, "BlackVeil.Noise.Light");
	UE_DEFINE_GAMEPLAY_TAG(Noise_Medium, "BlackVeil.Noise.Medium");
	UE_DEFINE_GAMEPLAY_TAG(Noise_Heavy, "BlackVeil.Noise.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Noise_Gunshot, "BlackVeil.Noise.Gunshot");

	// Gadget categories
	UE_DEFINE_GAMEPLAY_TAG(Gadget_NoiseMaker, "BlackVeil.Gadget.NoiseMaker");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_Flashbang, "BlackVeil.Gadget.Flashbang");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_Smoke, "BlackVeil.Gadget.Smoke");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_EMP, "BlackVeil.Gadget.EMP");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_StickyCamera, "BlackVeil.Gadget.StickyCamera");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_HackDevice, "BlackVeil.Gadget.HackDevice");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_FiberOptic, "BlackVeil.Gadget.FiberOptic");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_Lockpick, "BlackVeil.Gadget.Lockpick");
	UE_DEFINE_GAMEPLAY_TAG(Gadget_Detector, "BlackVeil.Gadget.Detector");

	// Interaction
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Door, "BlackVeil.Interaction.Door");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Lock, "BlackVeil.Interaction.Lock");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Terminal, "BlackVeil.Interaction.Terminal");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Camera, "BlackVeil.Interaction.Camera");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Body, "BlackVeil.Interaction.Body");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Intel, "BlackVeil.Interaction.Intel");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_AlarmPanel, "BlackVeil.Interaction.AlarmPanel");

	// Mission events
	UE_DEFINE_GAMEPLAY_TAG(Mission_Started, "BlackVeil.Mission.Started");
	UE_DEFINE_GAMEPLAY_TAG(Mission_ObjectiveComplete, "BlackVeil.Mission.ObjectiveComplete");
	UE_DEFINE_GAMEPLAY_TAG(Mission_Failed, "BlackVeil.Mission.Failed");
	UE_DEFINE_GAMEPLAY_TAG(Mission_AlarmTriggered, "BlackVeil.Mission.AlarmTriggered");
	UE_DEFINE_GAMEPLAY_TAG(Mission_CheckpointReached, "BlackVeil.Mission.CheckpointReached");

	// Guard archetypes
	UE_DEFINE_GAMEPLAY_TAG(Guard_Standard, "BlackVeil.Guard.Standard");
	UE_DEFINE_GAMEPLAY_TAG(Guard_Armored, "BlackVeil.Guard.Armored");
	UE_DEFINE_GAMEPLAY_TAG(Guard_Tech, "BlackVeil.Guard.Tech");
	UE_DEFINE_GAMEPLAY_TAG(Guard_Surveillance, "BlackVeil.Guard.Surveillance");
	UE_DEFINE_GAMEPLAY_TAG(Guard_Commander, "BlackVeil.Guard.Commander");
	UE_DEFINE_GAMEPLAY_TAG(Guard_Elite, "BlackVeil.Guard.Elite");
}
