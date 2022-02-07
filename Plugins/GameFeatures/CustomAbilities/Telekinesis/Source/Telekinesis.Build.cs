using UnrealBuildTool;

public class Telekinesis : ModuleRules
{
    public Telekinesis(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "ProjectElementus"
        });
    }
}