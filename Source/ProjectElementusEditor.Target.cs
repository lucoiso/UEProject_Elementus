// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectElementusEditorTarget : TargetRules
{
	public ProjectElementusEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("ProjectElementus");
	}
}