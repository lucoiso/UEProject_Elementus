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
			"Core",
			"CoreOnline",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"Niagara",
			"UMG",
			"ModularGameplay",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"GameFeatures",
			"ModularFeatures_ExtraActions",
			"JsonUtilities"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"EOSSDKHandler",
			"EOSShared",
			"OnlineSubsystemEOS",
			"OnlineSubsystem",
			"EOSVoiceChat",
			"VoiceChat"
		});
	}
}