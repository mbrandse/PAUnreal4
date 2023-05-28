// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimNode_TranslateRotateMesh.h"
#include "PiratesAdventure.h"
#include "Animation/AnimInstanceProxy.h"

FPA_AnimNode_TranslateRotateMesh::FPA_AnimNode_TranslateRotateMesh()
	: FAnimNode_Base()
	, Translation(FVector::ZeroVector)
	, InterpSpeed(1.f)
	, bUpdateX(false)// This is only to set
	, bUpdateY(false)// the defaults for the
	, bUpdateZ(true)// Axis pins (Can be changed in BP)
	, bUpdatePitch(false)
	, bUpdateYaw(false)
	, bUpdateRoll(false)
{
	BlendWeight = 1;
	StartRotation.ZeroRotator;
	TargetRotation.ZeroRotator;
	bTargetRotationIsSet = false;

	StartLocation.ZeroVector;
	TargetLocation.ZeroVector;
	bTargetLocationIsSet = false;
}

void FPA_AnimNode_TranslateRotateMesh::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	BasePose.Initialize(Context);
}

void FPA_AnimNode_TranslateRotateMesh::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	BasePose.Update(Context);

	//EvaluateGraphExposedInputs.Execute(Context);
	GetEvaluateGraphExposedInputs().Execute(Context);

	ACharacter* Character = Cast<ACharacter>(Context.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner());

	if (Character)
	{
		FVector NewTargetLocation = Character->GetMesh()->GetRelativeTransform().InverseTransformPosition(Translation);

		TargetLocation.X = bUpdateZ ? NewTargetLocation.X : Character->GetMesh()->RelativeLocation.X;
		TargetLocation.Y = bUpdateY ? NewTargetLocation.Y : Character->GetMesh()->RelativeLocation.Y;
		TargetLocation.Z = bUpdateZ ? NewTargetLocation.Z : Character->GetMesh()->RelativeLocation.Z;

		//then rotation
		TargetRotation.Pitch = bUpdatePitch ? Rotation.Pitch : Character->GetMesh()->RelativeRotation.Pitch;
		TargetRotation.Yaw = bUpdateYaw ? Rotation.Yaw : Character->GetMesh()->RelativeRotation.Yaw;
		TargetRotation.Roll = bUpdateRoll ? Rotation.Roll : Character->GetMesh()->RelativeRotation.Roll;

		Character->GetMesh()->SetWorldLocation(FMath::VInterpTo(Character->GetMesh()->GetComponentLocation(), Translation, Character->GetWorld()->GetDeltaSeconds(), InterpSpeed));
		Character->GetMesh()->SetWorldRotation(FMath::RInterpTo(Character->GetMesh()->GetComponentRotation(), Rotation, Character->GetWorld()->GetDeltaSeconds(), InterpSpeed));
	}
}

void FPA_AnimNode_TranslateRotateMesh::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	BasePose.CacheBones(Context);
}

void FPA_AnimNode_TranslateRotateMesh::Evaluate_AnyThread(FPoseContext& Output)
{
	BasePose.Evaluate(Output);
}