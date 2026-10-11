// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

/// <summary>
/// Standalone game target. This is what you build to produce a shippable
/// executable.
/// </summary>
public class BlackVeilTarget : TargetRules
{
	public BlackVeilTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		// V5 is the build-settings generation used by the 5.4-5.6 project
		// templates. If UBT ever rejects it on a future engine version, switch to
		// BuildSettingsVersion.Latest.
		DefaultBuildSettings = BuildSettingsVersion.V5;

		// "Latest" keeps the include order aligned with whatever engine version is
		// installed, which avoids hard-coding a version that may not exist.
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "BlackVeil" });
	}
}
