// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_Movement_AxisRestraint.h"
#include "PiratesAdventure.h"

// Sets default values for this component's properties
UPA_Movement_AxisRestraint::UPA_Movement_AxisRestraint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPA_Movement_AxisRestraint::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		ConstraintLocation = GetOwner()->GetActorLocation();
	}
	else
		UE_LOG(AnyLog, Warning, TEXT("CONSTRAIN TO AXIS COMPONENT DOESN'T HAVE AN OWNER"));
}

void UPA_Movement_AxisRestraint::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!bConstraintReleased)
	{
		if (bConstrainToX && GetOwner()->GetActorLocation().X != ConstraintLocation.X
			|| bConstrainToY && GetOwner()->GetActorLocation().Y != ConstraintLocation.Y
			|| bConstrainToZ && GetOwner()->GetActorLocation().Z != ConstraintLocation.Z)
		{
			FVector OwnerLoc = GetOwner()->GetActorLocation();
			FVector NewLoc;

			NewLoc.X = bConstrainToX ? ConstraintLocation.X : OwnerLoc.X;
			NewLoc.Y = bConstrainToY ? ConstraintLocation.Y : OwnerLoc.Y;
			NewLoc.Z = bConstrainToZ ? ConstraintLocation.Z : OwnerLoc.Z;
			
			if (InterpolationSpeed == 0.f)
			{
				GetOwner()->SetActorLocation(NewLoc);
			}
			else
			{
				GetOwner()->SetActorLocation(FMath::VInterpTo(GetOwner()->GetActorLocation(), NewLoc, DeltaTime, InterpolationSpeed));
			}
		}
	}
}

void UPA_Movement_AxisRestraint::ConstrainToXAxis(float XConstraint)
{
	ConstraintLocation.X = XConstraint != 0.f ? XConstraint : GetOwner()->GetActorLocation().X;

	bConstrainToX = true;
}

void UPA_Movement_AxisRestraint::ConstrainToYAxis(float YConstraint)
{
	ConstraintLocation.Y = YConstraint != 0.f ? YConstraint : GetOwner()->GetActorLocation().Y;

	bConstrainToY = true;
}

void UPA_Movement_AxisRestraint::ConstrainToZAxis(float ZConstraint)
{
	ConstraintLocation.Z = ZConstraint != 0.f ? ZConstraint : GetOwner()->GetActorLocation().Z;

	bConstrainToY = true;
}

void UPA_Movement_AxisRestraint::ReleaseConstraint()
{
	bConstraintReleased = true;
}

void UPA_Movement_AxisRestraint::ReinstateConstraint(bool bUpdatePosition)
{
	if (GetOwner() && bUpdatePosition)
	{
		ConstraintLocation = GetOwner()->GetActorLocation();
	}

	bConstraintReleased = false;
}

void UPA_Movement_AxisRestraint::DeleteConstraint()
{
	bConstrainToX = false;
	bConstrainToY = false;
	bConstrainToZ = false;

	ConstraintLocation = FVector::ZeroVector;
}