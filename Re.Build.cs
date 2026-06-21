using UnrealBuildTool;

public class Re : ModuleRules
{
	public Re(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange
		(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"MoviePlayer",
				"UMG",
				"Slate",
				"SlateCore",
				"GameplayTags"
			}
		);
	}
}