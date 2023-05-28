// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNodeBase.h"
#include "PA_AnimNode_LaunchCharacter.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct PIRATESADVENTURE_API FPA_AnimNode_LaunchCharacter : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
		FPoseLink BasePose;

	/** The speed at which the character should be launched **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinShownByDefault))
		FVector LaunchSpeed;

	/** If true, the launchspeed X and Y will overwrite the X and Y velocity. Otherwise it adds to it. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bOverrideXY;

	/** If true, the launchspeed Z will overwrite the Z velocity. Otherwise it adds to it. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
		bool bOverrideZ;

public:
	FPA_AnimNode_LaunchCharacter();
public:
	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)  override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	// End of FAnimNode_RCAnimNode interface
};
