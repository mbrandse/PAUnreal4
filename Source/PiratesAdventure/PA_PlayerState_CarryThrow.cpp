// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_CarryThrow.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "PA_PlayerSpringArmComponent.h"
#include "PA_CharacterMovementComponent.h"
#include "PA_PlayerState_Normal.h"

UPA_PlayerState_CarryThrow::UPA_PlayerState_CarryThrow()
{
	StateID = EPlayerStateType::GPS_GameCarryThrow;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bIsStoppingCarry = false;
	RotationRate = 500.f;
	DeadzoneNormalLow = 0.25f;
	DeadzoneNormalHigh = 0.9f;
	DesiredRotationTolerance = 1.f;
}

// Called when the game starts
void UPA_PlayerState_CarryThrow::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPA_PlayerState_CarryThrow::BeginState()
{
	bIsStoppingCarry = false;

	if (StateOwner)
	{
		UPA_PlayerState_Normal* NormalState = Cast<UPA_PlayerState_Normal>(StateOwner->GetRegisteredState(EPlayerStateType::GPS_GameNormal));
		if (NormalState)
		{
			DesiredRotation = NormalState->DesiredRotation;
		}
	}

	Super::BeginState();
}

void UPA_PlayerState_CarryThrow::EndState(EPlayerStateType NewState)
{
	bIsStoppingCarry = false;

	if (StateOwner)
	{
		UPA_PlayerState_Normal* NormalState = Cast<UPA_PlayerState_Normal>(StateOwner->GetRegisteredState(EPlayerStateType::GPS_GameNormal));
		if (NormalState)
		{
			NormalState->DesiredRotation = DesiredRotation;
		}
	}

	Super::EndState(NewState);
}

bool UPA_PlayerState_CarryThrow::HasReachedTargetRotation()
{
	if (StateOwner)
	{
		return StateOwner->Controller->GetControlRotation().Equals(DesiredRotation, DesiredRotationTolerance);
	}
	
	return false;
}

//JUMPING FUNCTIONALITY
void UPA_PlayerState_CarryThrow::PerformJump()
{
	if (StateOwner->Controller != NULL)
	{
		//No jumps during crouch/duck/attack/aim
		if (StateOwner->GetMovementDisabled()
			|| bIsStoppingCarry)
			return;

		if (!StateOwner->bIsJumping)
		{
			StateOwner->bIsJumping = true;
			StateOwner->Jump();
			return;
		}
	}
}

void UPA_PlayerState_CarryThrow::StartAttack()
{
	//make sure we cannot throw or put down objects when we are falling or jumping
	if (StateOwner 
		&& !StateOwner->GetMovementDisabled()
		&& HasReachedTargetRotation()
		&& !bIsStoppingCarry)
	{
		//Do a check to make sure we have nothing overlapping in front of us (for putting it down)
		TArray<AActor*> OverlappingActors;
		StateOwner->GetFrontalCollisionCapsuleComponent()->SetGenerateOverlapEvents(true);
		StateOwner->GetFrontalCollisionCapsuleComponent()->SetCapsuleSize(45.f, 30.f);
		StateOwner->GetFrontalCollisionCapsuleComponent()->SetRelativeLocation(FVector(45, 0, -70), true);
		StateOwner->GetFrontalCollisionCapsuleComponent()->GetOverlappingActors(OverlappingActors);
		AStaticMeshActor* HitActor;
		bool bHasOverlaps = false;
		for (auto OverlapIt = OverlappingActors.CreateIterator(); OverlapIt; ++OverlapIt)
		{
			HitActor = Cast<AStaticMeshActor>(*OverlapIt);
			if (HitActor)
			{
				bHasOverlaps = true;
				break;
			}
		}
		StateOwner->GetFrontalCollisionCapsuleComponent()->SetGenerateOverlapEvents(false);
		StateOwner->GetFrontalCollisionCapsuleComponent()->SetCapsuleSize(30.f, 80.f);
		StateOwner->GetFrontalCollisionCapsuleComponent()->SetRelativeLocation(FVector(100, 0, 0));

		if (!bHasOverlaps)
		{
			if (OnStopCarry.IsBound())
			{
				OnStopCarry.Broadcast(StateOwner->GetVelocity().Size());
				bIsStoppingCarry = true;
			}
			else
			{
				StateOwner->SetNewPlayerState(EPlayerStateType::GPS_GameNormal);
			}
		}
	}
}

void UPA_PlayerState_CarryThrow::StartContext()
{
	//We just treat this as an additional attack button
	//For now we comment this out for testing the character's functions to turn off context actions
	//StartAttack();
}

void UPA_PlayerState_CarryThrow::MoveRight(float Value)
{
	if (StateOwner)
	{
		//Setup modified X and Y input and get angle.
		float CorrectX, CorrectY;
		UPA_BlueprintFunctionLibrary::GetCorrectedInput(Value, StateOwner->GetVerticalMovement(), DeadzoneNormalLow, DeadzoneNormalHigh, CorrectX, CorrectY);
		float InputAngle = FMath::Clamp(FMath::RadiansToDegrees(FMath::Atan2(CorrectY, CorrectX)) + 180.f, 0.f, 360.f); //clamp it just to be sure we never get negative values
		FVector Direction = FVector::ZeroVector;
		UPA_PlayerSpringArmComponent* CameraBoom = Cast<UPA_PlayerSpringArmComponent>(StateOwner->GetCameraBoom());

		StateOwner->CorrectedHorizontalMovementSpeed = CorrectX;
		StateOwner->CorrectedVerticalMovementSpeed = CorrectY;

		if (StateOwner->bIsCrouched
			|| StateOwner->bIsDucked)
		{
			UPA_CharacterMovementComponent* MovementComponent = Cast<UPA_CharacterMovementComponent>(StateOwner->GetCharacterMovement());
			if (MovementComponent)
			{
				MovementComponent->bWantsToDuck = false;
				MovementComponent->bWantsToCrouch = false;
			}
		}

		if (CorrectX != 0.f)
		{
			if (CorrectX < 0.f)
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

		//Apply rotation
		if (StateOwner->Controller->GetControlRotation() != DesiredRotation
			&& !StateOwner->bDisableAutoRotation)
			StateOwner->Controller->SetControlRotation(FMath::RInterpConstantTo(StateOwner->Controller->GetControlRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationRate));
	}
}