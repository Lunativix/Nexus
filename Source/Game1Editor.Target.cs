using UnrealBuildTool;
using System.Collections.Generic;

public class Game1EditorTarget : TargetRules
{
	public Game1EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("NEXUS");
		ExtraModuleNames.Add("NEXUSEditor");
	}
}
