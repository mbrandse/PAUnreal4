// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PA_PlayerState_Base.h"
#include "PA_PlayerState_Normal.generated.h"

/**
 * 
 */
 //Used to determine what kind of movement we should process on moving left/right
UENUM()
enum class EMoveMode : uint8
{
	MD_WALKING UMETA(DisplayName = "Walking"), 
	MD_DUCKING UMETA(DisplayName = "Ducking"), 
	MD_CROUCHING UMETA(DisplayName = "Crouching")
};

UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Normal"))
class PIRATESADVENTURE_API UPA_PlayerState_Normal : public UPA_PlayerState_Base
{
	GENERATED_BODY()

public:

	UPA_PlayerState_Normal();
	
	// Called when the game starts
	virtual void InitializeComponent() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);

	virtual void MoveRight(float Value);
	virtual void MoveUp(float Value);

	virtual void PerformJump();
	virtual void Landed(const FHitResult& Hit);

	virtual void StartAttack();
	virtual void StopAttack();

	virtual void StartDefend();
	virtual void StopDefend();

	virtual void StartDodge();
	void StopDodge();

	void StopDodgeCooldown();
	FTimerHandle DodgeCoolDownHandle;

	//virtual void StartCrouch();
	//virtual void StopCrouch();

	virtual void AimToolRight(float Value);
	virtual void AimToolUp(float Value);

	virtual void StartTool();
	virtual void StopTool();

	//virtual void StartContext();
	//virtual void StopContext();

	//SLIPTURN FUNCTIONALITY

	/** How much "cooldown" between button presses is allowed to determine the actions after stopping with running **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float RunStopTolerance = 0.2f;

	UPROPERTY()
		float HorizontalMovementForRunLock;

	UPROPERTY()
		bool bIsProcessingStopRun = false;

	UPROPERTY()
		FTimerHandle StopRunTimerHandle;

	UPROPERTY()
		bool bIsSlipturning;

	//bool CanStopRun();

	//void ProcessSlipTurn();

	//void CancelCanStopRun();

	//void FinalizeSlipTurn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneNormalLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneNormalHigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneCrouchDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneAimLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DeadzoneAimHigh;

	//The minimum X input that is required to make a character start crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float CrouchEnterRange;

	//The maximum interval before we actually start crouching. Calculated in sec. Implemented so we don't accidentally hit down. Should be a small value, like 0.1.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float CrouchMaxInterval;

	//The maximum angle at which the player can enter crouch mode, taken from the X and Y input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float CrouchEnterZone;

	//The maximum angle at which the player can exit crouch mode, taken from the X and Y input
	//Consider making this slightly higher than 45 degrees, since keyboard XY input becomes 45 degrees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float CrouchExitZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float DodgeCooldown;

	bool bCanDodge;

	bool bIsDucking;

	EMoveMode MoveMode;

	UPROPERTY()
		class UPA_CharacterMovementComponent* MovementComponent;

	//Bool functions, so we can concentrate annoying && conditionals into one place. 
	bool CanDuck();

	bool CanCrouch();
	
};
