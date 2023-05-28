// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PA_AnimState_SetMorphTarget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Animate Morph Target"))
class PIRATESADVENTURE_API UPA_AnimState_SetMorphTarget : public UAnimNotifyState
{
public:

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Morph Target", meta = (ToolTip = "The morph target which will be used"))
		FName MorphTargetName;

	UPROPERTY(EditAnywhere, Category = "Morph Target", meta = (ToolTip = "The starting value for the morph target"))
		float MorphStart;

	UPROPERTY(EditAnywhere, Category = "Morph Target", meta = (ToolTip = "The target value for the morph target"))
		float MorphEnd;

	UPROPERTY(EditAnywhere, Category = "Morph Target")
		bool bEaseIn;

	UPROPERTY(EditAnywhere, Category = "Morph Target")
		bool bEaseOut;

	UPROPERTY(EditAnywhere, Category = "Morph Target", meta = (ToolTip = "Influences the intensity of the curve in case of ease in or out"))
		float CurveIntensity = 2.f;

	//later add bools for implementing ease in and out animations

	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) override;

private:
	bool ValidateParameters(USkeletalMeshComponent* MeshComp);

	float MorphDuration = 0.f; //We need this to update the tick component correctly. If 0.f, ignore it.
	float StartTime = 0.f;
	float ElapsedTime = 0.f;

	bool bStartedState = false; //since we really only want a single call to NotifyBegin
};
