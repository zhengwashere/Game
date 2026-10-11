// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.

using UnrealBuildTool;

public class BlackVeil : ModuleRules
{
	public BlackVeil(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"GameplayTasks",
			"GameplayTags",
			"UMG",
			"Slate",
			"SlateCore",
			"Niagara",
			"MotionWarping",
			"AnimGraphRuntime",
			"GameplayAbilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"NetCore",
			"PhysicsCore",
			"ChaosVehicles"
		});

		// Allow Blueprint access to all public headers
		PublicIncludePaths.AddRange(new string[]
		{
			"BlackVeil/Core",
			"BlackVeil/Player",
			"BlackVeil/AI",
			"BlackVeil/Gadgets",
			"BlackVeil/Mission",
			"BlackVeil/UI",
			"BlackVeil/Interaction",
			"BlackVeil/World"
		});
	}
}
