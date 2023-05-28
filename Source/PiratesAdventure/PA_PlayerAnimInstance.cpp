// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerAnimInstance.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Asset_Item.h"
#include "PA_Weapon_Base.h"
#include "PA_Tool_Base.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "PA_PlayerState_Climbing.h"
#include "PA_PlayerState_Shimmy.h"
#include "PA_PlayerState_LedgeGrab.h"
#include "PA_PlayerState_Boat.h"
#include "PA_PlayerState_Normal.h"

float UPA_PlayerAnimInstance::GetRelativeSpeed()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		FVector CurrentVelocity = Character->GetCapsuleComponent()->GetPhysicsLinearVelocity();
		CurrentVelocity.Z = Character->GetCharacterMovement()->Velocity.Z;

		float MaxVelocity = Character->GetCharacterMovement()->MaxWalkSpeed;

		return CurrentVelocity.Size() / MaxVelocity;
	}

	return 0.f;
}

EPlayerWeaponType UPA_PlayerAnimInstance::GetWeaponType()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		if (Character->GetEquippedWeapon() != NULL)
			return Character->GetEquippedWeapon()->CurrentWeaponType;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: EITHER THE EQUIPPED WEAPON IS NULL, OR THE EQUIPPED WEAPON'S WEAPONTYPE IS NONE."));
	return EPlayerWeaponType::NONE;

	//SetRootMotionMode(ERootMotionMode::RootMotionFromEverything);
}

void UPA_PlayerAnimInstance::GetToolType(EPlayerToolType& ToolType, bool& ToolIsActive)
{
	ToolType = EPlayerToolType::NONE;
	ToolIsActive = false;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character
		&& Character->GetEquippedTool() != NULL)
	{
		for (int32 Index = 0; Index < Character->Tools.Num(); Index++)
		{
			if (Character->Tools[Index] != NULL
				&& Character->Tools[Index]->ToolType == Character->GetEquippedTool()->ToolType
				&& Character->Tools[Index]->ItemData->bUnlocked)
			{
				ToolType = Character->GetEquippedTool()->ToolType;
				//This active is different from the inactive in the item data. This basically determines whether a tool is equipped AND being used, since both don't 
				//necessarily happen at the same time.
				ToolIsActive = Character->GetEquippedTool()->bToolIsActive;
			}
		}
	}
}


UAnimSequenceBase* UPA_PlayerAnimInstance::GetWeaponAnimLength(UAnimSequenceBase* Default, UAnimSequenceBase* Fire, UAnimSequenceBase* Ice, UAnimSequenceBase* Wind, UAnimSequenceBase* Wood)
{
	switch (GetWeaponType())
	{
	case EPlayerWeaponType::WEAPON_DEFAULT:
		return Default;
		break;

	case EPlayerWeaponType::WEAPON_FIRE:
		return Fire;
		break;

	case EPlayerWeaponType::WEAPON_ICE:
		return Ice;
		break;

	case EPlayerWeaponType::WEAPON_WIND:
		return Wind;
		break;

	case EPlayerWeaponType::WEAPON_WOOD:
		return Wood;
		break;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WE DIDN'T GET A RETURN VALUE"));

	return NULL;
}

float UPA_PlayerAnimInstance::GetHorizontalInputDirection()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		return Character->GetHorizontalMovement();
	}

	return 0.f;
}

float UPA_PlayerAnimInstance::GetverticalInputDirection()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		return Character->GetVerticalMovement();
	}

	return 0.f;
}

float UPA_PlayerAnimInstance::GetHorizontalAim()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		return Character->GetHorizontalAim();
	}

	return 0.f;
}

float UPA_PlayerAnimInstance::GetverticalAim()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		return Character->GetVerticalAim();
	}

	return 0.f;
}

int32 UPA_PlayerAnimInstance::GetLastKnownDirection()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		return Character->LastKnownDirection;
	}

	return 1;
}

bool UPA_PlayerAnimInstance::IsAttacking()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		if (Character->GetEquippedWeapon() != NULL)
			return Character->GetEquippedWeapon()->bIsAttacking;
	}

	return false;
}

bool UPA_PlayerAnimInstance::IsChargeAttacking()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		if (Character->GetEquippedWeapon() != NULL)
			return Character->GetEquippedWeapon()->bIsChargeAttacking;
	}

	return false;
}

bool UPA_PlayerAnimInstance::SetAttacking(bool bCanSet, bool bNewValue)
{
	if (bCanSet)
	{
		APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

		if (Character)
		{
			if (Character->GetEquippedWeapon() != NULL)
				Character->GetEquippedWeapon()->bIsAttacking = bNewValue;
		}
	}

	return bCanSet;
}

bool UPA_PlayerAnimInstance::SetChargeAttacking(bool bCanSet, bool bNewValue)
{
	if (bCanSet)
	{
		APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

		if (Character)
		{
			if (Character->GetEquippedWeapon() != NULL)
				Character->GetEquippedWeapon()->bIsChargeAttacking = bNewValue;
		}
	}

	return bCanSet;
}

//DAMAGE
void UPA_PlayerAnimInstance::StopReceivingDamage()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		Character->OnStopProcessingDamageAnimation();
		ReceiveStopReceivingDamage();
	}
}

//DODGE
void UPA_PlayerAnimInstance::StopDodge()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		UPA_PlayerState_Normal* NormalState = Cast<UPA_PlayerState_Normal>(Character->GetRegisteredState(Character->GetCurrentPlayerState()));
		if (NormalState)
		{
			NormalState->StopDodge();
			ReceiveStopDodge();
		}
	}
}

bool UPA_PlayerAnimInstance::IsUsingTool()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());

	if (Character)
	{
		if (Character->GetEquippedTool() != NULL)
			return Character->GetEquippedTool()->bToolIsBeingUsed;
	}

	return false;
}

bool UPA_PlayerAnimInstance::IsSlotActive(FName SlotNodeName)
{
	// stop temporary montage
	// when terminate (in the Montage_Advance), we have to lose reference to the temporary montage
	if (SlotNodeName != NAME_None)
	{
		for (int32 InstanceIndex = 0; InstanceIndex < MontageInstances.Num(); InstanceIndex++)
		{
			// check if this is playing
			FAnimMontageInstance* MontageInstance = MontageInstances[InstanceIndex];
			// make sure what is active right now is transient that we created by request
			if (MontageInstance && MontageInstance->IsActive() && MontageInstance->IsPlaying())
			{
				UAnimMontage* CurMontage = MontageInstance->Montage;

				if (CurMontage /* CurMontage->GetOuter() == GetTransientPackage() */) //The GetOuter part doesn't seem to work in this instance. Keep in mind if this is gonna cause problems.
				{
					// Check each track, in practice there should only be one on these
					for (int32 SlotTrackIndex = 0; SlotTrackIndex < CurMontage->SlotAnimTracks.Num(); SlotTrackIndex++)
					{
						const FSlotAnimationTrack* AnimTrack = &CurMontage->SlotAnimTracks[SlotTrackIndex];

						if (AnimTrack && AnimTrack->SlotName == SlotNodeName)
						{
							// Found it
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

//SHIMMYING FUNCTIONS
void UPA_PlayerAnimInstance::CanShimmy(bool& bCanShimmy)
{
	bCanShimmy = false;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character)
	{
		UPA_PlayerState_Shimmy* PlayerState = Cast<UPA_PlayerState_Shimmy>(Character->GetRegisteredState(EPlayerStateType::GPS_GameShimmy));
		if (PlayerState)
		{
			bCanShimmy = PlayerState->SidlePath != NULL;
			//bCanShimmy = PlayerState->bCanShimmy;
		}
	}
}

void UPA_PlayerAnimInstance::ShimmyDistanceTraceToA(bool& bCanFinishShimmy, float Range)
{
	bCanFinishShimmy = false;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character)
	{
		UPA_PlayerState_Shimmy* PlayerState = Cast<UPA_PlayerState_Shimmy>(Character->GetRegisteredState(EPlayerStateType::GPS_GameShimmy));
		if (PlayerState)
		{
			float DistFromA, DistFromB;

			PlayerState->RemainingDistance(DistFromA, DistFromB);
			bCanFinishShimmy = DistFromA <= Range;
		}
	}
}

void UPA_PlayerAnimInstance::ShimmyDistanceTraceToB(bool& bCanFinishShimmy, float Range)
{
	bCanFinishShimmy = false;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character)
	{
		UPA_PlayerState_Shimmy* PlayerState = Cast<UPA_PlayerState_Shimmy>(Character->GetRegisteredState(EPlayerStateType::GPS_GameShimmy));
		if (PlayerState)
		{
			float DistFromA, DistFromB;

			PlayerState->RemainingDistance(DistFromA, DistFromB);
			bCanFinishShimmy = DistFromB <= Range;
		}
	}
}

//LEDGE GRABBING FUNCTIONS
void UPA_PlayerAnimInstance::HitWallDuringSwing(bool& bHitWall)
{
	bHitWall = false;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character)
	{
		UPA_PlayerState_LedgeGrab* PlayerState = Cast<UPA_PlayerState_LedgeGrab>(Character->GetRegisteredState(EPlayerStateType::GPS_GameLedgegrab));
		if (PlayerState)
		{
			bHitWall = PlayerState->LedgeSwingHitWall();
		}
	}
}

void UPA_PlayerAnimInstance::CanClimbLedge(bool& bCanClimbLedge)
{
	bCanClimbLedge = false;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character)
	{
		UPA_PlayerState_LedgeGrab* PlayerState = Cast<UPA_PlayerState_LedgeGrab>(Character->GetRegisteredState(EPlayerStateType::GPS_GameLedgegrab));
		if (PlayerState)
		{
			bCanClimbLedge = PlayerState->CanClimb();
		}
	}
}

void UPA_PlayerAnimInstance::GetBoatSocketLocationAndRotation(FName SocketName, FVector& SocketLocation, FRotator& SocketRotation)
{
	SocketLocation = FVector::ZeroVector;
	SocketRotation = FRotator::ZeroRotator;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character)
	{
		UPA_PlayerState_Boat* PlayerState = Cast<UPA_PlayerState_Boat>(Character->GetRegisteredState(EPlayerStateType::GPS_GameBoat));
		if (PlayerState && PlayerState->GetBoatInstance() != NULL)
		{
			SocketLocation = PlayerState->GetBoatInstance()->BoatMesh->GetSocketLocation(SocketName);
			SocketRotation = PlayerState->GetBoatInstance()->BoatMesh->GetSocketRotation(SocketName);
		}
	}
}

//Tool related functions
void UPA_PlayerAnimInstance::ForceToolDeactivation(bool Input, bool& Output)
{
	Output = Input;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetOwningActor());
	if (Character && Input)
	{
		if (!Character->EquippedTool)
			return;

		//this may not have any effect, since it only affects input for a split second.
		if(Character->EquippedTool->bToolHasAiming)
		{
			Character->EquippedTool->StopAimTool();
		}
			
		Character->EquippedTool->StopTool();
	}
}