// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimNode_StartCombo.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Interface_MeleeAttack.h"
#include "Animation/AnimInstanceProxy.h"

void FPA_AnimNode_StartCombo::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	BasePose.Initialize(Context);

	//if not evaluated, the pointer based inputs will remain NULL.
	//EvaluateGraphExposedInputs.Execute(Context);
	GetEvaluateGraphExposedInputs().Execute(Context);

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(Context.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		if (!Animation)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No animation sequence has been plugged into this StartCombo node"));
			return;
		}

		MeleeInterface->InitializeAttackSequence(Animation);
	}
}

void FPA_AnimNode_StartCombo::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	BasePose.Update(Context);
}

void FPA_AnimNode_StartCombo::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	BasePose.CacheBones(Context);
}

void FPA_AnimNode_StartCombo::Evaluate_AnyThread(FPoseContext& Output)
{
	BasePose.Evaluate(Output);
}