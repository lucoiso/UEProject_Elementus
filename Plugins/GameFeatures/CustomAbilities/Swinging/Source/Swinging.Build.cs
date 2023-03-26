// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

using UnrealBuildTool;

public class Swinging : ModuleRules
{
	public Swinging(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Engine",
			"CoreUObject",
			"InputCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"GeometryCollectionEngine",
			"ElementusAbilitySystem"
		});

		SetupIrisSupport(Target);
	}
}