// ReSharper disable InconsistentNaming

using UnrealBuildTool;

public class ReEditorTarget : TargetRules
{
	public ReEditorTarget(TargetInfo Info) : base(Info)
	{
		Type = TargetType.Editor;

		ReTarget.ApplySharedSettings(this);

		ExtraModuleNames.AddRange
		(
			[
				"Re",
				"ReEditor"
			]
		);
	}
}
