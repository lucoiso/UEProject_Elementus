// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ElementusInventory : ModuleRules
{
	public ElementusInventory(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Engine",
				"CoreUObject"
			}
		);
	}
}