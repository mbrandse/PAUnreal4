// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_Climbing.h"
#include "PiratesAdventure.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PA_ISMA_Ladder.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values for this component's properties
UPA_PlayerState_Climbing::UPA_PlayerState_Climbing()
{
	StateID = EPlayerStateType::GPS_GameClimb;
	StateCooldownDuration = 0.25f;
	DeadzoneNormalLow = 0.25f;
	DeadzoneNormalHigh = 0.9f;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UPA_PlayerState_Climbing::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called every frame
void UPA_PlayerState_Climbing::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPA_PlayerState_Climbing::BeginState()
{
	Super::BeginState();
}

void UPA_PlayerState_Climbing::EndState(EPlayerStateType NewState)
{
	Super::EndState(NewState);
}

void UPA_PlayerState_Climbing::MoveUp(float Value)
{
	float CorrectX, CorrectY;
	UPA_BlueprintFunctionLibrary::GetCorrectedInput(StateOwner->GetHorizontalMovement(), Value, DeadzoneNormalLow, DeadzoneNormalHigh, CorrectX, CorrectY);

	StateOwner->CorrectedHorizontalMovementSpeed = CorrectX;
	StateOwner->CorrectedVerticalMovementSpeed = CorrectY;
}