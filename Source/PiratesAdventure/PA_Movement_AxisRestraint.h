// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "PA_Movement_AxisRestraint.generated.h"


UCLASS(ClassGroup = (Movement), meta = (BlueprintSpawnableComponent, DisplayName = "Character Axis Constraint"))
class PIRATESADVENTURE_API UPA_Movement_AxisRestraint : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPA_Movement_AxisRestraint();

	virtual void BeginPlay();
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	//Determines how fast the character is moved back to the constraint location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constrain To Axis")
		float InterpolationSpeed = 1.f;

	//an extra bool that enables us to turn off the overall constraints without having the delete the individual ones, making toggling them easier.
	UPROPERTY()
		bool bConstraintReleased = false;

	//Constrain character to X axis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constrain To Axis")
		bool bConstrainToX = true;

	//Constrain character to Y axis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constrain To Axis")
		bool bConstrainToY = false;

	//Constrain character to Z axis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constrain To Axis")
		bool bConstrainToZ = false;

	//Stores the current constraint location
	UPROPERTY()
		FVector ConstraintLocation;

	//Called to update or set the X constraint. If left to zero, will take the current owner position
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void ConstrainToXAxis(float XConstraint = 0.f);

	//Called to update or set the Y constraint. If left to zero, will take the current position
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void ConstrainToYAxis(float YConstraint = 0.f);

	//Called to update or set the Z constraint. If left to zero, will take the current position
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void ConstrainToZAxis(float ZConstraint = 0.f);

	//Deactivates any constraint currently active
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void ReleaseConstraint();		

	//Activates any constraint currently active
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void ReinstateConstraint(bool bUpdatePosition = false);

	//Removes any current constraint and sets the constraintlocation to zero.
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void DeleteConstraint();
};
