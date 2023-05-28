// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
#include "PA_PlayerState_WallJump.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Wall Jumping"))
class PIRATESADVENTURE_API UPA_PlayerState_WallJump : public UPA_PlayerState_Base
{
	GENERATED_BODY()
	
public:
	UPA_PlayerState_WallJump();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void PerformJump();
	virtual void Landed(const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall Jump State")
		TArray<UPrimitiveComponent*> OverlappedComponents;

	UPROPERTY()
		FVector WallNormal;

	bool FindWall(TArray<UPrimitiveComponent*>* OverlappingComponents); //find the initial wall and act based on it.
	bool WallCheck(); //to confirm whether we still have a wall to stick to.
	void InterruptWallStick(); //if there's no wall to stick to or we encounter an obstacle, cancel it and make the player fall.
	//void FindWallFromLedgeState(FVector FinalPosition, FRotator FinalRotation);

	bool bDEBUG_MODE = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump State")
		float HorizontalJumpSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump State")
		float DescendSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump State")
		float WallOffset = 20.f;

	UFUNCTION(BlueprintCallable, Category = "Latent")
		void CallBackEndWallJump();
};
