// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "PA_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UPA_CharacterMovementComponent();

	virtual void InitializeComponent() override;

	/** Perform movement on an autonomous client */
	virtual void PerformMovement(float DeltaSeconds) override;

	void Duck();

	void UnDuck();

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void UnCrouch(bool bClientSimulation = false); //we need some custom functionality for the event where the character goes from crouch to duck

	bool IsDucking() const;

	bool bWantsToDuck:1;

	bool bWantsToDuckFromCrouch:1;

	/** Collision half-height when crouching (component scale is applied separately) */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
		float DuckedHalfHeight;
	
	
};
