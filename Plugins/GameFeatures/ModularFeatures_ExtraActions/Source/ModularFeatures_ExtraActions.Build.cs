using UnrealBuildTool;

public class ModularFeatures_ExtraActions : ModuleRules
{
    public ModularFeatures_ExtraActions(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "GameFeatures",            
            "ModularGameplay",
        });
    }
}