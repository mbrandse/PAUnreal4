// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_AI_CharacterBase.h"
//#include "PA_DamageType_Base.h"
#include "PA_AI_CharacterHumanBase.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	STATE_NORMAL UMETA(DisplayName = "Normal"), 
	STATE_ATTACK UMETA(DisplayName = "Attack"),
	STATE_JUMP UMETA(DisplayName = "Jump"),
	STATE_FALL UMETA(DisplayName = "Fall"),
	STATE_DAMAGE UMETA(DisplayName = "Damage"),
	STATE_DEAD UMETA(DisplayName = "Dead"),
};

UCLASS()
class PIRATESADVENTURE_API APA_AI_CharacterHumanBase : public APA_AI_CharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APA_AI_CharacterHumanBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to make the pawn resume the pathing again
	virtual void Landed(const FHitResult& Hit);

	//AI state is the state in which the AI is, so we can make calls based on what the AI is doing at the moment.
	UPROPERTY()
		EAIState AIState; 

	UFUNCTION(BlueprintCallable, Category = "AI")
		void SetAIState(EAIState NewState) { AIState = NewState; };

	UFUNCTION(BlueprintPure, Category = "AI")
		EAIState GetAIState() { return AIState; };
};