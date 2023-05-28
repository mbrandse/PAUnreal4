// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
#include "PA_PlayerState_LedgeGrab.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLedgegrabData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector LedgeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector FinalPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector LeftVertexLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector RightVertexLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FRotator FinalRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector WallRunDestination;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AActor* LedgeActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPrimitiveComponent* LedgeComponent;

	void Reset()
	{
		LedgeLocation = FVector::ZeroVector;
		FinalPosition = FVector::ZeroVector;
		LeftVertexLocation = FVector::ZeroVector;
		RightVertexLocation = FVector::ZeroVector;
		FinalRotation = FRotator::ZeroRotator;
		WallRunDestination = FVector::ZeroVector;
		LedgeActor = NULL;
		LedgeComponent = NULL;
	}
};

UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Ledge Grabbing"))
class PIRATESADVENTURE_API UPA_PlayerState_LedgeGrab : public UPA_PlayerState_Base
{
	GENERATED_BODY()

public:
	UPA_PlayerState_LedgeGrab();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void MoveUp(float Value);
	virtual void PerformJump();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Grabbing State")
		TArray<UPrimitiveComponent*> OverlappedComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Grabbing State")
		FLedgegrabData LedgeData;
	
	//Determines whether the character will swing, or play the collide animation. Also determines how the player climbs the wall.
	bool bLedgeSwingHitWall = false;
	bool bCanClimbLedge = false;
	bool bCanExitState = false;

	//bCanEnterState is necessary for state cooldown, since otherwise the character will just grab the ledge again the moment he starts falling. 
	UPROPERTY()
		bool bStateIsEnabled = true;

	UPROPERTY()
		float EndStateTimeOut = 0.1f;

	UFUNCTION()
		void ReinstateState();

	FTimerHandle TimerEndStateTimeOut;

	bool bDEBUG_MODE = false;

	void FindLedge(TArray<UPrimitiveComponent*>* OverlappingComponents);
	void UpdateEdgesInformation(bool UseSameHeightOnly, UCapsuleComponent* Capsule = NULL, FLedgegrabData* UpdateLocation = NULL);
	FVector FindGrabPoint(const UCapsuleComponent* Capsule, FLedgegrabData* ledgeInfo);
	bool CanGrabLedge(UPrimitiveComponent* HitComponent, FHitResult* InitialHit);
	void GrabLedge(bool TwoHands);
	//bool CanWallStick(UPrimitiveComponent* HitComponent, FHitResult InitialHit, FVector LedgePosition);

	UFUNCTION(BlueprintCallable, Category = "Ledge Grabbing State")
		void CallbackEndGrabLedge(); 

	UFUNCTION(BlueprintPure, Category = "Ledge Grabbing State")
		bool LedgeSwingHitWall() { return bLedgeSwingHitWall; };

	UFUNCTION(BlueprintPure, Category = "Ledge Grabbing State")
		bool CanClimb() { return bCanClimbLedge; };
};
