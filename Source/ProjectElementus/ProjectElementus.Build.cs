// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

using UnrealBuildTool;

public class ProjectElementus : ModuleRules
{
	public ProjectElementus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"ElementusInventory",
			"GameplayTags"
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
			"JsonUtilities"
        });
	}
}