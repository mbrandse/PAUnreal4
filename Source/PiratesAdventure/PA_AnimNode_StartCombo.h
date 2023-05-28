// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Animation/AnimNodeBase.h"
#include "PA_AnimNode_StartCombo.generated.h"

USTRUCT(Blueprintable)
struct PIRATESADVENTURE_API FPA_AnimNode_StartCombo : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
		FPoseLink BasePose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Timing", meta = (PinShownByDefault))
		UAnimSequenceBase* Animation;

public:

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
};
