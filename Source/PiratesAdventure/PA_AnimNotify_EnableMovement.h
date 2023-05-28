// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "PA_AnimNotify_EnableMovement.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Player Enable Movement"))
class PIRATESADVENTURE_API UPA_AnimNotify_EnableMovement : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);		
};
