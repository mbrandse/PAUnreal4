// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "PA_AnimNotify_PlayerStopDamage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Player Damage Stop"))
class PIRATESADVENTURE_API UPA_AnimNotify_PlayerStopDamage : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
	
};
