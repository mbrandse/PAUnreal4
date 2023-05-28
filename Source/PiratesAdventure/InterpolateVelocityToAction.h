// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "object.h"
#include "PiratesAdventureCharacter.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"

#pragma once

/** Action that interpolates a component over time to a desired position */
class FInterpolateVelocityToAction : public FPendingLatentAction
{
public:
	/** Time over which interpolation should happen */
	float TotalTime;
	/** Time so far elapsed for the interpolation */
	float TimeElapsed;
	/** If we are currently interpolating. If false, update will complete */
	bool bInterpolating;

	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Component to interpolate */
	TWeakObjectPtr<APiratesAdventureCharacter> TargetCharacter;

	/** If we should modify velocity */
	bool bInterpVelocity;
	/** Location to interpolate from */
	FVector InitialVelocity;
	/** Location to interpolate to */
	FVector TargetVelocity;
	/** If the X axis should be ignored */
	bool bIgnoreX;
	/** If the Y axis should be ignored */
	bool bIgnoreY;
	/** If the Z axis should be ignored */
	bool bIgnoreZ;

	/** If we should modify rotation */
	bool bInterpRotation;
	/** Location to interpolate from */
	FRotator InitialRotation;
	/** Location to interpolate to */
	FRotator TargetRotation;

	/** Should we ease in (ie start slowly) during interpolation */
	bool bEaseIn;
	/** Should we east out (ie end slowly) during interpolation */
	bool bEaseOut;

	FInterpolateVelocityToAction(float Duration, const FLatentActionInfo& LatentInfo, APiratesAdventureCharacter* Character, bool bVelocity, bool bRotate, bool bInEaseOut, bool bInEaseIn)
		: TotalTime(Duration)
		, TimeElapsed(0.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, TargetCharacter(Character)
		, bInterpVelocity(bVelocity)
		, InitialVelocity(FVector::ZeroVector)
		, TargetVelocity(FVector::ZeroVector)
		, bIgnoreX(false)
		, bIgnoreY(false)
		, bIgnoreZ(false)
		, bInterpRotation(bRotate)
		, InitialRotation(FRotator::ZeroRotator)
		, TargetRotation(FRotator::ZeroRotator)
		, bEaseIn(bInEaseIn)
		, bEaseOut(bInEaseOut)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		// Update elapsed time
		TimeElapsed += Response.ElapsedTime();

		bool bComplete = (TimeElapsed >= TotalTime);

		// If we have a component to modify..
		if (TargetCharacter.IsValid() && bInterpolating)
		{
			// Work out 'Blend Percentage'
			const float BlendExp = 2.f;
			float DurationPct = TimeElapsed / TotalTime;
			float BlendPct;
			if (bEaseIn)
			{
				if (bEaseOut)
				{
					// EASE IN/OUT
					BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, BlendExp);
				}
				else
				{
					// EASE IN
					BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, BlendExp));
				}
			}
			else
			{
				if (bEaseOut)
				{
					// EASE OUT
					BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / BlendExp));
				}
				else
				{
					// LINEAR
					BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
				}
			}

			// Update velocity
			if (bInterpVelocity)
			{
				if (!bIgnoreX)
				{
					float NewVelocityX = bComplete ? TargetVelocity.X : FMath::Lerp(InitialVelocity.X, TargetVelocity.X, BlendPct);
					TargetCharacter->GetMovementComponent()->Velocity.X = NewVelocityX;
				}

				if (!bIgnoreY)
				{
					float NewVelocityY = bComplete ? TargetVelocity.Y : FMath::Lerp(InitialVelocity.Y, TargetVelocity.Y, BlendPct);
					TargetCharacter->GetMovementComponent()->Velocity.Y = NewVelocityY;
				}

				if (!bIgnoreZ)
				{
					float NewVelocityZ = bComplete ? TargetVelocity.Z : FMath::Lerp(InitialVelocity.Z, TargetVelocity.Z, BlendPct);
					TargetCharacter->GetMovementComponent()->Velocity.Z = NewVelocityZ;
				}
			}

			// Update rotation
			if (bInterpRotation)
			{
				FRotator NewRotation = bComplete ? TargetRotation : FMath::Lerp(InitialRotation, TargetRotation, BlendPct);
				TargetCharacter->SetActorRotation(NewRotation);
			}
		}

		Response.FinishAndTriggerIf(bComplete || !bInterpolating, ExecutionFunction, OutputLink, CallbackTarget);
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		static const FNumberFormattingOptions DelayTimeFormatOptions = FNumberFormattingOptions()
			.SetMinimumFractionalDigits(3)
			.SetMaximumFractionalDigits(3);
		return FText::Format(NSLOCTEXT("FInterpolateVelocityToAction", "ActionTimeFmt", "Move ({0} seconds left)"), FText::AsNumber(TotalTime - TimeElapsed, &DelayTimeFormatOptions)).ToString();
	}
#endif
};