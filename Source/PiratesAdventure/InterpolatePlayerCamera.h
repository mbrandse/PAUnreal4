// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "object.h"
#include "PA_PlayerSpringArmComponent.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "PiratesAdventure.h"

#pragma once

/** Action that observes the state of the spring arm component. Is not time dependent */
class FInterpolatePlayerCamera : public FPendingLatentAction
{

public:
	/** Since player camera movement doesn't depend on time, we need to have a tolerance at which point we can consider this latent function to be finished. */
	float Tolerance;

	/** If we are currently interpolating. If false, update will complete */
	bool bInterpolating;

	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Spring arm component to check for interpolation */
	TWeakObjectPtr<UPA_PlayerSpringArmComponent> TargetSpringArmComponent;

	/** Target location to move to */
	FVector TargetLocation;
	/** Target rotation to rotate to */
	FRotator TargetRotation;
	/** target arm length */
	float TargetArmLength;

	/** Whether this latent action is checking for location */
	bool bInterpolateLocation;
	/** Whether this latent action is checking for rotation  */
	bool bInterpolateRotation;
	/** Whether this latent action is checking for arm length  */
	bool bInterpolateArmLength;

	/** Whether to use target location as the means to consider this latent action finished */
	bool bUseTargetLocation;
	/** Whether to use target rotation as the means to consider this latent action finished */
	bool bUseTargetRotation;
	/** Whether to use target arm length as the means to consider this latent action finished */
	bool bUseTargetArmLength;

	FInterpolatePlayerCamera(const FLatentActionInfo& LatentInfo, UPA_PlayerSpringArmComponent* SpringArmComponent, FVector Location, FRotator Rotation, float ArmLength, bool bTargetLocation, bool bTargetRotation, bool bTargetArmLength)
		: Tolerance(1.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, TargetSpringArmComponent(SpringArmComponent)
		, TargetLocation(Location)
		, TargetRotation(Rotation)
		, TargetArmLength(ArmLength)
		, bUseTargetLocation(bTargetLocation)
		, bUseTargetRotation(bTargetRotation)
		, bUseTargetArmLength(bTargetArmLength)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (TargetSpringArmComponent.IsValid())
		{
			// Update elapsed time
			UPA_PlayerSpringArmComponent* SpringArmComponent = TargetSpringArmComponent.Get();
			if (SpringArmComponent)
			{
				//assume initial interpolation is true
				bInterpolating = true;
		
				//first make sure we check manual translation, as that one can also be dependant on player location
				if (bInterpolateLocation)
				{
					if (SpringArmComponent->bManualCameraTranslation)
					{
						if (SpringArmComponent->ManualCameraTranslationTarget != TargetLocation) bInterpolating = false;
						//if manual translation is not active, don't do anything, as owner location is variable.
					}
				}
				else if (bInterpolateRotation && SpringArmComponent->GetDesiredRotation() != TargetRotation
					|| bInterpolateArmLength && SpringArmComponent->DesiredTargetArmLength != TargetArmLength)
				{
					//then see whether something changed any of the spring arm targets, and reset the latent action if so.
					bInterpolating = false;
				}

				//when targets are not invalid, see if we have completed out action
				if (bUseTargetLocation)
				{
					if (SpringArmComponent->bManualCameraTranslation)
					{
						if (SpringArmComponent->PreviousDesiredLoc.Equals(TargetLocation, Tolerance)) bInterpolating = false;
					}
					else
					{
						if (SpringArmComponent->PreviousArmOrigin.Equals(SpringArmComponent->GetOwner()->GetActorLocation(), Tolerance)) bInterpolating = false;
					}
				}
				else if (bUseTargetRotation && SpringArmComponent->GetDesiredRotation().Equals(TargetRotation, Tolerance)
					|| bUseTargetArmLength && SpringArmComponent->DesiredTargetArmLength < Tolerance)
				{
					bInterpolating = false;
				}

				Response.FinishAndTriggerIf(!bInterpolating, ExecutionFunction, OutputLink, CallbackTarget);
			}
		}
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		return TEXT("FInterpolatePlayerCamera");
	}
#endif
};