// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "PiratesAdventureGameMode.generated.h"

UCLASS(minimalapi)
class APiratesAdventureGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APiratesAdventureGameMode();

	//! Allow the blueprint to determine whether we are running with the editor or not
	UFUNCTION(BlueprintPure, Category = GameMode)
		bool IsWithEditor() const
	{
#if WITH_EDITOR
		return true;
#else
		return false;
#endif
	}
};



