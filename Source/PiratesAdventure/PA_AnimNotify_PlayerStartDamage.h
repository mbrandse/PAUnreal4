// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "PA_AnimNotify_PlayerStartDamage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Player Damage Start"))
class PIRATESADVENTURE_API UPA_AnimNotify_PlayerStartDamage : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);	
	
};
