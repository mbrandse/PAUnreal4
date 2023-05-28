// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BrainComponent.h"
#include "PA_BrainComponent_Base.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_BrainComponent_Base : public UBrainComponent
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "AI|Logic", meta = (DisplayName = "Pause Logic"))
		void ReceivePauseLogic();

	UFUNCTION(BlueprintCallable, Category = "AI|Logic", meta = (DisplayName = "Resume Logic"))
		void ReceiveResumeLogic();
	
};
