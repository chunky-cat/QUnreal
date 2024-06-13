// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class QUnreal : ModuleRules
{
	private string ThirdPartyPath()
	{
		return Path.Combine(ModuleDirectory, "..", "..", "ThirdParty");
	}

	private string IncludePath()
	{
		return Path.Combine(ThirdPartyPath(), "include");
	}
    
	private string GetOSLibPath()
	{
		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			return Path.Combine(ThirdPartyPath(), "Libs", "Mac");
		}
		return Path.Combine(ThirdPartyPath(), "Libs", "Win");
	}
	
	public QUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RawMesh",
				"UMG", 
				"UnrealEd"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		PublicIncludePaths.AddRange( new string[] {Path.Combine(ModuleDirectory, "qformats", "include")});

		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
