// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_Interface_MeleeAttack.generated.h"

/**
 * Interface that handles basic melee functionality for use in conjunction with Animation Blueprints. 
 * Centralizes melee functionality, so we don't have to program it every time when we have an actor with melee attacks.
 */
UINTERFACE(BlueprintType)
class PIRATESADVENTURE_API UPA_Interface_MeleeAttack : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class PIRATESADVENTURE_API IPA_Interface_MeleeAttack
{
	GENERATED_IINTERFACE_BODY()

public:

	//for now, let's experiment with this. 
	UFUNCTION()
		virtual void InitializeAttackSequence(UAnimSequenceBase* Attack) {};

	UFUNCTION()
		virtual bool IsAttackPlaying() { return false; };

	UFUNCTION()
		virtual int32 GetComboCount() { return INDEX_NONE; };

	UFUNCTION()
		virtual bool CanContinueComboAttack() { return false; };

	UFUNCTION()
		virtual float GetAttackRelevantTimeRemaining() { return 0.f; };

	UFUNCTION()
		virtual float GetAttackRelevantTimeRemainingRatio() { return 0.f; };

	//This takes care of resetting any lingering attack variables (not including charge attacks)
	UFUNCTION()
		virtual void ResetNormalAttack() {};

	UFUNCTION()
		virtual void ResetChargeAttack() {};

	UFUNCTION()
		virtual bool CanContinueChargeAttack() { return false; };

	UFUNCTION()
		virtual int32 GetActorDirection() { return 1; };
};