// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNodeBase.h"
#include "PA_AnimNode_TranslateRotateMesh.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct PIRATESADVENTURE_API FPA_AnimNode_TranslateRotateMesh : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
		FPoseLink BasePose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinShownByDefault))
		FVector Translation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinShownByDefault))
		FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bUpdateX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bUpdateY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bUpdateZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bUpdatePitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bUpdateYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bUpdateRoll;

	/** Controls how much of the translation is blended to the target, default is 1 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletalControl, meta = (PinShownByDefault))
		float BlendWeight;

public:
	FPA_AnimNode_TranslateRotateMesh();
public:
	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)  override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;

private:
	FVector StartLocation;
	FVector TargetLocation;
	bool bTargetRotationIsSet;

	FRotator StartRotation;
	FRotator TargetRotation;
	bool bTargetLocationIsSet;
};
