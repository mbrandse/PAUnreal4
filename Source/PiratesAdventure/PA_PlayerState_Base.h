// Fill out your copyright notice in the Description page of Project Settings.

// The base state is responsible for hosting all of the functions that can be absorbed by state components. 

#pragma once

#include "Components/ActorComponent.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_Base.generated.h"


UCLASS( ClassGroup=(PlayerState) )
class PIRATESADVENTURE_API UPA_PlayerState_Base : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPA_PlayerState_Base();

	// Necessary for our own initializations
	virtual void InitializeComponent() override;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	//STATE RELATED FUNCTIONALITY

	// Called every time the game switches to this state, to "open" the state.
	virtual void BeginState(); 
	
	// Called when the game wants to switch to a different state, to "close" the state. 
	virtual void EndState(EPlayerStateType NewState); 

	APiratesAdventureCharacter* StateOwner;

	EPlayerStateType StateID = EPlayerStateType::GPS_None;

	bool bIsDefaultState = false;

	//Reset State is required to drive the animation blueprint, so we can reset a state that has clear beginnings and endings.
	bool bResetState = false;

	//MOVEMENT RELATED FUNCTIONS

	virtual void MoveRight(float Value) {};
	virtual void MoveUp(float Value) {};

	virtual void AimToolRight(float Value) {};
	virtual void AimToolUp(float Value) {};

	virtual void PerformJump() {};
	virtual void Landed(const FHitResult& Hit) {};

	virtual void StartAttack() {};
	virtual void StopAttack() {};

	virtual void StartDefend() {};
	virtual void StopDefend() {};

	virtual void StartDodge() {};

	//virtual void StartCrouch() {};
	//virtual void StopCrouch() {};

	virtual void StartContext() {};
	virtual void StopContext() {};

	virtual void StartTool() {};
	virtual void StopTool() {};


	//SETTERS

	void SetOwner(APiratesAdventureCharacter* NewOwner) { StateOwner = NewOwner; };

	UFUNCTION(BlueprintPure, Category = "Player State")
		bool CanResetState() { return bResetState; };

	//STATE COOLDOWN FUNCTIONALITY; in case we need states to have a time until they can be used again.
	UPROPERTY()
		FTimerHandle StateCooldownHandle;

	UPROPERTY()
		float StateCooldownDuration;

	UFUNCTION()
		void StopStateCooldown();

	UFUNCTION()
		virtual bool CanEnterState();
};
