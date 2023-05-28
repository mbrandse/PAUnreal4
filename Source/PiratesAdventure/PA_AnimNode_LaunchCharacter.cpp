// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimNode_LaunchCharacter.h"
#include "PiratesAdventure.h"
//#include "BonePose.h"
//#include "Private/EnginePrivate.h"
#include "Animation/AnimInstanceProxy.h"
#include "PA_Interface_MeleeAttack.h"

FPA_AnimNode_LaunchCharacter::FPA_AnimNode_LaunchCharacter()
	: FAnimNode_Base()
	, LaunchSpeed(FVector::ZeroVector)
	, bOverrideXY(false)
	, bOverrideZ(false)
{

}

void FPA_AnimNode_LaunchCharacter::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	BasePose.Initialize(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	ACharacter* Character = Cast<ACharacter>(Context.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner());

	if (Character)
	{
		Character->LaunchCharacter(LaunchSpeed, bOverrideXY, bOverrideZ);
	}
}

void FPA_AnimNode_LaunchCharacter::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	BasePose.Update(Context);
}

void FPA_AnimNode_LaunchCharacter::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	BasePose.CacheBones(Context);
}

void FPA_AnimNode_LaunchCharacter::Evaluate_AnyThread(FPoseContext& Output)
{
	BasePose.Evaluate(Output);
}