// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState_Trail.h"
#include "PA_AnimState_MorganSwordTrail.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Morgan Sword Trail"))
class PIRATESADVENTURE_API UPA_AnimState_MorganSwordTrail : public UAnimNotifyState_Trail
{
	GENERATED_BODY()

	virtual UParticleSystem* GetOverridenPSTemplate(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Trail")
		UParticleSystem* PSDefaultTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Trail")
		UParticleSystem* PSFireTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Trail")
		UParticleSystem* PSIceTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Trail")
		UParticleSystem* PSWindTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Trail")
		UParticleSystem* PSWoodTemplate;
	
	
};
