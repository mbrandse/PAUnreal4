// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PA_AnimState_SetMatParameter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Animate Material Parameter"))
class PIRATESADVENTURE_API UPA_AnimState_SetMatParameter : public UAnimNotifyState
{
public:

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation", meta = (ToolTip = "The parameter collection from which to read the data"))
		UMaterialParameterCollection* MaterialParameterCollection;

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation", meta = (ToolTip = "The parameter collection from which to read the data"))
		FName ParameterName;

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation", meta = (ToolTip = "The parameter collection from which to read the data"))
		float StartValue;

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation", meta = (ToolTip = "The parameter collection from which to read the data"))
		float TargetValue;

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation")
		bool bEaseIn;

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation")
		bool bEaseOut;

	UPROPERTY(EditAnywhere, Category = "Material Parameter Animation", meta = (ToolTip = "Influences the intensity of the curve in case of ease in or out"))
		float CurveIntensity = 2.f;
	
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) override;

private:
	bool ValidateParameters(USkeletalMeshComponent* MeshComp);

	float AnimDuration = 0.f; //We need this to update the tick component correctly. If 0.f, ignore it.
	float StartTime = 0.f;
	float ElapsedTime = 0.f;

	bool bStartedState = false; //since we really only want a single call to NotifyBegin
	
};
