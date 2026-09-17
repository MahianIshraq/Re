// ReSharper disable InconsistentNaming

using UnrealBuildTool;

public class Re : ModuleRules
{
	public Re(ReadOnlyTargetRules Target) : base(Target)
	{
		ApplySharedSettings(this);

		PublicDependencyModuleNames.AddRange
		(
			[
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags"
			]
		);

		PrivateDependencyModuleNames.AddRange
		(
			[
				"EnhancedInput",
				"MoviePlayer",
				"Slate",
				"SlateCore",
				"UMG"
			]
		);
	}

	public static void ApplySharedSettings(ModuleRules Rules)
	{
		Rules.PCHUsage		= PCHUsageMode.UseExplicitOrSharedPCHs;
		Rules.IWYUSupport	= IWYUSupport.Full;
	}
}
