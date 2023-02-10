// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

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
			"NetCore",
			"InputCore",
			"EnhancedInput",
			"ModularGameplay",
			"GameFeatures",
			"ModularFeatures_ExtraActions",
			"GameplayAbilities",
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
			"DeveloperSettings",
			"MoviePlayer",
			"ElementusAbilitySystem"
		});

		SetupIrisSupport(Target);
	}
}