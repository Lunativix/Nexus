using UnrealBuildTool;

public class NEXUS : ModuleRules
{
	public NEXUS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore",
			"DeveloperSettings"
		});

		PublicIncludePaths.Add(ModuleDirectory);
		PublicIncludePaths.Add(System.IO.Path.Combine(ModuleDirectory, "Public"));
	}
}
