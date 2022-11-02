// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

using UnrealBuildTool;

public class ElementusInventoryEditor : ModuleRules
{
	public ElementusInventoryEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"ElementusInventory",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"AssetTools",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"WorkspaceMenuStructure",
				"PropertyEditor",
				"GameplayTags"
			});
	}
}