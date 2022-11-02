// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEEOSSDKHandler

using System.IO;
using UnrealBuildTool;

public class EOSSDKHandler : ModuleRules
{
	public EOSSDKHandler(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core"
		});

		PublicDefinitions.Add("WITH_EOS_SDK=1");

		PublicIncludePaths.AddRange(new[]
		{
			Path.Combine(EngineDirectory, "Plugins/Online/OnlineSubsystemEOS/Source/OnlineSubsystemEOS/Private/"),
			Path.Combine(ModuleDirectory, "EOS/SDK/Include")
		});
	}
}