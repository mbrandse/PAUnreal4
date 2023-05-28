// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_Shimmy.h"
#include "PA_ISMA_SidleLedge.h"
#include "PiratesAdventure.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UPA_PlayerState_Shimmy::UPA_PlayerState_Shimmy()
{
	StateID = EPlayerStateType::GPS_GameShimmy;
	StateCooldownDuration = 0.25f;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	SidleSpeed = 5.f;
	RotationSpeed = 5.f;
}

// Called when the game starts
void UPA_PlayerState_Shimmy::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPA_PlayerState_Shimmy::BeginState()
{
	if (StateOwner)
	{
		CurrentTargetDirection = StateOwner->GetActorRotation();
	}

	//SetComponentTickEnabled(true);

	Super::BeginState();
}

void UPA_PlayerState_Shimmy::EndState(EPlayerStateType NewState)
{
	if (SidlePath)
		SidlePath = NULL;

	//SetComponentTickEnabled(false);

	Super::EndState(NewState);
}

void UPA_PlayerState_Shimmy::SetSidleData(class USplineComponent* NewSidlePath, TArray<FRotator> NewSidleDirections, int32 StartIndex, bool bSidleIsReversed)
{
	if (NewSidlePath)
	{
		SidlePath = NewSidlePath;
		SidleDirections = NewSidleDirections;
		CurrentTargetIndex = StartIndex;
		DistanceAlongPath = SidlePath->GetDistanceAlongSplineAtSplinePoint(StartIndex);
		bReverseInput = bSidleIsReversed;
	}
}

void UPA_PlayerState_Shimmy::RemainingDistance(float& FromEntryPointA, float& FromEntryPointB)
{
	if (SidlePath != NULL)
	{
		float MaxDistance = SidlePath->GetDistanceAlongSplineAtSplinePoint(SidlePath->GetNumberOfSplinePoints() - 1);

		FVector CharacterLoc = StateOwner->GetActorLocation();
		CharacterLoc += StateOwner->GetActorForwardVector() * StateOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
		CharacterLoc.Z -= StateOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FromEntryPointA = MaxDistance - FVector::Dist(CharacterLoc, SidlePath->GetLocationAtSplinePoint(SidlePath->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World));
		FromEntryPointB = MaxDistance - FVector::Dist(CharacterLoc, SidlePath->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));
	}
	else
	{
		FromEntryPointA = -1.f;
		FromEntryPointB = -1.f;
	}
}

//for the sake of rotation so we can have more complicated shimmy shapes
void UPA_PlayerState_Shimmy::MoveUp(float Value)
{

}

void UPA_PlayerState_Shimmy::MoveRight(float Value)
{
	//Start actually using the distance along spline for this one. Otherwise we still cannot turn the character correctly.
	if (SidlePath != NULL)
	{
		float InputModifier = bReverseInput ? -1.f : 1.f;

		float CorrectX, CorrectY;
		UPA_BlueprintFunctionLibrary::GetCorrectedInput(Value, StateOwner->GetVerticalMovement(), DeadzoneLow, DeadzoneHigh, CorrectX, CorrectY);
		StateOwner->CorrectedHorizontalMovementSpeed = CorrectX;
		StateOwner->CorrectedVerticalMovementSpeed = CorrectY;

		CorrectX *= InputModifier; //make sure to reverse the X AFTER you set the correctedHorizontalMovement, so the anim blueprint outputs the correct animation.

		if (CorrectX != 0)
		{
			if (CorrectX > 0)
			{
				DistanceAlongPath += (SidleSpeed * CorrectX);

				if (DistanceAlongPath > SidlePath->GetDistanceAlongSplineAtSplinePoint(SidlePath->GetNumberOfSplinePoints() - 1))
				{
					if (OnEndSidle.IsBound())
						OnEndSidle.Broadcast(SidlePath->GetNumberOfSplinePoints() - 1);
					return;
				}
				else if (DistanceAlongPath > SidlePath->GetDistanceAlongSplineAtSplinePoint(CurrentTargetIndex))
					CurrentTargetIndex++;

				FVector NewLocation = SidlePath->GetLocationAtDistanceAlongSpline(DistanceAlongPath, ESplineCoordinateSpace::World);
				NewLocation.Z = StateOwner->GetActorLocation().Z;
				StateOwner->SetActorLocation(NewLocation);

				//get the direction
				int32 DirectionIndex = FMath::Max(0, CurrentTargetIndex - 1);
				CurrentTargetDirection = SidleDirections.IsValidIndex(DirectionIndex) ? SidleDirections[DirectionIndex] : FRotator::ZeroRotator;
			}
			else
			{
				DistanceAlongPath += (SidleSpeed * CorrectX);

				if (DistanceAlongPath < SidlePath->GetDistanceAlongSplineAtSplinePoint(0))
				{
					if (OnEndSidle.IsBound())
						OnEndSidle.Broadcast(0);
					return;
				}
				else if (DistanceAlongPath < SidlePath->GetDistanceAlongSplineAtSplinePoint(CurrentTargetIndex - 1))
					CurrentTargetIndex--;

				FVector NewLocation = SidlePath->GetLocationAtDistanceAlongSpline(DistanceAlongPath, ESplineCoordinateSpace::World);
				NewLocation.Z = StateOwner->GetActorLocation().Z;
				StateOwner->SetActorLocation(NewLocation);

				//get the direction
				int32 DirectionIndex = FMath::Max(0, CurrentTargetIndex - 1);
				CurrentTargetDirection = SidleDirections.IsValidIndex(DirectionIndex) ? SidleDirections[DirectionIndex] : FRotator::ZeroRotator;
			}
		}

		UWorld* World = GetWorld();

		//The autorotation here may interfere. 
		if (StateOwner->Controller->GetControlRotation() != CurrentTargetDirection
			&& !StateOwner->bDisableAutoRotation
			&& World != NULL)
		{
			UE_LOG(AnyLog, Log, TEXT("Trying to rotate the character %s"), *(CurrentTargetDirection.ToString()));
			StateOwner->Controller->SetControlRotation(FMath::RInterpTo(StateOwner->GetActorRotation(), CurrentTargetDirection, World->GetDeltaSeconds(), RotationSpeed));
		}
	}
}