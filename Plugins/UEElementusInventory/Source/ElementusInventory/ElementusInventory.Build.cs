// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

using UnrealBuildTool;

public class ElementusInventory : ModuleRules
{
	public ElementusInventory(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(
		new[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(
		new[]
		{
			"Engine",
			"CoreUObject",
			"GameplayTags"
		});
	}
}