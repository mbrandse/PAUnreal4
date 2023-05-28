// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
#include "PA_PlayerState_Push.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPushAnimationState : uint8
{
	STATE_IDLE UMETA(DisplayName = "Idle"), //no animation
	STATE_FORWARD UMETA(DisplayName = "Open"),
	STATE_BACKWARD UMETA(DisplayName = "Close")
};

UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Pushing"))
class PIRATESADVENTURE_API UPA_PlayerState_Push : public UPA_PlayerState_Base
{
	GENERATED_BODY()

public:

	UPA_PlayerState_Push();
	
	// Called when the game starts
	virtual void InitializeComponent() override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void MoveRight(float Value);
};
