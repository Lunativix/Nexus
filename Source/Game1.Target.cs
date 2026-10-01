using UnrealBuildTool;
using System.Collections.Generic;

public class Game1Target : TargetRules
{
	public Game1Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("NEXUS");
	}
}
