// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

/// <summary>
/// Editor target. This is what you build to open the project in the Unreal
/// Editor, and the target that must succeed before any Blueprint or data asset
/// can be created.
/// </summary>
public class BlackVeilEditorTarget : TargetRules
{
	public BlackVeilEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "BlackVeil" });
	}
}
