// Fill out your copyright notice in the Description page of Project Settings.

#include "PiratesAdventure.h"
#include "PA_SlateStyles.h"
#include "Modules/ModuleManager.h"

class FPiratesAdventureGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(FPA_SlateStyles::GetStyleSetName());
		FPA_SlateStyles::Initialize();

		AddShaderSourceDirectoryMapping(TEXT("/Project"), FPaths::Combine(*(FPaths::ProjectDir()), TEXT("Shaders")));
	}

	virtual void ShutdownModule() override
	{
		FPA_SlateStyles::Shutdown();
	}

};

IMPLEMENT_PRIMARY_GAME_MODULE(FPiratesAdventureGameModule, PiratesAdventure, "PiratesAdventure");

DEFINE_LOG_CATEGORY(AnyLog);
DEFINE_LOG_CATEGORY(InitLog);
DEFINE_LOG_CATEGORY(AILog);