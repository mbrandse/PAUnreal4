// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_AI_ControllerBase.h"
#include "PA_AI_ControllerHumanBase.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_AI_ControllerHumanBase : public APA_AI_ControllerBase
{
	GENERATED_BODY()
	
public:

	APA_AI_ControllerHumanBase(const FObjectInitializer& ObjectInitializer);

	//UFUNCTION(BlueprintPure, Category = "Movement")
	//	EJumpAction GetCurrentJumpAction();
	
};
