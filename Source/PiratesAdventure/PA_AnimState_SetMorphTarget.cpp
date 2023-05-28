// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimState_SetMorphTarget.h"
#include "PiratesAdventure.h"

void UPA_AnimState_SetMorphTarget::NotifyBegin(USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration)
{
	// Only start morphing if we've got valid params
	if (ValidateParameters(MeshComp))
	{
		MorphDuration = TotalDuration;
		ElapsedTime = 0.f;

		Received_NotifyBegin(MeshComp, Animation, TotalDuration);
	}
}

void UPA_AnimState_SetMorphTarget::NotifyTick(USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (ValidateParameters(MeshComp)
		&& Animation
		&& !MorphTargetName.IsNone())
	{	
		ElapsedTime += FrameDeltaTime * (1.f / (MorphDuration / Animation->RateScale));
		float DurationPct = FMath::Clamp(ElapsedTime, 0.f, 1.f);
		float BlendPct;

		if (bEaseIn)
		{
			if (bEaseOut)
			{
				// EASE IN/OUT
				BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, CurveIntensity);
			}
			else
			{
				// EASE IN
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, CurveIntensity));
			}
		}
		else
		{
			if (bEaseOut)
			{
				// EASE OUT
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / CurveIntensity));
			}
			else
			{
				// LINEAR
				BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
			}
		}

		//clamp BlendPCT just in case
		BlendPct = FMath::Clamp(BlendPct, 0.f, 1.f);
		float MorphValue = FMath::Lerp(MorphStart, MorphEnd, BlendPct);
		MeshComp->SetMorphTarget(MorphTargetName, MorphValue); //testing; replace with animation later
	}

	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UPA_AnimState_SetMorphTarget::NotifyEnd(USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation)
{
	MorphDuration = 0.f;
	bStartedState = false;

	Received_NotifyEnd(MeshComp, Animation);
}

bool UPA_AnimState_SetMorphTarget::ValidateParameters(USkeletalMeshComponent* MeshComp)
{
	bool bValid = true;

	//if (MeshComp)
	//{
	//	if (MeshComp->MorphTargetCurves.Find(MorphTargetName))
	//	{
	//		bValid = true;
	//	}
	//}

	//check for valid morph target

	return bValid;
}

