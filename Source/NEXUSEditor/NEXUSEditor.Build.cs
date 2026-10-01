using UnrealBuildTool;

public class NEXUSEditor : ModuleRules
{
	public NEXUSEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"NEXUS",
			"UnrealEd"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AssetRegistry",
			"AssetTools",
			"NavigationSystem",
			"MaterialEditor"
		});

		PublicIncludePaths.Add(ModuleDirectory);
		PublicIncludePaths.Add(System.IO.Path.Combine(ModuleDirectory, "Public"));
	}
}
