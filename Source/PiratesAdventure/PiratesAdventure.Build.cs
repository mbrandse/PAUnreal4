// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class PiratesAdventure : ModuleRules
{
	public PiratesAdventure(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem",
            //"RamaSaveSystem" ,
            "RamaMeleePlugin",
            "DialoguePlugin",
            "StreamingFile",
            "CinematicCamera",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "RHI",
            "RenderCore",
            //"ShaderCore", //has been moved under RenderCore as of 4.22
            "ImageWrapper",
            //"RamaSaveSystem",
            "CinematicCamera",
        });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
