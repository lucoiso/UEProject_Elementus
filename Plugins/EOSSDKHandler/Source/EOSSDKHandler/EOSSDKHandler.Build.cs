// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEModularFeatures_ExtraActions

using System.IO;
using UnrealBuildTool;

public class EOSSDKHandler : ModuleRules
{
	public EOSSDKHandler(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core"
		});
		
		PublicDefinitions.Add("WITH_EOS_SDK=true");
		
		PublicIncludePaths.Add(Path.Combine(EngineDirectory, 
			"Plugins/Online/OnlineSubsystemEOS/Source/OnlineSubsystemEOS/Private/"));
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "EOS/SDK/Include"));
	}
}
