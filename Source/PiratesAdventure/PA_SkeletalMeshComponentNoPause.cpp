// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#include "PA_SkeletalMeshComponentNoPause.h"
#include "PiratesAdventure.h"
#include "Animation/AnimInstance.h"

bool UPA_SkeletalMeshComponentNoPause::ShouldTickPose() const 
{
	return true;
}

void UPA_SkeletalMeshComponentNoPause::ForceTickAnimation(float DeltaTime, bool bNeedsValidRootMotion, bool bDirectCall, bool bCallPose)
{
	if (bDirectCall)
	{
		if (GetAnimInstance())
			GetAnimInstance()->UpdateAnimation(DeltaTime, false);

		return;
	}

	if (bCallPose)
	{
		TickPose(DeltaTime, bNeedsValidRootMotion);
		return;
	}

	TickAnimation(DeltaTime, bNeedsValidRootMotion);
}