using UnrealBuildTool;
using System.Collections.Generic;

public class WyrdHandsBlockPuzzleTarget : TargetRules
{
	public WyrdHandsBlockPuzzleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("WyrdHandsBlockPuzzle");
	}
}
