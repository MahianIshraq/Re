// ReSharper disable InconsistentNaming

using UnrealBuildTool;

public class ReTarget : TargetRules
{
	public ReTarget(TargetInfo Info) : base(Info)
	{
		Type = TargetType.Game;

		ApplySharedSettings(this);

		ExtraModuleNames.AddRange
		(
			[
				"Re"
			]
		);
	}

	internal static void ApplySharedSettings(TargetRules Rules)
	{
		Rules.DefaultBuildSettings	= BuildSettingsVersion.V7;
		Rules.IncludeOrderVersion	= EngineIncludeOrderVersion.Unreal5_8;
		Rules.bUseUnityBuild		= false;
	}
}
