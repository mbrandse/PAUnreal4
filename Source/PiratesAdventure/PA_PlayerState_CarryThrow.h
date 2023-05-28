// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_Base.h"
#include "PA_PlayerState_CarryThrow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopCarrySignature, float, PlayerSpeed);

/**
 * 
 */
UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Carry and Throw"))
class PIRATESADVENTURE_API UPA_PlayerState_CarryThrow : public UPA_PlayerState_Base
{
	GENERATED_BODY()
	
public:

	UPA_PlayerState_CarryThrow();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void MoveRight(float Value);
	virtual void PerformJump();

	virtual void StartAttack();
	virtual void StartContext();

	bool bIsStoppingCarry; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Carry and Throw")
		FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Carry and Throw")
		float DesiredRotationTolerance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Carry and Throw")
		float RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Carry and Throw")
		float DeadzoneNormalLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Carry and Throw")
		float DeadzoneNormalHigh;

	UPROPERTY(BlueprintAssignable, Category = "Sidle")
		FStopCarrySignature OnStopCarry;

	UFUNCTION(BlueprintPure, Category = "Morgan Carry and Throw")
		bool HasReachedTargetRotation();
};
