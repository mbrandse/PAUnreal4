// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_Normal.h"
#include "PiratesAdventure.h"
#include "PA_PlayerAnimInstance.h"
#include "PA_CharacterMovementComponent.h"
#include "PA_PlayerSpringArmComponent.h"
#include "PA_Weapon_Base.h"
#include "PA_Tool_Base.h"
#include "PA_BlueprintFunctionLibrary.h"

// Sets default values for this component's properties
UPA_PlayerState_Normal::UPA_PlayerState_Normal()
{
	StateID = EPlayerStateType::GPS_GameNormal;
	bIsDefaultState = true;
	bIsDucking = false;
	RotationRate = 500.f;
	DeadzoneNormalLow = 0.25f;
	DeadzoneNormalHigh = 0.9f;
	DeadzoneCrouchDegrees = 20.f;
	CrouchEnterRange = -0.8f;
	CrouchMaxInterval = 0.05f;
	CrouchEnterZone = 22.5;
	CrouchExitZone = 50.f;
	DodgeCooldown = 1.f;
	bCanDodge = true;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	MoveMode = EMoveMode::MD_WALKING;
}

// Called when the game starts
void UPA_PlayerState_Normal::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called every frame
void UPA_PlayerState_Normal::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPA_PlayerState_Normal::BeginState()
{
	Super::BeginState();
}

void UPA_PlayerState_Normal::EndState(EPlayerStateType NewState)
{
	//FinalizeSlipTurn();

	if (StateOwner->bIsJumping || StateOwner->bIsDoubleJumping)
	{
		StateOwner->bIsJumping = false;
		StateOwner->bIsDoubleJumping = false;
	}

	MoveMode = EMoveMode::MD_WALKING;

	Super::EndState(NewState);
}

//JUMPING FUNCTIONALITY
void UPA_PlayerState_Normal::PerformJump()
{
	if (StateOwner->Controller != NULL)
	{
		//No jumps during crouch/duck/attack/aim
		if (StateOwner->IsDefending()
			|| StateOwner->GetMovementDisabled()
			|| StateOwner->bIsCrouched
			|| StateOwner->bIsDucked)
			return;

		if (StateOwner->EquippedWeapon
			&& StateOwner->EquippedWeapon->bIsAttacking)
			return;

		if (StateOwner->EquippedTool
			&& StateOwner->EquippedTool->bToolHasAiming
			&& StateOwner->EquippedTool->bToolIsActive)
			return;

		if (!StateOwner->bIsJumping)
		{
			StateOwner->bIsJumping = true;
			StateOwner->Jump();
			return;
		}
		else if (!StateOwner->bIsDoubleJumping && StateOwner->bCanDoubleJump && StateOwner->bIsJumping)
		{
			StateOwner->DoubleJump();
			return;
		}
	}
}

void UPA_PlayerState_Normal::Landed(const FHitResult& Hit)
{
	if (StateOwner)
	{
		if (StateOwner->EquippedWeapon)
			StateOwner->EquippedWeapon->LandFromAttack();
	}
}

//ATTACK FUNCTIONALITY
void UPA_PlayerState_Normal::StartAttack()
{
	if (StateOwner)
	{
		if (StateOwner->EquippedWeapon
			&& !StateOwner->EquippedWeapon->bAttackIsCoolingDown)
		{
			StateOwner->EquippedWeapon->bIsAttacking = true;

			//We also need to set the charge attack variables, so we can make sure the end of the button press can turn into a charge. 
			StateOwner->EquippedWeapon->bCanContinueChargeAttack = true;

			if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(StateOwner->EquippedWeapon->AnimationFinishedHandle))
			{
				//In case an animation is playing, we will want to set the continue combo button in case our anim blueprint needs it.
				StateOwner->EquippedWeapon->ProcessPressAttackButton();
			}
		}
	}
}

void UPA_PlayerState_Normal::StopAttack()
{
	if (StateOwner)
	{
		if (StateOwner->EquippedWeapon)
		{
			StateOwner->EquippedWeapon->bIsAttacking = false;
			StateOwner->EquippedWeapon->bCanContinueChargeAttack = false;

			//build in check to make sure you won't get charge attacks during incompatible states.
			//if (bIsSlipturning)
			//{
			//	StateOwner->EquippedWeapon->InterruptChargeAttack();
			//	return;
			//}

			StateOwner->EquippedWeapon->ProcessStopChargeAttack();
		}
	}
}

void UPA_PlayerState_Normal::StartDefend()
{
	UE_LOG(AnyLog, Log, TEXT("Starting Defense"));

	//Too simplistic at the moment of course
	if (StateOwner
		&& !StateOwner->bIsCrouched)
	{
		StateOwner->bIsDefending = true;
	}
}

void UPA_PlayerState_Normal::StopDefend()
{
	UE_LOG(AnyLog, Log, TEXT("Stopping Defense"));

	if (StateOwner)
	{
		StateOwner->bIsDefending = false;
	}
}

void UPA_PlayerState_Normal::StartDodge()
{
	if (StateOwner
		&& !StateOwner->GetMovementDisabled()
		&& !StateOwner->bIsCrouched
		&& bCanDodge)
	{
		UPA_PlayerAnimInstance* PlayerAnimInstance = Cast<UPA_PlayerAnimInstance>(StateOwner->GetMesh()->GetAnimInstance());
		if (PlayerAnimInstance)
		{
			StateOwner->DisableMovement();
			UPA_CharacterMovementComponent* MoveComp = Cast<UPA_CharacterMovementComponent>(StateOwner->GetCharacterMovement());
			if (MoveComp)
			{
				MoveComp->bWantsToCrouch = false;
				MoveComp->bWantsToDuck = false;
			}
			StateOwner->bIsDefending = false;

			if (StateOwner->GetLastKnownDirection() < 1)
			{
				DesiredRotation = FRotator(0, 90, 0);
				StateOwner->Controller->SetControlRotation(DesiredRotation);
			}
			else
			{
				DesiredRotation = FRotator(0, 270, 0);
				StateOwner->Controller->SetControlRotation(DesiredRotation);
			}

			PlayerAnimInstance->StartDodge(FVector(0.f, StateOwner->GetLastKnownDirection(), 0.f));
			bCanDodge = false;
		}
		//as long as our movement is not disabled, we should be able to dodge. 
	}
}

void UPA_PlayerState_Normal::StopDodge()
{
	if (StateOwner)
	{
		StateOwner->EnableMovement();
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(DodgeCoolDownHandle, this, &UPA_PlayerState_Normal::StopDodgeCooldown, DodgeCooldown, false);
		}
	}
}

void UPA_PlayerState_Normal::StopDodgeCooldown()
{
	bCanDodge = true;
}

void UPA_PlayerState_Normal::StartTool()
{
	if (StateOwner && StateOwner->EquippedTool)
	{
		StateOwner->EquippedTool->StartTool();
	}
}

void UPA_PlayerState_Normal::StopTool()
{
	if (StateOwner && StateOwner->EquippedTool)
	{
		StateOwner->EquippedTool->StopTool();
	}
}

void UPA_PlayerState_Normal::AimToolRight(float Value)
{
	if (StateOwner->EquippedTool
		&& StateOwner->EquippedTool->bToolHasAiming
		//&& !StateOwner->bIsDucked
		&& !StateOwner->bIsCrouched
		&& StateOwner->GetVelocity().Z == 0.f) //no aiming during fall, since our aim will be straight at that point.
	{
		float CorrectX, CorrectY;
		UPA_BlueprintFunctionLibrary::GetCorrectedInput(Value, StateOwner->GetVerticalAim(), DeadzoneAimLow, DeadzoneAimHigh, CorrectX, CorrectY);
		//StateOwner->EquippedTool->AimToolRight = CorrectX;
		//StateOwner->EquippedTool->AimToolUp = CorrectY;

		if (FMath::Abs(CorrectY) != 0.0f)
		{
			StateOwner->EquippedTool->StartAimTool(CorrectX, CorrectY);
		}
		else
		{
			StateOwner->EquippedTool->StopAimTool();
		}

		if (StateOwner->GetCorrectedHorizontalMovement() == 0.0f)
		{
			if (CorrectX != 0.0f)
			{
				//Set character rotation, but only when we are not ducking
				if (Value < 0 /*&& StateOwner->Controller->GetControlRotation().Yaw != 90.f*/)
				{
					DesiredRotation = FRotator(0, 90, 0);
				}
				else
				{
					DesiredRotation = FRotator(0, 270, 0);
				}

				StateOwner->Controller->SetControlRotation(FMath::RInterpConstantTo(StateOwner->Controller->GetControlRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationRate));
			}
		}
	}
}

void UPA_PlayerState_Normal::AimToolUp(float Value)
{

}

bool UPA_PlayerState_Normal::CanCrouch()
{
	bool bIsCurrentlyAiming = false;

	if (StateOwner->EquippedTool
		&& StateOwner->EquippedTool->bToolHasAiming
		&& StateOwner->EquippedTool->bToolIsActive)
	{
		bIsCurrentlyAiming = true;
	}

	return !StateOwner->bIsJumping
		&& !StateOwner->bIsDoubleJumping
		&& !bIsCurrentlyAiming
		&& !StateOwner->bIsDefending;
}

bool UPA_PlayerState_Normal::CanDuck()
{
	return !StateOwner->bIsJumping
		&& !StateOwner->bIsDoubleJumping;
}

void UPA_PlayerState_Normal::MoveRight(float Value)
{
	if (StateOwner)
	{
		//Setup the movement component, in case we don't have one.
		if (!MovementComponent)
			MovementComponent = Cast<UPA_CharacterMovementComponent>(StateOwner->GetCharacterMovement());

		//Setup modified X and Y input and get angle.
		float CorrectX, CorrectY;
		UPA_BlueprintFunctionLibrary::GetCorrectedInput(Value, StateOwner->GetVerticalMovement(), DeadzoneNormalLow, DeadzoneNormalHigh, CorrectX, CorrectY);
		float InputAngle = FMath::Clamp(FMath::RadiansToDegrees(FMath::Atan2(CorrectY, CorrectX)) + 180.f, 0.f, 360.f); //clamp it just to be sure we never get negative values
		FVector Direction = FVector::ZeroVector;
		UPA_PlayerSpringArmComponent* CameraBoom = Cast<UPA_PlayerSpringArmComponent>(StateOwner->GetCameraBoom());

		StateOwner->CorrectedHorizontalMovementSpeed = CorrectX;
		StateOwner->CorrectedVerticalMovementSpeed = CorrectY;

		bool bIsCurrentlyAiming = false;

		if (StateOwner->EquippedTool
			&& StateOwner->EquippedTool->bToolHasAiming
			&& StateOwner->EquippedTool->bToolIsActive)
		{
			bIsCurrentlyAiming = true;
		}

		//If we are falling or jumping, reset everything immediately.
		if (StateOwner->GetVelocity().Z != 0.f
			|| StateOwner->bIsJumping
			|| StateOwner->bIsDoubleJumping)
		{
			MovementComponent->bWantsToDuck = false;
			MovementComponent->bWantsToCrouch = false;
			//MoveMode = EMoveMode::MD_WALKING;
		}

		//--------------------------------------------------------------------------------------------------------------
		//We evaluate the move mode from bottom up; 
		// 1. first we check whether we can crouch. 
		// 2. if we are not crouching, we can check whether we should duck. 
		// 3. if that is false too, the only remaining option is walking
		//
		// For all modes, we need to use bIsCrouched and bIsDucked to see whether the state is actually active. 
		// When changing modes we instead said bWantsToCrouch/Duck rather than bIsCrouched/Ducked, since our state 
		// may not actually change even if we request it. 
		//
		// In addition, we don't want to activate either duck or crouch when the player is aiming a tool
		//
		//--------------------------------------------------------------------------------------------------------------

		//--------------------------------------------------------------------------------------------------------------
		//IF THE CHARACTER IS CROUCHING, USE CROUCH MODE
		//--------------------------------------------------------------------------------------------------------------
		if (StateOwner->bIsCrouched
			/*&& !StateOwner->bIsJumping
			&& !StateOwner->bIsDoubleJumping*/)
		{
			if (CanCrouch()
				/*!bIsCurrentlyAiming
				&& !StateOwner->bIsDefending*/)
			{
				//if our angles are within duck range, reset back to duck
				if (InputAngle > 90.f - CrouchEnterZone
					&& InputAngle < 90.f + CrouchEnterZone
					&& CorrectY < -0.1f)
				{
					//MovementComponent->UnCrouch();
					//MovementComponent->Duck();
					MovementComponent->bWantsToCrouch = false;
					MovementComponent->bWantsToDuck = true;
				}
				else if (InputAngle > 90.f + CrouchExitZone
					|| InputAngle < 90.f - CrouchExitZone //if we exceed exit range, get back to walking
					|| CorrectY >= -0.1f)
				{
					MovementComponent->bWantsToDuck = false;
					MovementComponent->bWantsToCrouch = false;
				}

				//Just in case here we assume that we can still remain in this function, even if we satisfied the conditions for walking.
				//This is because even if we satisfy the conditions, it doesn't matter anything if the collision capsule cannot be resized.
				float ValueFromAngle = 0.f;
				float MinRange, MaxRange;
				if (InputAngle < 90.f - CrouchEnterZone
					|| InputAngle > 270.f) //since it wraps around. A pure left is around 0 degrees, but any lower than that and it becomes 360 degrees or less.
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Going Left")));

					if (InputAngle > 270.f)
					{
						ValueFromAngle = -1.f;
					}
					else
					{
						MinRange = 90.f + DeadzoneCrouchDegrees - CrouchExitZone;
						MaxRange = (90.f - CrouchEnterZone) - MinRange;
						ValueFromAngle = (1.f - FMath::Clamp((InputAngle - MinRange) / MaxRange, 0.f, 1.f)) * -1.f;
					}
				}
				else if (InputAngle > 90.f + CrouchEnterZone)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Going Right")));

					MinRange = 90.f + CrouchEnterZone;
					MaxRange = ((90.f + CrouchExitZone) - DeadzoneCrouchDegrees) - MinRange;
					ValueFromAngle = FMath::Clamp((InputAngle - MinRange) / MaxRange, 0.f, 1.f);
				}

				//failsafe. If X and Y is 0, the angle becomes 180 degrees, resulting in maximum movement to the right even if there's no input.
				if (CorrectX == 0.f)
				{
					ValueFromAngle = 0.f;
				}

				if (CameraBoom != NULL)
				{
					const FRotator YawRotation(0, CameraBoom->GetDesiredRotation().Yaw, 0);
					Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				}
				else
				{
					Direction = FVector(0.f, -1.f, 0.f);
				}

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("InputAngle: %f, ValueFromAngle: %f, Direction: %s"), InputAngle, ValueFromAngle, *(Direction.ToString())));

				StateOwner->AddMovementInput(Direction, ValueFromAngle);
			}
		}
		//--------------------------------------------------------------------------------------------------------------
		//IF THE CHARACTER IS DUCKED, USE DUCK MODE
		//--------------------------------------------------------------------------------------------------------------
		else if (StateOwner->bIsDucked
			/*&& !StateOwner->bIsJumping
			&& !StateOwner->bIsDoubleJumping*/)
		{
			if (CanDuck()
				/*!StateOwner->bIsJumping
				&& !StateOwner->bIsDoubleJumping*/)
			{
				//make sure we set the corrected horizontal movement to 0, for the sake of aiming, since we can assume that during ducking we are not moving anyway.
				StateOwner->CorrectedHorizontalMovementSpeed = 0.f;

				if (!bIsCurrentlyAiming 
					&& !StateOwner->IsDefending()
					&& (CorrectY < CrouchEnterRange
						&& InputAngle > 90.f - CrouchExitZone && InputAngle < 90.f - CrouchEnterZone
						|| InputAngle > 90.f + CrouchEnterZone && InputAngle < 90.f + CrouchExitZone))
				{
					MovementComponent->bWantsToCrouch = true;
					return;
				}
				else if (CorrectY >= -0.1f) //if our Y input is above threshold, get back to walking. We can do this during defense too.
				{
					MovementComponent->bWantsToDuck = false;
					MovementComponent->bWantsToCrouch = false;
					return;
				}

				//Set character rotation, but only if we are not aiming or defending
				if (!bIsCurrentlyAiming
					&& !StateOwner->IsDefending())
				{
					if (InputAngle < 90.f - (CrouchEnterZone / 3.f) /*&& StateOwner->Controller->GetControlRotation().Yaw != 90.f*/)
					{
						DesiredRotation = FRotator(0, 90, 0);
					}
					else if (InputAngle > 90.f + (CrouchEnterZone / 3.f))
					{
						DesiredRotation = FRotator(0, 270, 0);
					}
				}
			}
		}
		//--------------------------------------------------------------------------------------------------------------
		//IF BOTH ARE FALSE, USE WALKING MODE
		//--------------------------------------------------------------------------------------------------------------
		else
		{
			//From walking we can only switch to duck, but only if we are not falling
			if (CorrectY < CrouchEnterRange
				&& InputAngle > 90.f - CrouchEnterZone 
				&& InputAngle < 90.f + CrouchEnterZone
				&& !StateOwner->bIsJumping
				&& !StateOwner->bIsDoubleJumping)
			{
				MovementComponent->bWantsToDuck = true;
				MovementComponent->bWantsToCrouch = false;
				return;
			}

			//Now that we have checked whether the character wants to duck, limit the movement only to during non-defense
			if (!StateOwner->IsDefending())
			{
				//Set character rotation, but only if we are not aiming a tool
				if (!bIsCurrentlyAiming || CorrectX != 0.f)
				{
					if (CorrectX < 0.f /*&& StateOwner->Controller->GetControlRotation().Yaw != 90.f*/)
					{
						DesiredRotation = FRotator(0, 90, 0);
					}
					else if (CorrectX > 0.f)
					{
						DesiredRotation = FRotator(0, 270, 0);
					}
				}

				if (CameraBoom != NULL)
				{
					const FRotator YawRotation(0, CameraBoom->GetDesiredRotation().Yaw, 0);
					Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				}
				else
				{
					Direction = FVector(0.f, -1.f, 0.f);
				}

				StateOwner->AddMovementInput(Direction, CorrectX);
			}
		}

		//only update when it's not equal and auto rotation is enabled
		if (StateOwner->Controller->GetControlRotation() != DesiredRotation
			&& !StateOwner->bDisableAutoRotation)
			StateOwner->Controller->SetControlRotation(FMath::RInterpConstantTo(StateOwner->Controller->GetControlRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationRate));

		//UE_LOG(AnyLog, Warning, TEXT("Current movement is right %f up %f"), CorrectX, CorrectY);

		////only update when it's not equal
		//if (StateOwner->Controller->GetControlRotation() != DesiredRotation)
		//{
		//	StateOwner->Controller->SetControlRotation(FMath::RInterpConstantTo(StateOwner->Controller->GetControlRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationRate));
		//	StateOwner->GetEquippedTool()->bLockToolAiming = false;
		//}
	}
}

void UPA_PlayerState_Normal::MoveUp(float Value)
{

}