//// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
//
//#pragma once
//
//#include "Engine.h"
//#include "PiratesAdventure.h"
////#include "SlateBasics.h"
////#include "Runtime/UMG/Public/UMG.h"
////#include "Runtime/UMG/Public/UMGStyle.h"
////#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
////#include "Runtime/UMG/Public/IUMGModule.h"
////#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
//
////LOGGING CATEGORIES
//
//DECLARE_LOG_CATEGORY_EXTERN(EditorLog, Log, All);
//				   
////#endif

#pragma once

#include "Engine.h"
#include "PiratesAdventure.h"
//#include "ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(PAEditor, All, All)

class FPiratesAdventureEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};