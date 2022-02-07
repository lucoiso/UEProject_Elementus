// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectElementusEditorTarget : TargetRules
{
    public ProjectElementusEditorTarget(TargetInfo target) : base(target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("ProjectElementus");
    }
}