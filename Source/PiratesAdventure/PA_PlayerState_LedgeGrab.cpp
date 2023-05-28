// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_LedgeGrab.h"
#include "PiratesAdventure.h"
#include "Components/StaticMeshComponent.h"
#include "Public/StaticMeshResources.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PA_PlayerState_WallJump.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "PiratesAdventureBoat.h"

UPA_PlayerState_LedgeGrab::UPA_PlayerState_LedgeGrab()
{
	StateID = EPlayerStateType::GPS_GameLedgegrab;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UPA_PlayerState_LedgeGrab::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called every frame
void UPA_PlayerState_LedgeGrab::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float AnimLength, AnimRemainingTime, AnimWeight;

	//First check the Swing Air and update the collision component based on the character location
	UPA_BlueprintFunctionLibrary::GetAnimSequenceData(StateOwner, "LedgeGrabbing", "Swing Air", AnimLength, AnimRemainingTime, AnimWeight);

	if (AnimWeight >= 0.15f && AnimRemainingTime >= 2.5f)
	{
		FTransform NewTransform = StateOwner->GetMesh()->GetBoneTransform(StateOwner->GetMesh()->GetBoneIndex("Crotch_0"));
		FVector NewLocation = StateOwner->GetTransform().InverseTransformPosition(NewTransform.GetLocation());

		StateOwner->SetFrontalCollisionCapsuleLocation(NewLocation);

		StateOwner->GetFrontalCollisionCapsuleOverlaps(OverlappedComponents);
		if (OverlappedComponents.Num() != 0)
		{
			UPrimitiveComponent* HitComp;

			for (auto OverlapIt = OverlappedComponents.CreateIterator(); OverlapIt; ++OverlapIt)
			{
				HitComp = *OverlapIt;
				if (HitComp != NULL
					&& HitComp != StateOwner->GetCapsuleComponent()
					&& HitComp != StateOwner->GetFrontalCollisionCapsuleComponent()
					&& HitComp->GetAttachmentRootActor() != StateOwner)
				{
					bLedgeSwingHitWall = true;
					//SetComponentTickEnabled(false);
				}
			}
		}
	}

	UPA_BlueprintFunctionLibrary::GetAnimSequenceData(StateOwner, "LedgeGrabbing", "Idle/Run Finish", AnimLength, AnimRemainingTime, AnimWeight);
	
	if (AnimWeight >= 0.15f && bCanExitState) //after the initial blend, the weight will never become truly zero.
	{

		StateOwner->SetActorEnableCollision(true);
		StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		StateOwner->SetNewPlayerState(EPlayerStateType::GPS_GameNormal);
	}
}

void UPA_PlayerState_LedgeGrab::BeginState()
{
	SetComponentTickEnabled(true);

	static int32 Tester;
	UE_LOG(AnyLog, Log, TEXT("Attempting ledge grab %d"), Tester++);

	StateOwner->SetFrontalCollisionCapsuleSize(30.f, 2.f);
}

void UPA_PlayerState_LedgeGrab::EndState(EPlayerStateType NewState)
{
	SetComponentTickEnabled(false);

	StateOwner->SetFrontalCollisionCapsuleSize();
	StateOwner->SetFrontalCollisionCapsuleLocation();
	LedgeData.Reset();
	OverlappedComponents.Empty();

	bCanClimbLedge = false;
	bLedgeSwingHitWall = false;
	bCanExitState = false;

	StateOwner->bIsJumping = false;
	StateOwner->bIsDoubleJumping = false;

	Super::EndState(NewState);
}

void UPA_PlayerState_LedgeGrab::ReinstateState()
{
	bStateIsEnabled = true;
}

void UPA_PlayerState_LedgeGrab::FindLedge(TArray<UPrimitiveComponent*>* OverlappingComponents)
{
	static int32 counter;

	if (StateOwner->Controller && StateOwner->CurrentStateType == EPlayerStateType::GPS_GameNormal)
	{
		FVector MeshDimensions;
		FVector CurrentActorLoc = StateOwner->GetActorLocation();
		FRotator CurrentActorRot = StateOwner->GetActorRotation();
		LedgeData.LedgeLocation = FVector::ZeroVector;

		UPrimitiveComponent* HitComp = NULL;
		UPrimitiveComponent* TempHitComp = NULL;

		LedgeData.Reset();

		if (StateOwner->CurrentStateType != EPlayerStateType::GPS_GameLedgegrab)
		{
			for (auto OverlapIt = OverlappingComponents->CreateIterator(); OverlapIt; ++OverlapIt)
			{
				TempHitComp = *OverlapIt;

				//TODO: Maybe you should limit this to static meshes only. It's probably the only kind of thing you want to grab anyway.
				//if (TempHitComp->GetAttachmentRootActor()->GetClass() != StateOwner->GetClass()
				//	&& !TempHitComp->GetAttachmentRootActor()->GetClass()->IsChildOf(APawn::StaticClass())
				//	&& !TempHitComp->GetAttachmentRootActor()->GetClass()->IsChildOf(APiratesAdventureBoat::StaticClass()) //if we try to ledgeclimb the boat, the engine will crash.
				//	&& !TempHitComp->GetAttachmentRootActor()->GetClass()->IsChildOf(AVolume::StaticClass())) //same goes for volumes. 
				if(TempHitComp->GetAttachmentRootActor()->GetClass()->IsChildOf(AStaticMeshActor::StaticClass())) //let's see if this works well with only this.
				{

					if (HitComp == NULL)
						HitComp = TempHitComp;

					//Find the upper 'wall' = falling
					if (StateOwner->GetCharacterMovement()->MovementMode == MOVE_Falling)
					{
						if (LedgeData.LedgeActor == HitComp->GetAttachmentRootActor())
							return;

						else if (TempHitComp->RelativeLocation.Z > HitComp->RelativeLocation.Z)
							HitComp = TempHitComp;
					}
					else if (StateOwner->GetCharacterMovement()->Velocity.Z > 0.f) //find the lower 'wall' = Jumping
						if (TempHitComp->RelativeLocation.Z < HitComp->RelativeLocation.Z)
							HitComp = TempHitComp;
				}

				if (HitComp != NULL)
				{
					FCollisionQueryParams initialTraceParams(TEXT(""), false, StateOwner);
					//initialTraceParams.bTraceAsyncScene = true;
					initialTraceParams.bTraceComplex = true;
					initialTraceParams.bReturnPhysicalMaterial = false;

					MeshDimensions = HitComp->Bounds.BoxExtent;

					float smallZOffSet = MeshDimensions.Z / 3; //Some ledges have convex vertices, lower the tracing a bit
					FVector initialTraceBaseLocation = FVector(CurrentActorLoc.X, CurrentActorLoc.Y, HitComp->RelativeLocation.Z + MeshDimensions.Z - smallZOffSet);
					FVector EndinitialTrace = StateOwner->GetActorLocation() + StateOwner->GetActorForwardVector() * 100;
					EndinitialTrace.Z = HitComp->RelativeLocation.Z + MeshDimensions.Z - smallZOffSet;
					FHitResult initialTraceHit(ForceInit);

					GetWorld()->LineTraceSingleByChannel(initialTraceHit, initialTraceBaseLocation, EndinitialTrace, ECC_WorldStatic, initialTraceParams);

					if (bDEBUG_MODE)
						DrawDebugLine(GetWorld(), initialTraceBaseLocation, EndinitialTrace, FColor::Blue, false, 15.0f, 0, 2.0f);

					if (initialTraceHit.bBlockingHit && initialTraceHit.Component == HitComp)
					{
						if (LedgeData.LedgeComponent != HitComp)
						{
							LedgeData.LedgeComponent = HitComp;
							UpdateEdgesInformation(false);
						}

						LedgeData.LedgeLocation = FindGrabPoint(StateOwner->GetCapsuleComponent(), &LedgeData);

						if (bDEBUG_MODE)
							DrawDebugSphere(GetWorld(), LedgeData.LedgeLocation, 5.0f, 0, FColor::Blue, false, 3.0f, 0);
					}
					else
						return;

					if (CanGrabLedge(HitComp, &initialTraceHit) && StateOwner->GetCharacterMovement()->Velocity.Z < 0.f)
					{
						FRotator finalRotation = FRotationMatrix::MakeFromX(initialTraceHit.Normal).Rotator();
						FVector finalPosition = FRotationMatrix::MakeFromX(initialTraceHit.Normal).TransformVector(FVector(-10, 0, 60)); //ledge offset

						LedgeData.FinalPosition = FRotationMatrix(finalRotation).TransformVector(FVector(-10, 0, 60)); //ledge offset
						finalRotation = initialTraceHit.Normal.GetSafeNormal2D().Rotation();
						LedgeData.FinalRotation = FRotationMatrix::MakeFromX(finalRotation.Vector() * -1).Rotator();
						LedgeData.LedgeComponent = HitComp;
						LedgeData.LedgeActor = HitComp->GetAttachmentRootActor();

						GrabLedge(false);

						StateOwner->SetNewPlayerState(EPlayerStateType::GPS_GameLedgegrab);

						return;
					}

					//if we don't test for wall run in the ledge state, the wall state will continue overwriting the ledge state.
					//if (StateOwner->GetRegisteredState(EPlayerStateType::GPS_GameWallJump) && StateOwner->GetCharacterMovement()->Velocity.Z > 0.f)
					//{
					//	//UE_LOG(AnyLog, Log, TEXT("TESTING FOR WALLRUN"));

					//	FVector LedgeLoc = initialTraceHit.bBlockingHit ? LedgeData.LedgeLocation : FVector::ZeroVector;

					//	initialTraceBaseLocation = StateOwner->GetActorLocation();
					//	EndinitialTrace = initialTraceBaseLocation + (StateOwner->GetActorForwardVector() * StateOwner->GetFrontalCollisionCapsuleComponent()->GetUnscaledCapsuleRadius());

					//	GetWorld()->LineTraceSingleByChannel(initialTraceHit, initialTraceBaseLocation, EndinitialTrace, ECC_WorldStatic, initialTraceParams);
					//	if (bDEBUG_MODE)
					//		DrawDebugLine(GetWorld(), initialTraceBaseLocation, EndinitialTrace, FColor::Yellow, false, 15.0f, 0, 2.0f);

					//	if (CanWallStick(HitComp, initialTraceHit, LedgeLoc))
					//	{
					//		//FRotator finalRotation = FRotationMatrix::MakeFromX(initialTraceHit.Normal).Rotator();
					//		//FVector finalPosition = FRotationMatrix::MakeFromX(initialTraceHit.Normal).TransformVector(FVector(-10, 0, 60)); //ledge offset

					//		//LedgeData.FinalPosition = FRotationMatrix(finalRotation).TransformVector(FVector(-10, 0, 60)); //ledge offset
					//		//finalRotation = initialTraceHit.Normal.GetSafeNormal2D().Rotation();
					//		//LedgeData.FinalRotation = FRotationMatrix::MakeFromX(finalRotation.Vector() * -1).Rotator();
					//		//LedgeData.LedgeComponent = HitComp;
					//		//LedgeData.LedgeActor = HitComp->GetAttachmentRootActor();

					//		UPA_PlayerState_WallJump* WallState = Cast<UPA_PlayerState_WallJump>(StateOwner->GetRegisteredState(EPlayerStateType::GPS_GameWallJump));
					//		WallState->FindWallFromLedgeState(initialTraceHit.ImpactPoint, initialTraceHit.ImpactNormal.Rotation().GetInverse());
					//		StateOwner->SetNewPlayerState(EPlayerStateType::GPS_GameWallJump);
					//		return;
					//	}
					/*}*/
				}
			}
		}
	}

	return;
}

void UPA_PlayerState_LedgeGrab::GrabLedge(bool TwoHands)
{
	//bIsWallRunning = false;

	if (StateOwner->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling || StateOwner->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying && StateOwner->GetCharacterMovement()->Velocity.Z <= 2.0f)
	{
		if (LedgeData.LedgeComponent != NULL && LedgeData.FinalPosition != FVector::ZeroVector)
		{
			//Set up information for the latent move function
			FLatentActionInfo LatentInfo;
			LatentInfo.UUID = 1;
			LatentInfo.Linkage = 0;
			LatentInfo.ExecutionFunction = FName(TEXT("CallbackEndGrabLedge"));
			LatentInfo.CallbackTarget = this;

			//Hardcoded: Make sure the hands fit properly in the ledge location
			LedgeData.FinalPosition.Z += StateOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2;
			UKismetSystemLibrary::MoveComponentTo(StateOwner->GetCapsuleComponent(), LedgeData.LedgeLocation - LedgeData.FinalPosition, LedgeData.FinalRotation, true, false, 0.2f, true, EMoveComponentAction::Move, LatentInfo);

			StateOwner->SetBase(LedgeData.LedgeComponent);
		}
	}
}

void UPA_PlayerState_LedgeGrab::CallbackEndGrabLedge()
{
	//reset component velocity
	StateOwner->GetCapsuleComponent()->ComponentVelocity = FVector::ZeroVector;
	StateOwner->GetCharacterMovement()->DisableMovement();
	StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	StateOwner->GetCharacterMovement()->Velocity = FVector::ZeroVector;

	//bCanMove = true;
	//bIsLedgeGrabbing = true;

	//UE_LOG(AnyLog, Log, TEXT("LedgeGrab latent execution finished"));
}


void UPA_PlayerState_LedgeGrab::UpdateEdgesInformation(bool UseSameHeightOnly, UCapsuleComponent* Capsule, FLedgegrabData* UpdateLocation)
{
	UpdateLocation = UpdateLocation == NULL ? &LedgeData : UpdateLocation;
	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(LedgeData.LedgeComponent->GetAttachmentRootActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (StaticMeshComponent->GetStaticMesh()->RenderData->LODResources.Num() > 0)
	{
		FPositionVertexBuffer* VertexBuffer = &StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		if (VertexBuffer)
		{
			const uint32 VertexCount = VertexBuffer->GetNumVertices();
			float DistanceToPlayerRight = 0.f;
			float VertexHeightRight = 0.f;
			float DistanceToPlayerLeft = 0.f;
			float VertexHeightLeft = 0.f;
			float tmpDistance = 0.f;
			FVector LeftEdgeLoc = FVector::ZeroVector;
			FVector RightEdgeLoc = FVector::ZeroVector;
			FVector CenterComponentSpaceLocation = StaticMeshComponent->GetComponentTransform().InverseTransformPosition(StaticMeshComponent->RelativeLocation);

			FVector ActorLocation = Capsule == NULL ? StateOwner->GetActorLocation() : Capsule->GetComponentLocation();
			ActorLocation.Z += StateOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

			for (uint32 Index = 0; Index < VertexCount; Index++)
			{
				const FVector WorldSpaceVertexLocation = StaticMeshComponent->GetComponentTransform().TransformPosition(VertexBuffer->VertexPosition(Index));

				if (UseSameHeightOnly)
					if (WorldSpaceVertexLocation.Z != UpdateLocation->LedgeLocation.Z)
						continue;

				FTransform TransformCharToWorld = Capsule == NULL ? StateOwner->GetTransform() : Capsule->GetComponentTransform();
				tmpDistance = FVector::Dist(WorldSpaceVertexLocation, ActorLocation);

				if (TransformCharToWorld.InverseTransformPosition(WorldSpaceVertexLocation).Y < 0) //left
				{
					if (tmpDistance < DistanceToPlayerLeft || VertexBuffer->VertexPosition(Index).Z > VertexHeightLeft
						|| DistanceToPlayerLeft == 0)
					{
						DistanceToPlayerLeft = tmpDistance;
						VertexHeightLeft = VertexBuffer->VertexPosition(Index).Z;
						LeftEdgeLoc = WorldSpaceVertexLocation;
					}
				}
				else if (TransformCharToWorld.InverseTransformPosition(WorldSpaceVertexLocation).Y > 0) //right
				{
					if (tmpDistance < DistanceToPlayerRight || VertexBuffer->VertexPosition(Index).Z > VertexHeightRight
						|| DistanceToPlayerRight == 0)
					{
						DistanceToPlayerRight = tmpDistance;
						VertexHeightRight = VertexBuffer->VertexPosition(Index).Z;
						RightEdgeLoc = WorldSpaceVertexLocation;
					}
				}
			}

			if (LeftEdgeLoc != FVector::ZeroVector && RightEdgeLoc != FVector::ZeroVector)
			{
				UpdateLocation->LeftVertexLocation = LeftEdgeLoc;
				UpdateLocation->RightVertexLocation = RightEdgeLoc;

				if (bDEBUG_MODE)
				{
					DrawDebugSphere(GetWorld(), UpdateLocation->LeftVertexLocation, 15, 5, FColor::Red, false, 5.0f, 0);
					DrawDebugSphere(GetWorld(), UpdateLocation->RightVertexLocation, 15, 5, FColor::Green, false, 5.0f, 0);
				}
			}
		}
	}
}

FVector UPA_PlayerState_LedgeGrab::FindGrabPoint(const UCapsuleComponent* Capsule, FLedgegrabData* ledgeInfo)
{
	FVector actorLocation = Capsule->GetComponentLocation();
	FVector actorDirection = Capsule->GetForwardVector() * 50.0f;

	float upper = actorDirection.X*(actorLocation.Y - ledgeInfo->LeftVertexLocation.Y) + actorDirection.Y*(ledgeInfo->LeftVertexLocation.X - actorLocation.X);
	float lower = actorDirection.X*(ledgeInfo->RightVertexLocation.Y - ledgeInfo->LeftVertexLocation.Y) - actorDirection.Y*(ledgeInfo->RightVertexLocation.X - ledgeInfo->LeftVertexLocation.X);
	float t = upper / lower;

	FVector returnVec = ledgeInfo->LeftVertexLocation + t*(ledgeInfo->RightVertexLocation - ledgeInfo->LeftVertexLocation);
	returnVec.Z = ledgeInfo->LeftVertexLocation.Z + t*(ledgeInfo->RightVertexLocation.Z - ledgeInfo->LeftVertexLocation.Z);

	return returnVec;
}

bool UPA_PlayerState_LedgeGrab::CanGrabLedge(UPrimitiveComponent* HitComponent, FHitResult* InitialHit)
{
	UCapsuleComponent* capsule = StateOwner->GetCapsuleComponent();

	if (FMath::Abs((StateOwner->GetActorLocation().Z + StateOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()) - LedgeData.LedgeLocation.Z) > 10)
		return false;
	else if (StateOwner->GetCharacterMovement()->Velocity.Z < 0)
	{
		float smallLedgeOffset = 10.0f;
		FVector CurrentCharacterLocation = StateOwner->GetActorLocation();
		if (CurrentCharacterLocation.Z + StateOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() > LedgeData.LedgeLocation.Z + smallLedgeOffset
			&& CurrentCharacterLocation.Z + StateOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() < LedgeData.LedgeLocation.Z - smallLedgeOffset)
			return false;
	}
	return true;
}

//bool UPA_PlayerState_LedgeGrab::CanWallStick(UPrimitiveComponent* HitComponent, FHitResult InitialHit, FVector LedgePosition)
//{
//	//first check if the player is not falling and whether the wall is straight
//	FVector ComparisonA = LedgeData.LedgeLocation;
//	FVector ComparisonB = InitialHit.ImpactPoint;
//	ComparisonA.Normalize();
//	ComparisonB.Normalize();
//	float LedgeAngle = FMath::Abs(FVector::DotProduct(ComparisonA, ComparisonB));
//
//	UE_LOG(AnyLog, Log, TEXT("CHECKING WALLSTICK %f %f"), InitialHit.ImpactNormal.Z, LedgeAngle);
//
//	if (InitialHit.ImpactNormal.Z > 0.005 //ledges can have slight angles that can still be considered straight.
//		|| LedgeAngle != 0.f)
//		return false;
//
//	if (LedgePosition != FVector::ZeroVector)
//	{
//		//make sure we don't have a ledgeposition lower than the player
//		if (LedgePosition.Z <= StateOwner->GetActorLocation().Z)
//		{
//			FVector CompBounds = HitComponent->Bounds.BoxExtent;
//			LedgePosition.Z = CompBounds.Z + HitComponent->RelativeLocation.Z;
//		}
//
//		//first check whether the distance of the ledge to the ground is large enough. We don't want wall jumping to start on ledges we can reach with jumping
//		//Current formula only takes into account singular jumps.
//		float MinLedgeHeight = (5 * FMath::Square(StateOwner->GetCharacterMovement()->JumpZVelocity / 100.f)) / StateOwner->GetCharacterMovement()->GravityScale;
//		MinLedgeHeight += (StateOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2);
//
//		FCollisionQueryParams FloorTraceParams(TEXT(""), false, StateOwner);
//		FloorTraceParams.bTraceAsyncScene = true;
//		FloorTraceParams.bTraceComplex = false;
//		FloorTraceParams.bReturnPhysicalMaterial = false;
//
//		FVector FloorTraceStart = StateOwner->GetActorLocation();
//		FVector FloorTraceEnd = FloorTraceStart;
//		FloorTraceEnd.Z -= MinLedgeHeight;
//		FHitResult FloorTraceHit(ForceInit);
//
//		if (bDEBUG_MODE)
//			DrawDebugLine(GetWorld(), FloorTraceStart, FloorTraceEnd, FColor::Red, false, 1.f);
//
//		GetWorld()->LineTraceSingleByChannel(FloorTraceHit, FloorTraceStart, FloorTraceEnd, ECC_WorldStatic, FloorTraceParams);
//
//		if (FloorTraceHit.bBlockingHit)
//		{
//			if (FMath::Abs(LedgePosition.Z - FloorTraceHit.ImpactPoint.Z) < MinLedgeHeight)
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}

//for the sake of rotation so we can have more complicated shimmy shapes
void UPA_PlayerState_LedgeGrab::MoveUp(float Value)
{

}

void UPA_PlayerState_LedgeGrab::PerformJump()
{
	float AnimLength, AnimRemainingTime, AnimWeightA, AnimWeightB;

	//First check the Swing Air and update the collision component based on the character location
	UPA_BlueprintFunctionLibrary::GetAnimSequenceData(StateOwner, "LedgeGrabbing", "Hang Air", AnimLength, AnimRemainingTime, AnimWeightA);
	UPA_BlueprintFunctionLibrary::GetAnimSequenceData(StateOwner, "LedgeGrabbing", "Hang Wall", AnimLength, AnimRemainingTime, AnimWeightB);

	if (AnimWeightA >= 0.15f || AnimWeightB >= 0.15f)
	{
		if (StateOwner->GetVerticalMovement() >= 0.f)
		{
			StateOwner->SetActorEnableCollision(false);
			bCanExitState = true;
			bCanClimbLedge = true;
			SetComponentTickEnabled(true); //add functionality that checks whether the climb state is finished
		}
		else
		{
			StateOwner->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

			bStateIsEnabled = false;
			GetWorld()->GetTimerManager().SetTimer(TimerEndStateTimeOut, this, &UPA_PlayerState_LedgeGrab::ReinstateState, EndStateTimeOut, false);

			StateOwner->SetNewPlayerState(EPlayerStateType::GPS_GameNormal);
		}
	}
}