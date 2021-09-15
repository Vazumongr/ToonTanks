using UnrealBuildTool;
using System.IO;

public class AWSDynamoDB : ModuleRules
{
    public AWSDynamoDB(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Private/AWSDynamoDBPrivatePCH.h";

        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore", "Projects", "AWSBase", "BaseTPLibrary",  "DYNAMODBTPModule", });
        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

    }
}