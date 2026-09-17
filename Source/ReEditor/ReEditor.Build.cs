// ReSharper disable InconsistentNaming

using UnrealBuildTool;

public class ReEditor : ModuleRules
{
	public ReEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		Re.ApplySharedSettings(this);

		PrivateDependencyModuleNames.AddRange
		(
			[
				"AssetRegistry",
				"Core",
				"CoreUObject",
				"EditorSubsystem",
				"Engine",
				"InputCore",
				"LevelEditor",
				"PlacementMode",
				"PropertyEditor",
				"Re",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"UnrealEd",
				"WorkspaceMenuStructure"
			]
		);
	}
}
