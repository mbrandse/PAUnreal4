// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_Interface_ContextAction.generated.h"

/**
 * Interface that handles all elemental effect related functionality. Can be shared on either enemies or objects. 
 * Presence of this interface will allow us to do specific functionality without having to create a separate class for it. We can just use a blueprint class and slap this interface on it.
 */
UINTERFACE(BlueprintType)
class PIRATESADVENTURE_API UPA_Interface_ContextAction : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class PIRATESADVENTURE_API IPA_Interface_ContextAction
{
	GENERATED_IINTERFACE_BODY()

public:
	//context functions
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer) {};

	virtual void StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer) {};
};