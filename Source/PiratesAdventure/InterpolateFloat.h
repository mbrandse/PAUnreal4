// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "object.h"
#include "GameFramework/Actor.h"
#include "Engine/LatentActionManager.h"
#include "Curves/CurveFloat.h"
#include "Misc/OutputDeviceNull.h"
#include "LatentActions.h"

#pragma once

/** Action that interpolates a component over time to a desired position */
class FInterpolateFloat: public FPendingLatentAction
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
	/** Function to execute on update (needs to have a float parameter) */
	FName UpdateFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Weapon to animate */
	TWeakObjectPtr<UObject> TargetObject;

	/** Location to interpolate from */
	float InitialValue;
	/** Location to interpolate to */
	float TargetValue;

	/** Should we ease in (ie start slowly) during interpolation */
	bool bEaseIn;
	/** Should we east out (ie end slowly) during interpolation */
	bool bEaseOut;

	FInterpolateFloat(float Duration, const FLatentActionInfo& LatentInfo, UObject* Object)
		: TotalTime(Duration)
		, TimeElapsed(0.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, TargetObject(Object)
		, InitialValue(0.0)
		, TargetValue(0.0)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		// Update elapsed time
		TimeElapsed += Response.ElapsedTime();

		bool bComplete = (TimeElapsed >= TotalTime);

		// If we have a component to modify..
		if (TargetObject.IsValid() && bInterpolating)
		{
			UObject* Object = TargetObject.Get();

			if (Object)
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

				float NewValue = bComplete ? TargetValue : FMath::Lerp(InitialValue, TargetValue, BlendPct);
				FOutputDeviceNull ar;
				const FString command = FString::Printf(TEXT("%s %f"), *(UpdateFunction.ToString()), NewValue);
				Object->CallFunctionByNameWithArguments(*command, ar, NULL, true);
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
		return FText::Format(NSLOCTEXT("FScaleActiveSkillSphere", "ActionTimeFmt", "Move ({0} seconds left)"), FText::AsNumber(TotalTime - TimeElapsed, &DelayTimeFormatOptions)).ToString();
	}
#endif
};