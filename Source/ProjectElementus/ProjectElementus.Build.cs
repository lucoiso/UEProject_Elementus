// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

using UnrealBuildTool;

public class ProjectElementus : ModuleRules
{
	public ProjectElementus(ReadOnlyTargetRules target) : base(target)
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
			"CoreOnline",
			"InputCore",
			"EnhancedInput",
			"ModularGameplay",
			"GameFeatures",
			"ModularFeatures_ExtraActions",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"UMG",
			"Niagara",
			"EOSSDKHandler",
			"EOSShared",
			"OnlineSubsystemEOS",
			"OnlineSubsystem",
			"EOSVoiceChat",
			"VoiceChat",
			"JsonUtilities",
			"ElementusInventory"
		});
	}
}