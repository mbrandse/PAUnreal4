// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_WallJump.h"
#include "PiratesAdventure.h"
#include "Kismet/KismetSystemLibrary.h"

UPA_PlayerState_WallJump::UPA_PlayerState_WallJump()
{
	StateID = EPlayerStateType::GPS_GameWallJump;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UPA_PlayerState_WallJump::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called every frame
void UPA_PlayerState_WallJump::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (WallCheck())
	{
		StateOwner->SetActorLocation(StateOwner->GetActorLocation() - FVector(0.f, 0.f, DescendSpeed));
	}
	else
	{
		InterruptWallStick();
	}
}

void UPA_PlayerState_WallJump::BeginState()
{
	SetComponentTickEnabled(true);

	static int32 Tester;

	UE_LOG(AnyLog, Log, TEXT("ENTERED WALLJUMP STATE %d"), Tester++);

	//StateOwner->SetFrontalCollisionCapsuleSize(30.f, 2.f); //set the frontal collision so that you can ui
}

void UPA_PlayerState_WallJump::EndState(EPlayerStateType NewState)
{
	SetComponentTickEnabled(false);

	StateOwner->SetFrontalCollisionCapsuleSize();
	StateOwner->SetFrontalCollisionCapsuleLocation();
	OverlappedComponents.Empty();

	Super::EndState(NewState);
}

void UPA_PlayerState_WallJump::PerformJump()
{
	SetComponentTickEnabled(false);

	StateOwner->bIsJumping = true;
	StateOwner->bIsDoubleJumping = true;

	FVector LaunchDirection = WallNormal * HorizontalJumpSpeed;
	LaunchDirection.Z = StateOwner->GetActorUpVector().Z * StateOwner->JumpDoubleHeight;

	StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	StateOwner->LaunchCharacter(LaunchDirection, true, true);

	EndState(EPlayerStateType::GPS_GameNormal);
}

void UPA_PlayerState_WallJump::Landed(const FHitResult& Hit)
{
	SetComponentTickEnabled(false);

	StateOwner->bIsJumping = false;
	StateOwner->bIsDoubleJumping = false;

	StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	EndState(EPlayerStateType::GPS_GameNormal);
}

void UPA_PlayerState_WallJump::InterruptWallStick()
{
	SetComponentTickEnabled(false);

	StateOwner->bIsJumping = true;
	StateOwner->bIsDoubleJumping = true;

	StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	EndState(EPlayerStateType::GPS_GameNormal);
}

bool UPA_PlayerState_WallJump::FindWall(TArray<UPrimitiveComponent*>* OverlappingComponents)
{
	static int32 counter;

	if (StateOwner->Controller && StateOwner->CurrentStateType == EPlayerStateType::GPS_GameNormal)
	{
		FVector MeshDimensions;
		FVector CurrentActorLoc = StateOwner->GetActorLocation();
		FRotator CurrentActorRot = StateOwner->GetActorRotation();

		UPrimitiveComponent* HitComp = NULL;
		UPrimitiveComponent* TempHitComp = NULL;

		if (StateOwner->CurrentStateType != EPlayerStateType::GPS_GameWallJump)
		{
			FCollisionQueryParams initialTraceParams(TEXT(""), false, StateOwner);
			//initialTraceParams.bTraceAsyncScene = true;
			initialTraceParams.bTraceComplex = true;
			initialTraceParams.bReturnPhysicalMaterial = false;

			FVector initialTraceBaseLocation = StateOwner->GetActorLocation();
			FVector EndinitialTrace = StateOwner->GetActorLocation() + StateOwner->GetActorForwardVector() * 100;
			FHitResult initialTraceHit(ForceInit);

			if (!GetWorld())
				return false;

			GetWorld()->LineTraceSingleByChannel(initialTraceHit, initialTraceBaseLocation, EndinitialTrace, ECC_WorldStatic, initialTraceParams);

			if (bDEBUG_MODE)
				DrawDebugLine(GetWorld(), initialTraceBaseLocation, EndinitialTrace, FColor::Blue, false, 15.0f, 0, 2.0f);

			if (initialTraceHit.bBlockingHit)
			{

				FRotator FinalRotation = StateOwner->GetActorForwardVector().Rotation().GetInverse();
				FVector FinalPosition = initialTraceHit.ImpactPoint + (initialTraceHit.ImpactNormal * WallOffset);
				WallNormal = initialTraceHit.ImpactNormal;

				//Set up information for the latent move function
				FLatentActionInfo LatentInfo;
				LatentInfo.UUID = 1;
				LatentInfo.Linkage = 0;
				LatentInfo.ExecutionFunction = FName(TEXT("CallBackEndWallJump"));
				LatentInfo.CallbackTarget = this;

				UKismetSystemLibrary::MoveComponentTo(StateOwner->GetCapsuleComponent(), FinalPosition, FinalRotation, true, false, 0.2f, true, EMoveComponentAction::Move, LatentInfo);

				StateOwner->SetNewPlayerState(EPlayerStateType::GPS_GameWallJump);

				//StateOwner->SetActorEnableCollision(false);
				StateOwner->GetCharacterMovement()->StopActiveMovement();
				StateOwner->GetCharacterMovement()->Velocity = FVector::ZeroVector;
				StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

				return true;
			}
		}
	}

	return false;
}

//void UPA_PlayerState_WallJump::FindWallFromLedgeState(FVector FinalPosition, FRotator FinalRotation)
//{
//	StateOwner->GetCharacterMovement()->StopActiveMovement();
//	StateOwner->GetCharacterMovement()->Velocity = FVector::ZeroVector;
//	StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
//
//	//Set up information for the latent move function
//	FLatentActionInfo LatentInfo;
//	LatentInfo.UUID = 1;
//	LatentInfo.Linkage = 0;
//	LatentInfo.ExecutionFunction = FName(TEXT("CallBackEndWallJump"));
//	LatentInfo.CallbackTarget = this;
//
//	UKismetSystemLibrary::MoveComponentTo(StateOwner->GetCapsuleComponent(), FinalPosition, FinalRotation, true, false, 0.2f, EMoveComponentAction::Move, LatentInfo);
//}

void UPA_PlayerState_WallJump::CallBackEndWallJump()
{

}

bool UPA_PlayerState_WallJump::WallCheck()
{
	return true;
}