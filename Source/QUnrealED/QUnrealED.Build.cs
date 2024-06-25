using System;
using System.IO;
using UnrealBuildTool;

public class QUnrealED : ModuleRules
{
    public QUnrealED(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "UnrealEd"
            }
        );

        
        PublicIncludePaths.AddRange( new string[] {Path.Combine(ModuleDirectory, "qformats", "include")});

        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "AssetRegistry",
                "Slate",
                "SlateCore",
                "RawMesh",
                "QUnreal",
                "DeveloperSettings",
                
            }
        );
        
        CppStandard = CppStandardVersion.Cpp20;

    }
}