// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
//#include "PA_ISMA_Ladderv2.h"
#include "PA_PlayerState_Climbing.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Climbing"))
class PIRATESADVENTURE_API UPA_PlayerState_Climbing : public UPA_PlayerState_Base
{
	GENERATED_BODY()
	
public:

	UPA_PlayerState_Climbing();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);
	
	virtual void MoveUp(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneNormalLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneNormalHigh;
};
