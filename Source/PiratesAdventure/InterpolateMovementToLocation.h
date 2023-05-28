// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "object.h"
#include "GameFramework/Character.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "PiratesAdventure.h"

#pragma once

/** Action that uses the movement component of a character to smoothly move the character to a location using its speed rather than setting its location. */
class FInterpolateMovementToLocation : public FPendingLatentAction
{
public:
	/** Time over which movement should happen; is calculated based on the distance to the target combined with the character's movement speed */
	float TotalTime;
	/** Time so far elapsed for the interpolation */
	float TimeElapsed;

	/** Time to ensure we don't get stuck with zero velocity */
	float FailsafeTotalTime;
	float FailsafeTimeElapsed;

	/** If we are currently interpolating. If false, update will complete */
	bool bInterpolating;

	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Character to interpolate */
	TWeakObjectPtr<ACharacter> TargetCharacter;

	/** Target location to move to */
	FVector TargetLocation;
	/** Tolerance for how much the character has to approach the target location. */
	float TargetTolerance;
	/** Max allowed speed of the character */
	float TargetMaxMovementSpeed;
	/** Used to reset the movement speed of a character back to what it was before this latent action was called */
	float TargetOldMovementSpeed;

	/** If we should modify character rotation when the movement has finished */
	bool bUpdateCharacterFacing;
	/** Rotation to interpolate to */
	FRotator TargetFacing;
	/** The speed of the rotation */
	float TargetFacingInterpolationDuration;

	///** Should we ease in (ie start slowly) during character rotation */
	//bool bCharacterFacingEaseIn;
	///** Should we east out (ie end slowly) during character rotation */
	//bool bCharacterFacingEaseOut;

	FInterpolateMovementToLocation(const FLatentActionInfo& LatentInfo, ACharacter* Character, FVector Location, float MaxTargetSpeed, float CurrentTargetSpeed, bool bRotateFacing, FRotator RotateFacingTarget = FRotator::ZeroRotator, float RotateFacingDuration = 0.2f)
		: TimeElapsed(0.f)
		, FailsafeTotalTime(0.25f)
		, FailsafeTimeElapsed(0.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, TargetCharacter(Character)
		, TargetLocation(Location)
		, TargetTolerance(1.f)
		, TargetMaxMovementSpeed(MaxTargetSpeed)
		, TargetOldMovementSpeed(CurrentTargetSpeed)
		, bUpdateCharacterFacing(bRotateFacing)
		, TargetFacing(RotateFacingTarget)
		, TargetFacingInterpolationDuration(RotateFacingDuration)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (TargetCharacter.IsValid())
		{
			// Update elapsed time
			ACharacter* Character = TargetCharacter.Get();
			if (Character)
			{
				FVector CurrentCharacterLoc = Character->GetActorLocation();
				if (Character->GetCharacterMovement()->MaxWalkSpeed != TargetMaxMovementSpeed)
				{
					TargetOldMovementSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
					Character->GetCharacterMovement()->MaxWalkSpeed = TargetMaxMovementSpeed;
				}

				//TimeElapsed += Response.ElapsedTime();
				float RemDistance = FVector2D::Distance(FVector2D(CurrentCharacterLoc.X, CurrentCharacterLoc.Y), FVector2D(TargetLocation.X, TargetLocation.Y));
				//if (TotalTime == 0.f) TotalTime = FVector::Dist(Character->GetActorLocation(), TargetLocation) / TargetMaxMovementSpeed /*Character->GetCharacterMovement()->MaxWalkSpeed*/;

				UE_LOG(AnyLog, Log, TEXT("Remaining Distance = %f"), RemDistance);

				//bool bComplete = (TimeElapsed >= TotalTime);
				bool bComplete = RemDistance <= TargetTolerance;
				bool bRotationComplete = !bUpdateCharacterFacing;

				// If we have a component to modify..
				if (bInterpolating)
				{
					//first see if our timing has concluded and whether we need to rotate after that. If so, rotate first.
					if (bComplete && !bRotationComplete)
					{
						FRotator CurrentRot = Character->GetActorRotation();
						if (TimeElapsed < TargetFacingInterpolationDuration)
						{
							TimeElapsed += Response.ElapsedTime();

							float DurationPercent = TimeElapsed / TargetFacingInterpolationDuration;
							FRotator NewRot = FMath::Lerp(CurrentRot, TargetFacing, DurationPercent);
							Character->SetActorRotation(NewRot);
						}
						else
						{
							bRotationComplete = true;
						}
					}
					else if(!bComplete)
					{
						//The process the actual movement.

						//first calculate the direction based on the current location and the target
						FVector CurrentLocation = Character->GetActorLocation();
						FVector TargetDirection = TargetLocation - CurrentLocation;
						TargetDirection.Normalize();

						FRotator TargetRotation, CurrentRotation = Character->GetActorRotation();
						TargetRotation.Yaw = TargetDirection.Rotation().Yaw;
						if (TargetRotation != CurrentRotation)
						{
							Character->SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, Character->GetWorld()->GetDeltaSeconds(), 1.f));
						}

						Character->AddMovementInput(TargetDirection, 1.f);
					}
				}

				//finally do a check, in case our character is stuck against a wall or something.
				UE_LOG(AnyLog, Log, TEXT("CURRENT VELOCITY IS %f"), Character->GetCharacterMovement()->Velocity.Size());
				if (Character->GetCharacterMovement()->Velocity.Size() <= 10.f) //a velocity of 10 is already very low, and on hit it won't ever become truly zero. 
				{
					FailsafeTimeElapsed += Response.ElapsedTime();
					UE_LOG(AnyLog, Log, TEXT("VELOCITY IS 0, COUNTDOWN TO FAILSAFE %f"), FailsafeTimeElapsed);
					if (FailsafeTimeElapsed >= FailsafeTotalTime)
					{
						UE_LOG(AnyLog, Log, TEXT("ACTIVATE FAILSAFE"));
						bInterpolating = false;
					}
				}
				else
				{
					FailsafeTimeElapsed = 0.f;
				}

				if ((bComplete && bRotationComplete) || !bInterpolating)
				{
					Character->GetCharacterMovement()->MaxWalkSpeed = TargetOldMovementSpeed;
				}

				Response.FinishAndTriggerIf((bComplete && bRotationComplete) || !bInterpolating, ExecutionFunction, OutputLink, CallbackTarget);
			}
		}
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		static const FNumberFormattingOptions DelayTimeFormatOptions = FNumberFormattingOptions()
			.SetMinimumFractionalDigits(3)
			.SetMaximumFractionalDigits(3);
		return FText::Format(NSLOCTEXT("FInterpolateMovementToAction", "ActionTimeFmt", "Move ({0} seconds left)"), FText::AsNumber(TotalTime - TimeElapsed, &DelayTimeFormatOptions)).ToString();
	}
#endif
};