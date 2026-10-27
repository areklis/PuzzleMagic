using UnrealBuildTool;

public class WyrdHandsBlockPuzzle : ModuleRules
{
	public WyrdHandsBlockPuzzle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SDL3",
			"ProceduralMeshComponent",
			"RealtimeMeshComponent",
			"MeshOptimizer",
			"AudioMixer",
			"Synthesis",
			"Json"
		});

		// The demo build: PUZZLE_DEMO=1 in the environment of the build (Tools/build_packages.ps1 -DemoApk sets it) compiles the game as
		// the demo. A phone has no loose marker file like the Windows demo's DemoBuild.txt (see PuzzleDemo.h).
		bool bDemo = System.Environment.GetEnvironmentVariable("PUZZLE_DEMO") == "1";
		if (bDemo)
		{
			PublicDefinitions.Add("PUZZLE_DEMO_BUILD=1");
		}
		// PUZZLE_LEAN_ANDROID=1 (-ReleaseApk) gives the full game the same lean manifest without making it the demo.
		if ((bDemo || System.Environment.GetEnvironmentVariable("PUZZLE_LEAN_ANDROID") == "1") && Target.Platform == UnrealTargetPlatform.Android)
		{
			// The APK asks for no network or purchase permissions (the engine adds them to every manifest; the game uses none).
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", System.IO.Path.Combine(ModuleDirectory, "PuzzleDemo_UPL.xml"));
		}

		// FastNoise2 is prebuilt for Win64 only (an iOS build of it needs a Mac); elsewhere the
		// game falls back to FMath::PerlinNoise.
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.Add("FastNoise2");
			PublicDefinitions.Add("WITH_FASTNOISE2=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_FASTNOISE2=0");
		}
	}
}
