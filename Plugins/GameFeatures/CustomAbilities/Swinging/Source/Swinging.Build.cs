using UnrealBuildTool;

public class Swinging : ModuleRules
{
    public Swinging(ReadOnlyTargetRules target) : base(target)
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