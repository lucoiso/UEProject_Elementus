// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectElementusTarget : TargetRules
{
	public ProjectElementusTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("ProjectElementus");
	}
}