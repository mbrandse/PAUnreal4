// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_BrainComponent_Base.h"
#include "PiratesAdventure.h"

void UPA_BrainComponent_Base::ReceivePauseLogic()
{
	PauseLogic(TEXT("Pause Logic"));
}

void UPA_BrainComponent_Base::ReceiveResumeLogic()
{
	ResumeLogic(TEXT("Resume Logic"));
}
