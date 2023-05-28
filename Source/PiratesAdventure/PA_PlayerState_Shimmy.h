// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
#include "PA_ISMA_ShimmyLedge.h"
#include "PA_PlayerState_Shimmy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSidleEndSignature, int32, Index);

/**
 * 
 */
UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Shimmying"))
class PIRATESADVENTURE_API UPA_PlayerState_Shimmy : public UPA_PlayerState_Base
{
	GENERATED_BODY()

public:

	UPA_PlayerState_Shimmy();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void MoveUp(float Value);
	virtual void MoveRight(float Value);
	
	UPROPERTY()
		class APA_ISMA_SidleLedge* ShimmyInstance;

	UPROPERTY()
		class USplineComponent* SidlePath;

	UPROPERTY()
		TArray<FRotator> SidleDirections;

	UPROPERTY()
		FVector TargetLocationDuringTransition;

	UPROPERTY()
		float DistanceAlongPath;

	//UPROPERTY()
	//	bool bCanShimmy;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sidle")
		float SidleSpeed;

	bool bReverseInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sidle")
		float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sidle")
		float DeadzoneLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sidle")
		float DeadzoneHigh;

	UPROPERTY()
		int32 CurrentTargetIndex; 

	UPROPERTY()
		FRotator CurrentTargetDirection;

	//UPROPERTY()
	//	bool bCanExitState = false;

	//UFUNCTION(BlueprintCallable, Category = "Climbing State")
	//	void CallbackShimmyFacing();

	//UFUNCTION(BlueprintCallable, Category = "Climbing State")
	//	void FinalizeShimmying();

	//UFUNCTION(BlueprintPure, Category = "Shimmying State")
	//	bool CanShimmy() { return bCanShimmy; };

	//UFUNCTION()
	//	void SetShimmyInstance(class APA_ISMA_SidleLedge* NewInstance) { ShimmyInstance = NewInstance; };

	UFUNCTION(BlueprintCallable, Category = "Sidle")
		void SetSidleData(class USplineComponent* NewSidlePath, TArray<FRotator> NewSidleDirections, int32 StartIndex, bool bSidleIsReversed = false);
	
	UFUNCTION(BlueprintPure, Category = "Sidle")
		void RemainingDistance(float& FromEntryPointA, float& FromEntryPointB);

	UPROPERTY(BlueprintAssignable, Category = "Sidle")
		FSidleEndSignature OnEndSidle;
};
