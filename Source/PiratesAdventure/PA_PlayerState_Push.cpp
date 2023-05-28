// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_Push.h"
#include "PiratesAdventure.h"

UPA_PlayerState_Push::UPA_PlayerState_Push()
{
	StateID = EPlayerStateType::GPS_GamePush;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPA_PlayerState_Push::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPA_PlayerState_Push::BeginState()
{

}

void UPA_PlayerState_Push::EndState(EPlayerStateType NewState)
{
	Super::EndState(NewState);
}

void UPA_PlayerState_Push::MoveRight(float Value)
{

}