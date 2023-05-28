// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PiratesAdventureEditor : ModuleRules
{
    //public PiratesAdventureEditor(ReadOnlyTargetRules Target) : base(Target)
    public PiratesAdventureEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "PiratesAdventure",
                "Core",
                "CoreUObject",
                "Json",
                "Slate",
                "SlateCore",
                "EditorWidgets",
                "UnrealEd",
                "EditorStyle",
                "Engine",
                "InputCore",
                "AnimGraph",
                "AnimGraphRuntime",
                "BlueprintGraph",
                "LevelEditor",
                "KismetWidgets",
                "WorkspaceMenuStructure",
                "Projects",
                "GraphEditor"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
                "PiratesAdventure", 
                "UnrealED",
                "AnimGraph",
                "AnimGraphRuntime",
                "BlueprintGraph",
                "Engine",
                "PropertyEditor",
                "SlateCore"
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
                "MainFrame",
                "Settings"
            }
        );
    }
}
