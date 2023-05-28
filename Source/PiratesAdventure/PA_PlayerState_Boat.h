// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
#include "PiratesAdventureBoat.h"
#include "PA_Pawn_Boat.h"
#include "PA_PlayerState_Boat.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Boat"))
class PIRATESADVENTURE_API UPA_PlayerState_Boat : public UPA_PlayerState_Base
{
	GENERATED_BODY()
	
public:

	UPA_PlayerState_Boat();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void MoveRight(float Value);

	UPROPERTY()
		//APiratesAdventureBoat* BoatInstance;
		APA_Pawn_Boat* BoatInstance;

	UFUNCTION(BlueprintPure, Category = "Player State Boat")
		APA_Pawn_Boat* GetBoatInstance() { return BoatInstance; };
		//APiratesAdventureBoat* GetBoatInstance() { return BoatInstance; };

	UFUNCTION(BlueprintCallable, Category = "Player State Boat")
		void SetBoatInstance(/*APiratesAdventureBoat* NewBoatInstance*/APA_Pawn_Boat* NewBoatInstance) { BoatInstance = NewBoatInstance; };
	
};
