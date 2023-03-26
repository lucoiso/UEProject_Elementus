using UnrealBuildTool;

public class Telekinesis : ModuleRules
{
	public Telekinesis(ReadOnlyTargetRules Target) : base(Target)
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
			"ElementusAbilitySystem"
		});

		SetupIrisSupport(Target);
	}
}