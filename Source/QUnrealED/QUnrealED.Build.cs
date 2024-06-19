using System;
using System.IO;
using UnrealBuildTool;

public class QUnrealED : ModuleRules
{
    public QUnrealED(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "UnrealEd",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AssetTools",
                "QUnreal",
                "UnrealEd",
                "DeveloperSettings"
            }
        );
        

    }
}