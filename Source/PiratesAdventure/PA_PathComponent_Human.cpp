// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PathComponent_Human.h"
#include "PA_AI_CharacterBase.h"
#include "GameFramework/Character.h"
#include "PiratesAdventure.h"
#include "PA_NavArea_Base.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavigationPath.h"
#include "AITypes.h"
#include "AIController.h"
#include "NavigationSystem.h"

UPA_PathComponent_Human::UPA_PathComponent_Human()
{
	//FrontalCheckDistance = 250.f;
	//FloorHeightTolerance = 25.f;
	//MaxJumpHeight = 400.f;
	//MaxFallHeight = 800.f;
	//JumpStartTolerance = 75.f;
	//bCanFallOnlyReachableDistances = true;
	//bDebug = false;
	//bRequestedWallTrace = false;
	//bRequestedJump = false;
	//CurrentJumpAction = EJumpAction::JA_NONE;
}

////void UPA_PathComponent_Human::FollowPathSegmentNonJump(float DeltaTime)
////{
////	if (!Path.IsValid() || MovementComp == nullptr)
////	{
////		return;
////	}
////
////	const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
////	const FVector CurrentTarget = GetCurrentTargetLocation();
////
////	// set to false by default, we will set set this back to true if appropriate
////	bIsDecelerating = false;
////
////	const bool bAccelerationBased = MovementComp->UseAccelerationForPathFollowing();
////	if (bAccelerationBased)
////	{
////		CurrentMoveInput = (CurrentTarget - CurrentLocation).GetSafeNormal();
////		CurrentMoveInput.X = 0;
////
////		if (MoveSegmentStartIndex >= DecelerationSegmentIndex)
////		{
////			const FVector PathEnd = Path->GetEndLocation();
////			const float DistToEndSq = FVector::DistSquared(CurrentLocation, PathEnd);
////			const bool bShouldDecelerate = DistToEndSq < FMath::Square(CachedBrakingDistance);
////			if (bShouldDecelerate)
////			{
////				bIsDecelerating = true;
////
////				const float SpeedPct = FMath::Clamp(FMath::Sqrt(DistToEndSq) / CachedBrakingDistance, 0.0f, 1.0f);
////				CurrentMoveInput *= SpeedPct;
////			}
////		}
////
////		PostProcessMove.ExecuteIfBound(this, CurrentMoveInput);
////		MovementComp->RequestPathMove(CurrentMoveInput);
////	}
////	else
////	{
////		FVector MoveVelocity = (CurrentTarget - CurrentLocation) / DeltaTime;
////		MoveVelocity.X = 0;
////
////		const int32 LastSegmentStartIndex = Path->GetPathPoints().Num() - 2;
////		const bool bNotFollowingLastSegment = (MoveSegmentStartIndex < LastSegmentStartIndex);
////
////		PostProcessMove.ExecuteIfBound(this, MoveVelocity);
////		MovementComp->RequestDirectMove(MoveVelocity, bNotFollowingLastSegment);
////	}
////}
//
//////is called every tick to make an AI keep following the path
////void UPA_PathComponent_Human::FollowPathSegment(float DeltaTime)
////{
////	UWorld* World = GetWorld();
////	if (!World) return;
////
////	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
////	const ANavigationData* NavData = World->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
////	AAIController* AIController = Cast<AAIController>(GetOwner());
////
////	if (AIController)
////	{
////		if (NavData
////			&& HasPartialPath()) //if the path is not partial (anymore), we have found a direct route to our target. If the path is partial, we have to jump somewhere to get there. 
////		{
////			const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
////			if (NavMesh)
////			{
////				APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
////
////				if (Character != NULL
////					&& Character->GetCharacterMovement()->Velocity.Z == 0.f) //if a jump is already happening, don't do anything.	
////				{
////					if (Character->bCanJump //make sure we only do this if the character has been set to be able to jump...
////						&& Character->MaxJumpZDistance > 0.f //...and actually has a max jump distance that is bigger than 0.
////						&& Character->GetCharacterMovement()->Velocity.Y != 0.f) //make sure we only evaluate Jump Pathing when character has Y speed
////					{
////						//first reset the horizontal displacement, to make sure we don't accidentally do something weird
////						HorizontalVelocity = 0.f;
////
////						//Initialize some basic location vectors
////						FVector ClosestVert = FVector::ZeroVector;
////						FVector ActorLoc = Character->GetActorLocation();
////						FVector ActorOrigin = ActorLoc; //Get the actor location without the capsule height
////						ActorOrigin.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
////						ActorOrigin.Z += 10.f; //just add a little offset so we are not actually on the floor and our jump arc calculation doesn't get stuck.
////
////						FVector MoveGoal = GetMoveGoal() != NULL ? GetMoveGoal()->GetActorLocation() : OriginalMoveRequestGoalLocation;
////						FVector GoalDirection = MoveGoal - ActorLoc;
////						GoalDirection.Normalize();
////
////						//Use the target as the primary means of getting the forward vector, since we want the AI to moving towards the player. 
////						FVector AIForwardVector = GoalDirection.Y > 0.f ? FVector(0.f, 1.f, 0.f) : FVector(0.f, -1.f, 0.f);
////
////						////Initialize our poly compare locations
////						//FVector NearestPolyCompareLocation = FVector::ZeroVector;
////						//NavNodeRef CurrentPoly = NavMesh->FindNearestPoly(ActorOrigin, FVector(20.f, 20.f, 20.f));
////						//FVector CurrentPolyCompareLocation;
////						//NavMesh->GetPolyCenter(CurrentPoly, CurrentPolyCompareLocation);
////
////						//First start checking whether we are in range of a wall
////						if (FMath::Abs(GetCurrentTargetLocation().Y - Character->GetActorLocation().Y) < Character->WallCheckTolerance
////							&& FMath::Abs(GetCurrentTargetLocation().Y - Character->GetActorLocation().Y) >= Character->JumpCheckTolerance
////							&& !bRequestedWallTrace)
////						{
////							UE_LOG(AnyLog, Log, TEXT("Checking for wall"));
////							bRequestedWallTrace = true;
////
////							if (CalculateWallJump(ActorOrigin, AIForwardVector, WallJumpForce, bDebug))
////							{
////								CurrentJumpAction = EJumpAction::JA_WALL;
////								AIController->StopMovement();
////							}
////
////							//FCollisionQueryParams TraceParams(TEXT(""), false, Character);
////							//TraceParams.bTraceAsyncScene = true;
////							//TraceParams.bTraceComplex = false;
////							//TraceParams.bReturnPhysicalMaterial = false;
////							//TraceParams.AddIgnoredActor(Character); //ignore self
////
////							//FVector TraceStart, TraceEnd;
////							//TraceStart = ActorOrigin;
////							//TraceStart.Z += Character->MinJumpZDistance;
////							//TraceEnd = TraceStart + (AIForwardVector * (Character->WallCheckTolerance * 2.f)); //increase wallcheck just a little so we can have our trace results earlier.
////
////							//FHitResult Trace;
////							//bool bFirstTraceHit = World->LineTraceSingleByChannel(Trace, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
////
////							////In case we get a hit on our initial trace, we have a wall and we can start getting our polies within jumping range. 
////							//if (bFirstTraceHit)
////							//{
////							//	if (bDebug)
////							//	{
////							//		DrawDebugLine(World, TraceStart, Trace.ImpactPoint, FColor::Red, false, 1.f);
////							//		DrawDebugLine(World, Trace.ImpactPoint, TraceEnd, FColor::Green, false, 1.f);
////							//		DrawDebugPoint(World, Trace.ImpactPoint, 20.f, FColor::Green, false, 1.f);
////							//	}
////
////							//	//if we have a hit, we need to get all polies within range.
////							//	FVector CheckBoxCenter = Trace.ImpactPoint;
////							//	CheckBoxCenter.Z += (Character->MaxJumpZDistance * 0.5) + 10.f;
////							//	FBox CheckBox;
////							//	CheckBox.Max = CheckBoxCenter + FVector(50.f, 50.f, Character->MaxJumpZDistance * 0.5);
////							//	CheckBox.Min = CheckBoxCenter - FVector(50.f, 50.f, Character->MaxJumpZDistance * 0.5);
////							//	TArray<FNavPoly> CheckedPolys;
////
////							//	if (bDebug)
////							//		DrawDebugBox(GetWorld(), CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Yellow, false, 0.1f);
////
////							//	NavMesh->GetPolysInBox(CheckBox, CheckedPolys);
////
////							//	if (CheckedPolys.Num() != 0)
////							//	{
////							//		//Do a trace to make sure we didn't get a poly within a static mesh. 
////							//		FVector PolyPoint;
////							//		FVector TargetPoint = FVector::ZeroVector; //The target point will save all potential targets.
////							//		for (FNavPoly Poly : CheckedPolys)
////							//		{
////							//			NavMesh->GetPolyCenter(Poly.Ref, NearestPolyCompareLocation);
////							//			NavMesh->GetClosestPointOnPoly(Poly.Ref, ActorOrigin, PolyPoint);
////							//			if (PolyPoint != FVector::ZeroVector
////							//				&& NearestPolyCompareLocation != CurrentPolyCompareLocation) //make sure we ignore the poly we are on at the moment.
////							//			{
////							//				//use largely the same trace distance we used in our first trace, with modified Z
////							//				TraceEnd = PolyPoint;
////							//				TraceEnd.Z += 10.f;
////							//				TraceStart.Z = TraceEnd.Z;
////
////							//				FVector CeilingTrace = TraceEnd;
////							//				CeilingTrace.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5; //do a bit more height than full height, since the jump arc itself also needs room.
////
////							//				bool bWallTraceHit = World->LineTraceSingleByChannel(Trace, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
////							//				if (!bWallTraceHit)
////							//				{
////							//					bool bCeilingTraceHit = World->LineTraceSingleByChannel(Trace, TraceEnd, CeilingTrace, ECC_Visibility, TraceParams);
////							//					if (!bCeilingTraceHit)
////							//					{
////							//						//If we don't get a hit on both the wall and ceiling trace, we have a reachable distance + enough space on the new poly.
////							//						TargetPoint = PolyPoint;
////							//						break;
////							//					}
////							//				}
////							//			}
////							//		}
////
////							//		if (TargetPoint != FVector::ZeroVector)
////							//		{
////							//			CurrentJumpAction = EJumpAction::JA_WALL;
////							//			AIController->StopMovement();
////
////							//			////if our target is not zero, we have a point to jump to. 
////							//			//if (bDebug)
////							//			//	DrawDebugPoint(World, TargetPoint, 20, FColor::White, false, 20);
////
////							//			//FVector JumpForce;
////							//			//UGameplayStatics::SuggestProjectileVelocity_CustomArc(Character, JumpForce, ActorOrigin, TargetPoint, 0.f, 0.3f);
////
////							//			//if (JumpForce != FVector::ZeroVector)
////							//			//{
////							//			//	Character->GetCharacterMovement()->Velocity = JumpForce;
////							//			//	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
////							//			//}
////							//			//else
////							//			//{
////							//			//	//Our jump force became 0, so abort the path
////							//			//	AIController->StopMovement();
////
////							//			//	if (bDebug)
////							//			//		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("No viable jump force on first jump check"));
////							//			//}
////							//		}
////							//	}
////						}
////						//else
////						//{
////						//	if (bDebug)
////						//		DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, 1.f);
////						//}
////					}
////
////					//TEMPORARILY DISABLED UNTIL WE GET THE FIRST SECTION (JUMP) WORKING WITHOUT RELYING ON A COLLISION CAPSULE.
////					//Next up we want to check whether our character is actually getting close on the current target location (the end point on the current nav poly)
////					else if (FMath::Abs(GetCurrentTargetLocation().Y - Character->GetActorLocation().Y) < Character->JumpCheckTolerance
////						&& CurrentJumpAction == EJumpAction::JA_NONE) //only process this when we have no wall traced
////					{
////						//New implementation; put jumping functionality within the tasks for control, and use the FollowPath to setup jump requests.
////						CurrentJumpAction = EJumpAction::JA_OTHER;
////						AIController->StopMovement();
////
////						////For our first check we first see if we have some small distance path close to us, so we can take that one instead. 
////						////if we have a hit, we need to get all polies within range.
////						//FVector CheckBoxCenter = ActorOrigin + (AIForwardVector * 200);
////						//FBox CheckBox;
////						//CheckBox.Max = CheckBoxCenter + FVector(100.f, 100.f, 100.f);
////						//CheckBox.Min = CheckBoxCenter - FVector(100.f, 100.f, 100.f);
////						//TArray<FNavPoly> CheckedPolys;
////
////						//if (bDebug)
////						//	DrawDebugBox(GetWorld(), CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Yellow, false, 0.1f);
////
////						//NavMesh->GetPolysInBox(CheckBox, CheckedPolys);
////						//NavNodeRef TargetPoly;
////
////						//if (CheckedPolys.Num() != 0)
////						//{
////						//	FVector ClosestPoint = FVector::ZeroVector;
////						//	FVector PolyPoint;
////
////						//	for (FNavPoly Poly : CheckedPolys)
////						//	{
////						//		NavMesh->GetPolyCenter(Poly.Ref, NearestPolyCompareLocation);
////						//		NavMesh->GetClosestPointOnPoly(Poly.Ref, ActorOrigin, PolyPoint);
////
////						//		if (NearestPolyCompareLocation != CurrentPolyCompareLocation)
////						//		{
////						//			if (ClosestPoint == FVector::ZeroVector
////						//				|| FVector::Dist(PolyPoint, ActorLoc) < FVector::Dist(ClosestPoint, ActorLoc))
////						//			{
////						//				FCollisionQueryParams TraceParams(TEXT(""), false, Character);
////						//				TraceParams.bTraceAsyncScene = true;
////						//				TraceParams.bTraceComplex = false;
////						//				TraceParams.bReturnPhysicalMaterial = false;
////						//				TraceParams.AddIgnoredActor(Character); //ignore self
////
////						//				FHitResult Trace;
////
////						//				FVector TraceStart;
////						//				TraceStart = PolyPoint;
////						//				TraceStart.Z += 10;
////						//				FVector TraceEnd = TraceStart;
////						//				TraceEnd.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5;
////
////						//				bool bCeilingTraceHit = World->LineTraceSingleByChannel(Trace, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
////						//				if (!bCeilingTraceHit)
////						//				{
////						//					TargetPoly = Poly.Ref;
////						//					ClosestPoint = PolyPoint;
////						//				}
////						//			}
////						//		}
////						//	}
////
////						//	if (ClosestPoint != FVector::ZeroVector)
////						//	{
////						//		//if our target is not zero, we have a point to jump to. 
////						//		if (bDebug)
////						//			DrawDebugPoint(World, ClosestPoint, 20, FColor::White, false, 20);
////
////						//		FVector JumpForce;
////						//		UGameplayStatics::SuggestProjectileVelocity_CustomArc(Character, JumpForce, ActorOrigin, ClosestPoint, 0.f, 0.3f);
////						//		JumpForce.Y *= 1.4;
////
////						//		if (JumpForce != FVector::ZeroVector)
////						//		{
////						//			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("CURRENT LAUNCH SPEED: %s"), *(JumpForce.ToString())));
////
////						//			Character->GetCharacterMovement()->Velocity = JumpForce;
////						//			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
////						//			AIController->StopMovement();
////						//			NewStartingLocationAfterJump = ClosestPoint;
////						//			bRequestedJump = true;
////						//		}
////						//		else
////						//		{
////						//			//Our jump force became 0, so abort the path
////						//			AIController->StopMovement();
////
////						//			if (bDebug)
////						//				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("No viable jump force on second jump check"));
////						//		}
////						//	}
////						//}
////
////						////if our character is indeed within range of the end of the poly, first do a simple check for polies in front and whether there is only a small gap between them. 
////						////this check is only for small holes, holes the AI actor wouldn't be able to fall in anyway, but would still stop path finding progress. 
////						//NearestPolyCompareLocation = GetCurrentTargetLocation() + (AIForwardVector * (Character->GetCapsuleComponent()->GetScaledCapsuleRadius() + Character->JumpCheckTolerance * 2));
////						//NavNodeRef NearestPoly = NavMesh->FindNearestPoly(NearestPolyCompareLocation, FVector(30.f, 30.f, 30.f));
////
////						//if (bDebug)
////						//	DrawDebugBox(World, NearestPolyCompareLocation, FVector(30.f, 30.f, 30.f), FQuat::Identity, FColor::Red, false, 0.1f);
////
////						//FVector NearestVert;
////						//NavMesh->GetClosestPointOnPoly(NearestPoly, ActorLoc, NearestVert);
////
////						//if (NearestVert != FVector::ZeroVector) //Nearest vert becomes a zero vector if invalid
////						//{
////						//	if (bDebug)
////						//		DrawDebugPoint(World, NearestVert, 20, FColor::White, false, 0.1f);
////
////						//	FVector JumpForce;
////						//	UGameplayStatics::SuggestProjectileVelocity_CustomArc(Character, JumpForce, ActorOrigin, NearestVert, 0.f, 0.6f);
////
////						//	if (JumpForce != FVector::ZeroVector)
////						//	{
////						//		Character->GetCharacterMovement()->Velocity = JumpForce;
////						//		Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
////
////						//		if (bDebug)
////						//			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, TEXT("We can do a small jump"));
////						//	}
////						//	else
////						//	{
////						//		//Our jump force became 0, so abort the path
////						//		AIController->StopMovement();
////
////						//		if (bDebug)
////						//			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("No viable jump force"));
////						//	}
////						//}
////					}
////				}
////				else
////				{
////					FollowPathSegmentNonJump(DeltaTime);
////				}
////			}
////		}
////	}
////
////	ACharacter* Character = AIController->GetCharacter();
////	if (Character
////		&& Character->GetCharacterMovement()->Velocity.Z == 0.f)
////	{
////		FollowPathSegmentNonJump(DeltaTime);
////	}
////}
////
//////AAIController* AIController = Cast<AAIController>(GetOwner());
//////if (AIController)
//////{
//////	ACharacter* Character = AIController->GetCharacter();
//////	if (Character != NULL
//////		&& Character->GetCharacterMovement()->Velocity.Z == 0.f)
//////	{
//////		Super::FollowPathSegment(DeltaTime);
//////	}
//////}
////}
//
////is called every tick to make an AI keep following the path
//void UPA_PathComponent_Human::FollowPathSegment(float DeltaTime)
//{
//	UWorld* World = GetWorld();
//	if (!World) return;
//
//	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
//	const ANavigationData* NavData = World->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
//	AAIController* AIController = Cast<AAIController>(GetOwner());
//	
//	if (AIController)
//	{
//		if (NavData
//			&& HasPartialPath()) //if the path is not partial (anymore), we have found a direct route to our target. If the path is partial, we have to jump somewhere to get there. 
//		{
//			const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
//			if (NavMesh)
//			{
//				APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
//
//				if (Character != NULL
//					&& Character->GetCharacterMovement()->Velocity.Z == 0.f) //if a jump is already happening, don't do anything.	
//				{
//					if (Character->bCanJump //make sure we only do this if the character has been set to be able to jump...
//						&& Character->MaxJumpZDistance > 0.f //...and actually has a max jump distance that is bigger than 0.
//						&& Character->GetCharacterMovement()->Velocity.Y != 0.f) //make sure we only evaluate Jump Pathing when character has Y speed
//					{
//						//first reset the horizontal displacement, to make sure we don't accidentally do something weird
//						//HorizontalVelocity = 0.f;
//
//						//Initialize some basic location vectors
//						FVector ClosestVert = FVector::ZeroVector;
//						FVector ActorLoc = Character->GetActorLocation();
//						FVector ActorOrigin = ActorLoc; //Get the actor location without the capsule height
//						ActorOrigin.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
//						ActorOrigin.Z += 10.f; //just add a little offset so we are not actually on the floor and our jump arc calculation doesn't get stuck.
//
//						FVector MoveGoal = GetMoveGoal() != NULL ? GetMoveGoal()->GetActorLocation() : OriginalMoveRequestGoalLocation;
//						FVector GoalDirection = MoveGoal - ActorLoc;
//						GoalDirection.Normalize();
//
//						////Use the target as the primary means of getting the forward vector, since we want the AI to moving towards the player. 
//						//FVector AIForwardVector = GoalDirection.Y > 0.f ? FVector(0.f, 1.f, 0.f) : FVector(0.f, -1.f, 0.f);
//
//						////First start checking whether we are in range of a wall
//						//if (FMath::Abs(GetCurrentTargetLocation().Y - Character->GetActorLocation().Y) < Character->WallCheckTolerance
//						//	&& FMath::Abs(GetCurrentTargetLocation().Y - Character->GetActorLocation().Y) >= Character->JumpCheckTolerance
//						//	&& !bRequestedWallTrace)
//						//{
//						//	UE_LOG(AnyLog, Log, TEXT("Checking for wall"));
//						//	bRequestedWallTrace = true;
//
//						//	if (CalculateWallJump(ActorOrigin, AIForwardVector, WallJumpForce, bDebug))
//						//	{
//						//		CurrentJumpAction = EJumpAction::JA_WALL;
//						//		AIController->StopMovement();
//						//	}
//						//}
//
//						//Next up we want to check whether our character is actually getting close on the current target location (the end point on the current nav poly)
//						/*else */
//						if (FMath::Abs(GetCurrentTargetLocation().Y - Character->GetActorLocation().Y) < Character->JumpCheckTolerance
//							&& CurrentJumpAction == EJumpAction::JA_NONE) //only process this when we have no wall traced
//						{
//							//New implementation; put jumping functionality within the tasks for control, and use the FollowPath to setup jump requests.
//							CurrentJumpAction = EJumpAction::JA_OTHER;
//							bMakeSmallJump = true;
//							AIController->StopMovement();
//						}
//					}
//					else
//					{
//						//FollowPathSegmentNonJump(DeltaTime);
//						Super::FollowPathSegment(DeltaTime);
//					}
//				}
//			}
//		}
//
//		ACharacter* Character = AIController->GetCharacter();
//		if (Character
//			&& Character->GetCharacterMovement()->Velocity.Z == 0.f)
//		{
//			//FollowPathSegmentNonJump(DeltaTime);
//			Super::FollowPathSegment(DeltaTime);
//		}
//	}
//}
//
//bool UPA_PathComponent_Human::GetClosestLocationInRange(FVector CompareExtents, FVector CompareLocation, FVector& Location)
//{
//	Location = FVector::ZeroVector;
//
//	if (CompareExtents == FVector::ZeroVector)
//		return false;
//
//	//Setup some basic variables
//	UWorld* World = GetWorld();
//	if (!World) return false;
//
//	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
//	const ANavigationData* NavData = World->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
//	if (!NavData) return false;
//	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
//	if (!NavMesh) return false;
//
//	NavNodeRef Poly = NavMesh->FindNearestPoly(CompareLocation, CompareExtents);
//	FVector CurrentPolyCompareLocation;
//	NavMesh->GetClosestPointOnPoly(Poly, CompareLocation, Location);
//
//	return true;
//}
//
////bool UPA_PathComponent_Human::CalculateWallJump(FVector StartLocation, FVector Direction, FVector& RequiredJumpForce, bool bDebugJump)
////{
////	//first see if we already set the velocity, so we don't have to redo the entire function
////	if (WallJumpForce != FVector::ZeroVector)
////	{
////		RequiredJumpForce = WallJumpForce;
////		return true;
////	}
////	else
////		RequiredJumpForce = FVector::ZeroVector;
////
////	//Setup some basic variables
////	UWorld* World = GetWorld();
////	if (!World) return false;
////
////	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
////	const ANavigationData* NavData = World->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
////	if (!NavData) return false;
////	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
////	if (!NavMesh) return false;
////	AAIController* AIController = Cast<AAIController>(GetOwner());
////	if (!AIController) return false;
////	APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
////	if (!Character) return false;
////
////	//Initialize our poly compare locations
////	FVector NearestPolyCompareLocation = FVector::ZeroVector;
////	NavNodeRef CurrentPoly = NavMesh->FindNearestPoly(StartLocation, FVector(20.f, 20.f, 20.f));
////	FVector CurrentPolyCompareLocation;
////	NavMesh->GetPolyCenter(CurrentPoly, CurrentPolyCompareLocation);		
////
////	FCollisionQueryParams TraceParams(TEXT(""), false, Character);
////	TraceParams.bTraceAsyncScene = true;
////	TraceParams.bTraceComplex = false;
////	TraceParams.bReturnPhysicalMaterial = false;
////	TraceParams.AddIgnoredActor(Character); //ignore self
////
////	FVector TraceStart, TraceEnd;
////	TraceStart = StartLocation;
////	TraceStart.Z += Character->MinJumpZDistance;
////	TraceEnd = TraceStart + (Direction * (Character->WallCheckTolerance * 2.f)); //increase wallcheck just a little so we can have our trace results earlier.
////
////	FHitResult Trace;
////	bool bFirstTraceHit = World->LineTraceSingleByChannel(Trace, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
////
////	//In case we get a hit on our initial trace, we have a wall and we can start getting our polies within jumping range. 
////	if (bFirstTraceHit)
////	{
////		if (bDebugJump)
////		{
////			DrawDebugLine(World, TraceStart, Trace.ImpactPoint, FColor::Red, false, 1.f);
////			DrawDebugLine(World, Trace.ImpactPoint, TraceEnd, FColor::Green, false, 1.f);
////			DrawDebugPoint(World, Trace.ImpactPoint, 20.f, FColor::Green, false, 1.f);
////		}
////
////		//if we have a hit, we need to get all polies within range.
////		FVector CheckBoxCenter = Trace.ImpactPoint;
////		CheckBoxCenter.Z += (Character->MaxJumpZDistance * 0.5) + 10.f;
////		FBox CheckBox;
////		CheckBox.Max = CheckBoxCenter + FVector(50.f, 50.f, Character->MaxJumpZDistance * 0.5);
////		CheckBox.Min = CheckBoxCenter - FVector(50.f, 50.f, Character->MaxJumpZDistance * 0.5);
////		TArray<FNavPoly> CheckedPolys;
////
////		if (bDebugJump)
////			DrawDebugBox(GetWorld(), CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Yellow, false, 0.1f);
////
////		NavMesh->GetPolysInBox(CheckBox, CheckedPolys);
////
////		if (CheckedPolys.Num() != 0)
////		{
////			//Do a trace to make sure we didn't get a poly within a static mesh. 
////			FVector PolyPoint;
////			FVector TargetPoint = FVector::ZeroVector; //The target point will save all potential targets.
////			for (FNavPoly Poly : CheckedPolys)
////			{
////				NavMesh->GetPolyCenter(Poly.Ref, NearestPolyCompareLocation);
////				NavMesh->GetClosestPointOnPoly(Poly.Ref, StartLocation, PolyPoint);
////				if (PolyPoint != FVector::ZeroVector
////					&& NearestPolyCompareLocation != CurrentPolyCompareLocation) //make sure we ignore the poly we are on at the moment.
////				{
////					//use largely the same trace distance we used in our first trace, with modified Z
////					TraceEnd = PolyPoint;
////					TraceEnd.Z += 10.f;
////					TraceStart.Z = TraceEnd.Z;
////
////					FVector CeilingTrace = TraceEnd;
////					CeilingTrace.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5; //do a bit more height than full height, since the jump arc itself also needs room.
////
////					bool bWallTraceHit = World->LineTraceSingleByChannel(Trace, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
////					if (!bWallTraceHit)
////					{
////						bool bCeilingTraceHit = World->LineTraceSingleByChannel(Trace, TraceEnd, CeilingTrace, ECC_Visibility, TraceParams);
////						if (!bCeilingTraceHit)
////						{
////							//If we don't get a hit on both the wall and ceiling trace, we have a reachable distance + enough space on the new poly.
////							TargetPoint = PolyPoint;
////							break;
////						}
////					}
////				}
////			}
////
////			if (TargetPoint != FVector::ZeroVector)
////			{
////				UGameplayStatics::SuggestProjectileVelocity_CustomArc(Character, RequiredJumpForce, StartLocation, TargetPoint, 0.f, 0.3f);
////				return true;
////			}
////		}
////	}
////	else
////	{
////		if (bDebugJump)
////			DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, 1.f);
////	}
////	
////	return false;
////}

bool UPA_PathComponent_Human::CalculateSmallJump(FVector CompareExtents, FVector& RequiredJumpForce, FVector JumpForceMultiplyer, bool bDebugJump)
{
	RequiredJumpForce = FVector::ZeroVector;
	bMakeSmallJump = false; //since we already called our function, we don't need this to be true anymore. 

	//Setup some basic variables
	UWorld* World = GetWorld();
	if (!World) return false;

	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSystem) return false;
	const ANavigationData* NavData = NavSystem->GetNavDataForProps(AgentProperties);
	if (!NavData) return false;
	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
	if (!NavMesh) return false;
	AAIController* AIController = Cast<AAIController>(GetOwner());
	if (!AIController) return false;
	APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
	if (!Character) return false;

	FVector ActorLoc = Character->GetActorLocation();
	FVector ActorOrigin = ActorLoc;
	ActorOrigin.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.f;

	//Initialize our poly compare locations
	FVector NearestPolyCompareLocation = FVector::ZeroVector;
	NavNodeRef CurrentPoly = NavMesh->FindNearestPoly(ActorOrigin, FVector(20.f, 20.f, 20.f));
	FVector CurrentPolyCompareLocation;
	NavMesh->GetPolyCenter(CurrentPoly, CurrentPolyCompareLocation);

	FVector MoveGoal = GetMoveGoal() != NULL ? GetMoveGoal()->GetActorLocation() : OriginalMoveRequestGoalLocation;
	FVector GoalDirection = MoveGoal - ActorLoc;
	//Use the target as the primary means of getting the forward vector, since we want the AI to moving towards the player. 
	FVector AIForwardVector = GoalDirection.Y > 0.f ? FVector(0.f, 1.f, 0.f) : FVector(0.f, -1.f, 0.f);

	//For small jump check we see if we have some small distance path close to us, so we can take that one instead. 
	FVector CheckBoxCenter = ActorOrigin + (AIForwardVector * (CompareExtents.Y * 2));
	FBox CheckBox;
	CheckBox.Max = CheckBoxCenter + CompareExtents;
	CheckBox.Min = CheckBoxCenter - CompareExtents;
	TArray<FNavPoly> CheckedPolys;
	
	if (bDebugJump)
		DrawDebugBox(GetWorld(), CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Yellow, false, 0.1f);
	
	NavMesh->GetPolysInBox(CheckBox, CheckedPolys);
	NavNodeRef TargetPoly;
	
	if (CheckedPolys.Num() != 0)
	{
		FVector ClosestPoint = FVector::ZeroVector;
		FVector PolyPoint;

		for (FNavPoly Poly : CheckedPolys)
		{
			NavMesh->GetPolyCenter(Poly.Ref, NearestPolyCompareLocation);
			NavMesh->GetClosestPointOnPoly(Poly.Ref, ActorOrigin, PolyPoint);

			if (NearestPolyCompareLocation != CurrentPolyCompareLocation)
			{
				if (ClosestPoint == FVector::ZeroVector
					|| FVector::Dist(PolyPoint, ActorLoc) < FVector::Dist(ClosestPoint, ActorLoc))
				{
					FCollisionQueryParams TraceParams(TEXT(""), false, Character);
					//TraceParams.bTraceAsyncScene = true;
					TraceParams.bTraceComplex = false;
					TraceParams.bReturnPhysicalMaterial = false;
					TraceParams.AddIgnoredActor(Character); //ignore self

					FHitResult Trace;

					FVector TraceStart;
					TraceStart = PolyPoint;
					TraceStart.Z += 10;
					FVector TraceEnd = TraceStart;
					TraceEnd.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5;

					bool bCeilingTraceHit = World->LineTraceSingleByChannel(Trace, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
					if (!bCeilingTraceHit)
					{
						TargetPoly = Poly.Ref;
						ClosestPoint = PolyPoint;
					}
				}
			}
		}

		if (ClosestPoint != FVector::ZeroVector)
		{
			//if our target is not zero, we have a point to jump to. 
			if (bDebugJump)
				DrawDebugPoint(World, ClosestPoint, 20, FColor::White, false, 20);

			FVector JumpForce;
			UGameplayStatics::SuggestProjectileVelocity_CustomArc(Character, JumpForce, ActorOrigin, ClosestPoint, 0.f, 0.3f);

			UE_LOG(AnyLog, Log, TEXT("CURRENT REQUESTED FORCE IS %s"), *(JumpForce.ToString()));
			//JumpForce.Y *= 1.4;

			if (JumpForce != FVector::ZeroVector)
			{
				RequiredJumpForce = JumpForce * JumpForceMultiplyer;
				return true;
			}
		}
	}

	return false;
}

////bool UPA_PathComponent_Human::CalculateLargeJump(float CompareXExtents, TArray<FVector>& PotentialLocations, bool bGetRandomPath, bool bDebugJump)
////{
////	//Setup some basic variables
////	UWorld* World = GetWorld();
////	if (!World) return false;
////
////	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
////	ANavigationData* NavData = World->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
////	if (!NavData) return false;
////	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
////	if (!NavMesh) return false;
////	AAIController* AIController = Cast<AAIController>(GetOwner());
////	if (!AIController) return false;
////	APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
////	if (!Character) return false;
////
////	UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(World);
////
////	FVector ActorLoc = Character->GetActorLocation();
////	FVector ActorOrigin = ActorLoc;
////	ActorOrigin.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.f;
////
////	//Initialize our poly compare locations
////	FVector NearestPolyCompareLocation = FVector::ZeroVector;
////	NavNodeRef CurrentPoly = NavMesh->FindNearestPoly(ActorOrigin, FVector(20.f, 20.f, 20.f));
////	FVector CurrentPolyCompareLocation;
////	NavMesh->GetPolyCenter(CurrentPoly, CurrentPolyCompareLocation);
////
////	FVector MoveGoal = GetMoveGoal() != NULL ? GetMoveGoal()->GetActorLocation() : OriginalMoveRequestGoalLocation;
////	FVector GoalDirection = MoveGoal - ActorLoc;
////	//Use the target as the primary means of getting the forward vector, since we want the AI to moving towards the player. 
////	FVector AIForwardVector = GoalDirection.Y > 0.f ? FVector(0.f, 1.f, 0.f) : FVector(0.f, -1.f, 0.f);
////
////	//For small jump check we see if we have some small distance path close to us, so we can take that one instead. 
////	FVector CheckBoxCenter = ActorOrigin + (AIForwardVector * (Character->MaxJumpYDistance * 0.5));
////	FBox CheckBox;
////	float MinZExtent = Character->bFallOnlyJumpZDistance ? Character->MaxJumpZDistance : Character->MaxFallZDistance;
////	CheckBox.Max = CheckBoxCenter + FVector(CompareXExtents, Character->MaxJumpYDistance * 0.5, Character->MaxJumpZDistance);
////	CheckBox.Min = CheckBoxCenter - FVector(CompareXExtents, Character->MaxJumpYDistance * 0.5, MinZExtent);
////	TArray<FNavPoly> CheckedPolys;
////
////	if (bDebugJump)
////		DrawDebugBox(GetWorld(), CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Yellow, false, 0.1f);
////
////	NavMesh->GetPolysInBox(CheckBox, CheckedPolys);
////	FVector ClosestVert;
////
////	if (CheckedPolys.Num() != 0 && MoveGoal != FVector::ZeroVector) //since our movegoal is important now too, we don't want any calculations on a zerovector
////	{
////		TArray<NavNodeRef> ViablePolies;
////		TArray<FVector> ViableTargetLocations;
////
////		for (int32 Index = 0; Index < CheckedPolys.Num(); Index++)
////		{
////			NavMesh->GetPolyCenter(CheckedPolys[Index].Ref, NearestPolyCompareLocation);
////
////			if (NearestPolyCompareLocation != CurrentPolyCompareLocation)
////			{	
////				bool bPolyAlreadyInArray = false;
////
////				for (NavNodeRef Poly : ViablePolies)
////				{
////					FVector CheckedPolyCenter;
////					NavMesh->GetPolyCenter(Poly, CheckedPolyCenter);
////					if (CheckedPolyCenter == NearestPolyCompareLocation)
////					{
////						bPolyAlreadyInArray = true;
////						break;
////					}
////				}
////
////				NavMesh->GetClosestPointOnPoly(CheckedPolys[Index].Ref, ActorLoc, ClosestVert);
////
////				if(bDebugJump)
////					DrawDebugPoint(World, ClosestVert, 20.f, FColor::White, false, 1.f);
////
////				ViableTargetLocations.Add(ClosestVert);
////			}
////		}
////
////		if (bGetRandomPath)
////		{
////			PotentialLocations.Add(ViableTargetLocations[FMath::RandRange(0, ViableTargetLocations.Num() - 1)]);
////		}
////		else
////		{
////			PotentialLocations = ViableTargetLocations;
////		}
////
////		return PotentialLocations.Num() != 0;
////	}
////
////	return false;
////}
//
//void UPA_PathComponent_Human::SetMoveSegment(int32 SegmentStartIndex)
//{
//	//else
//	//{
//		Super::SetMoveSegment(SegmentStartIndex);
//	//}
//
//
//		AAIController* AIController = Cast<AAIController>(GetOwner());
//		if (AIController)
//		{
//			APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
//
//			if (Character != NULL
//				&& Character->GetCharacterMovement()->Velocity.Z == 0.f)
//			{
//				//reset any jump trace variables, so we can start the segment with a clean slate
//				bMakeSmallJump = false; 
//				//bRequestedWallTrace = false;
//				bRequestedJump = false;
//				//WallJumpForce = FVector::ZeroVector;
//				CurrentJumpAction = EJumpAction::JA_NONE;
//			}
//		}
//
//	//const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
//	//const ANavigationData* NavData = GetWorld()->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
//
//	//if (NavData
//	//	&& HasPartialPath()) //if the path is not partial (anymore), we have found a direct route to our target. If the path is partial, we have to jump somewhere to get there. 
//	//{
//	//	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
//	//	if (NavMesh)
//	//	{
//	//		AAIController* AIController = Cast<AAIController>(GetOwner());
//	//		if (AIController)
//	//		{
//	//			APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
//
//	//			if (Character != NULL
//	//				&& Character->GetCharacterMovement()->Velocity.Z == 0.f
//	//				&& Character->GetCharacterMovement()->Velocity.Y != 0.f) //make sure we only evaluate Jump Pathing when character has Y speed
//	//			{
//	//				if (Character->bCanJump) //make sure we only do this if the character has been set to be able to jump
//	//				{
//	//					//UE_LOG(AnyLog, Log, TEXT("CURRENTLY EXECUTING SETMOVESEGMENT"));
//
//	//					//get a "forward" vector that determines whether the character is walking left or right while ignoring rotation
//	//					FVector AIForwardVector = Character->GetCharacterMovement()->Velocity.Y > 0.f ? FVector(0.f, 1.f, 0.f) : FVector(0.f, -1.f, 0.f);
//
//	//					//first reset the horizontal displacement, to make sure we don't accidentally do something weird
//	//					HorizontalVelocity = 0.f;
//
//	//					//Initialize some vectors
//	//					FVector ClosestVert = FVector::ZeroVector;
//	//					FVector ActorLoc = Character->GetActorLocation();
//	//					FVector ActorOrigin = ActorLoc; //Get the actor location without the capsule height
//	//					ActorOrigin.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
//
//
//	//					//Set the capsule collision so we can get any potential wall collision. Use world instead of relative, so it only faces forward or backward. 
//	//					Character->GetFrontalCollisionCapsuleComponent()->SetWorldLocationAndRotation(ActorLoc + (AIForwardVector * FrontalCheckDistance), FQuat::Identity);
//	//					//get wall collision early so we can modify the range for where we check for nearest poly.
//	//					bool bIsWall = CollisionOverlapsStaticPrimitive(Character->GetFrontalCollisionCapsuleComponent());
//
//	//					FVector NearestPolyCompareLocation = ActorLoc + (AIForwardVector * FrontalCheckDistance);
//	//					if (bIsWall) NearestPolyCompareLocation.Z += MaxJumpHeight + 10.f;
//	//					FVector NearestPolyCompareExtent = bIsWall ? FVector(350.f, 50.f, MaxJumpHeight) : FVector(350.f, 50.f, MaxFallHeight);
//
//
//	//					//do a wall check here, otherwise you will get weird polies.
//	//					NavNodeRef NearestPoly = NavMesh->FindNearestPoly(NearestPolyCompareLocation, NearestPolyCompareExtent);
//	//					NavNodeRef CurrentPoly = NavMesh->FindNearestPoly(Character->GetActorLocation(), FVector(50.f, 50.f, 250.f));
//
//	//					FVector CurrentCenter, NearestCenter;
//	//					NavMesh->GetPolyCenter(NearestPoly, NearestCenter);
//	//					NavMesh->GetPolyCenter(CurrentPoly, CurrentCenter);
//
//	//					FVector MoveGoal = FVector::ZeroVector; //was originally an actor, but that would mean we cannot use this unless the MoveTo AI action has an actor as target.
//	//					if (GetMoveGoal())
//	//					{
//	//						MoveGoal = GetMoveGoal()->GetActorLocation();
//	//					}
//	//					else
//	//					{
//	//						MoveGoal = OriginalMoveRequestGoalLocation; //Since Path->GetDestinationLocation returns the current destination based on the partial path, not the actual end.
//	//					}
//
//	//					NavMesh->GetClosestPointOnPoly(NearestPoly, ActorLoc, ClosestVert);
//
//	//					if (bDebug)
//	//						DrawDebugPoint(GetWorld(), ClosestVert, 20.f, FColor::Purple, false, 10.f);
//
//	//					/*
//	//					THE REASON THIS CURRENTLY DOESNT WORK IS THAT NAVMESH BUILDS NAVMESHES BASED ON THE ANGLE OF COLLISIONS, EVEN IF SAID NAVMESHES COULD TECHNICALLY NEVER BE REACHED DUE TO 
//	//					THE MESH IN QUESTION BEING UNDER A WALL. 
//
//	//					THIS CAUSES THE CURRENT SYSTEM TO ACTUALLY FIND A VERTEX WITHIN A WALL, AND BE SATISFIED BECAUSE IT IS TECHNICAllY THE CLOSEST. WE NEED TO USE COLLISION WITHIN THE CHARACTER
//	//					TO DETERMINE A WALL FIRST, AND MODIFY OUR POLY SEARCHING BASED ON THAT. IN SHORT, OUR POLYSEARCH SYSTEM IS TOO SIMPLE AND CAN ONLY ACCURATELY WORK WHEN THERE AREN'T ANY WALLS. 
//	//					
//	//					*/
//
//	//					ClosestVert.X = ActorLoc.X; //Make sure we reset the X to our character, so we don't move the character off the X axis.
//
//	//					bool bInJumpingRange = FMath::Abs(Path->GetDestinationLocation().Y - Character->GetActorLocation().Y) < JumpStartTolerance || bIsWall;
//
//	//					//Character->GetFrontalCollisionCapsuleComponent()->SetRelativeLocation(FVector::ZeroVector);
//
//	//					//TEMPORARY DISABLE. FIRST GET THE POLYCHECK WORKING
//	//					//if (Path->GetDestinationLocation() != INVALID_NAVEXTENT
//	//					//	&& bInJumpingRange)
//	//					//{
//	//					//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TRYING TO START A JUMP"));
//
//	//					//	//first let's do a quick check for simple jumping, to save on processing.
//	//					//	//WE NEED TO DO A PRECHECK FOR WALLS OR NOT, SO WE CAN ALTER THE VERT WE ARE TRYING TO GET.
//	//					//	if (ClosestVert.Z - ActorOrigin.Z <= MaxJumpHeight
//	//					//		&& ClosestVert.Z - ActorOrigin.Z > FloorHeightTolerance
//	//					//		&& CurrentCenter != NearestCenter)
//	//					//	{
//	//					//		//OLD IMPLEMENTATION, USE JUMPFORCE NOW
//	//					//		////First let's check whether the nearest poly is not equal to our current poly. 
//	//					//		//FVector DestDir = FVector(0.f, AIForwardVector, 0.f);
//
//	//					//		//FVector LaunchPower = DestDir * (FMath::Abs(ActorLoc.Y - ClosestVert.Y) * 2.f);
//	//					//		//HorizontalVelocity = LaunchPower.Y;
//	//					//		//float LedgeHeight = FMath::Abs(ActorLoc.Z - ClosestVert.Z) + 25.f;
//
//	//					//		////the X = GravityScale * (square root of 2000 Y) is an approximation of max jump height in UE4. 
//	//					//		////X = velocity
//	//					//		////Y = Ledge height
//	//					//		//LaunchPower.Z = (FMath::Sqrt(2000.f * LedgeHeight)) * Character->GetCharacterMovement()->GravityScale;
//
//	//					//		FVector LaunchPower = GetNecessaryJumpForce(ActorLoc, ClosestVert);
//
//	//					//		if (LaunchPower != FVector::ZeroVector)
//	//					//		{
//	//					//			Character->GetCharacterMovement()->Velocity = LaunchPower;
//	//					//			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
//	//					//		}
//	//					//		else
//	//					//		{
//	//					//			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("COULDN'T FIND A VIABLE JUMP FORCE"));
//	//					//		}
//
//	//					//		return;
//	//					//	}
//	//					//	//If the simple jumping doesn't return what we want, we can become more complicated
//	//					//	else if (CurrentCenter == NearestCenter)
//	//					//	{
//	//					//		//PauseMove();
//
//	//					//		float CheckBoxExtent = Character->GetCharacterMovement()->MaxWalkSpeed;
//	//					//		FVector CheckBoxCenter = ActorOrigin + Character->GetActorForwardVector() * CheckBoxExtent;
//	//					//		FBox CheckBox;
//	//					//		CheckBox.Max = CheckBoxCenter + FVector(175.f, CheckBoxExtent * 0.5, MaxJumpHeight);
//	//					//		CheckBox.Min = CheckBoxCenter - FVector(175.f, CheckBoxExtent * 0.5, bCanFallOnlyReachableDistances ? MaxJumpHeight : MaxFallHeight);
//	//					//		TArray<FNavPoly> CheckedPolys;
//
//	//					//		if (bDebug)
//	//					//			DrawDebugBox(GetWorld(), CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Yellow, false, 0.1f);
//
//	//					//		NavMesh->GetPolysInBox(CheckBox, CheckedPolys);
//
//	//					//		if (CheckedPolys.Num() != 0 && MoveGoal != FVector::ZeroVector)
//	//					//		{
//	//					//			float ShortestPath = 0.f;
//	//					//			int32 ViablePathIndex = INDEX_NONE;
//
//	//					//			for (int32 Index = 0; Index < CheckedPolys.Num(); Index++)
//	//					//			{
//	//					//				float PathLength;
//
//	//					//				NavMesh->GetClosestPointOnPoly(CheckedPolys[Index].Ref, ActorLoc, ClosestVert);
//	//					//				ENavigationQueryResult::Type QueryResult = NavMesh->CalcPathLength(ClosestVert, MoveGoal, PathLength);
//
//	//					//				FVector PolyCenter;
//	//					//				NavMesh->GetPolyCenter(CheckedPolys[Index].Ref, PolyCenter);
//
//	//					//				if (bDebug)
//	//					//					DrawDebugPoint(GetWorld(), PolyCenter, 20.f, QueryResult == ENavigationQueryResult::Success ? FColor::Yellow : FColor::Red, false, 0.1f);
//
//	//					//				if (QueryResult == ENavigationQueryResult::Success && PathLength <= CheckBoxExtent + JumpStartTolerance * 2.f)
//	//					//				{
//	//					//					if (PathLength < ShortestPath || ShortestPath == 0.f)
//	//					//					{
//	//					//						ShortestPath = PathLength;
//	//					//						ViablePathIndex = Index;
//	//					//					}
//	//					//				}
//	//					//			}
//
//	//					//			if (ViablePathIndex != INDEX_NONE)
//	//					//			{
//	//					//				//For now, let's ignore distance and stuff.
//	//					//				NavMesh->GetClosestPointOnPoly(CheckedPolys[ViablePathIndex].Ref, ActorLoc, ClosestVert);
//
//	//					//				//OLD IMPLEMENTATION, USE JUMPFORCE NOW
//	//					//				//float JumpDistance = FVector::Dist(ActorOrigin, ClosestVert);
//	//					//				//FVector LaunchPower = FVector(0.f, JumpDistance * AIForwardVector, 0.f);
//	//					//				//HorizontalVelocity = LaunchPower.Y;
//
//	//					//				//if (ClosestVert.Z < ActorOrigin.Z)
//	//					//				//{
//	//					//				//	UE_LOG(AnyLog, Log, TEXT("CURRENT JUMP DISTANCE IS %f"), JumpDistance);
//
//	//					//				//	float ZLaunch = JumpDistance / (CheckBoxExtent + JumpStartTolerance * 2.f);
//	//					//				//	ZLaunch *= (MaxJumpHeight * 0.25);
//	//					//				//	LaunchPower.Z = (FMath::Sqrt(2000.f * ZLaunch)) * Character->GetCharacterMovement()->GravityScale;
//	//					//				//}
//	//					//				//else
//	//					//				//{
//	//					//				//	//TODO: don't make the jump hardcoded. 
//	//					//				//	LaunchPower.Z = (FMath::Sqrt(2000.f * 125.f)) * Character->GetCharacterMovement()->GravityScale;
//	//					//				//}
//
//	//					//				FVector LaunchPower = GetNecessaryJumpForce(ActorLoc, ClosestVert);
//
//	//					//				if (LaunchPower != FVector::ZeroVector)
//	//					//				{
//	//					//					Character->GetCharacterMovement()->Velocity = LaunchPower;
//	//					//					Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
//	//					//				}
//
//	//					//				return;
//	//					//			}
//	//					//		}
//	//					//	}
//	//					//}
//	//				}
//
//	//				Super::SetMoveSegment(SegmentStartIndex);
//	//			}
//	//			else
//	//			{
//	//				//If we are falling, making sure the AI pawn is not able to slow down horizontally in the middle of the jump. 
//	//				if (HorizontalVelocity != 0.f)
//	//					Character->GetCharacterMovement()->Velocity.Y = HorizontalVelocity;
//	//			}
//	//		}
//	//	}
//	//}
//	//else
//	//	Super::SetMoveSegment(SegmentStartIndex);
//
//	//TEMP
//	//Super::SetMoveSegment(SegmentStartIndex);
//}
//
//void UPA_PathComponent_Human::UpdatePathSegment()
//{
//	AAIController* AIController = Cast<AAIController>(GetOwner());
//	if (AIController)
//	{
//		ACharacter* Character = AIController->GetCharacter();
//		if (Character != NULL
//			&& Character->GetCharacterMovement()->Velocity.Z == 0.f)
//		{
//			Super::UpdatePathSegment();
//		}
//	}
//}
//
////bool UPA_PathComponent_Human::CollisionOverlapsStaticPrimitive(class UCapsuleComponent* CollisionComponent)
////{
////	if (!CollisionComponent)
////		return false;
////
////	TArray<UPrimitiveComponent*> Comps;
////	CollisionComponent->GetOverlappingComponents(Comps);
////
////	if (Comps.Num() != 0)
////	{
////		for (UPrimitiveComponent* Comp : Comps)
////		{
////			if (Cast<UStaticMeshComponent>(Comp))
////				return true;
////		}
////	}
////
////	return false;
////}
//
////FVector UPA_PathComponent_Human::GetNecessaryJumpForce(FVector StartLocation, FVector EndLocation, float MinimumArc/* = 3.f*/, float MaximumArc/* = 7.f*/, int32 EvaluationSteps/* = 8*/)
////{
////	FVector JumpForce = FVector::ZeroVector;
////
////	//NEW IMPLEMENTATION; we will simply assume that only height is necessary and build the force from there. No max force anymore, just height. 
////	
////	//AAIController* AIController = Cast<AAIController>(GetOwner());
////	//if (AIController)
////	//{
////	//	APA_AI_CharacterBase* Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
////
////	//	if (EvaluationSteps != 0 && Character)
////	//	{
////	//		float ArcDelta = MaximumArc - MinimumArc;
////	//		float ArcStep = ArcDelta / (float)EvaluationSteps;
////	//		float MaxZ = Character->MaxJumpZ;
////	//		float MaxY = Character->MaxJumpY;
////	//		float CurrentArc;
////
////	//		for (int32 Index = 0; Index < EvaluationSteps; Index++)
////	//		{ 
////	//			//make sure we start calculating from the minimum arc, since lower arc value actually means higher apex. 
////	//			//we really only want high arcs for high distances, and going from low ensures we only get a high arc if there is nothing else.
////	//			CurrentArc = MinimumArc + (Index * ArcStep);
////	//			UGameplayStatics::SuggestProjectileVelocity_CustomArc(Character, JumpForce, StartLocation, EndLocation, 0.f, MaximumArc);
////	//			if (JumpForce.Z <= MaxZ && FMath::Abs(JumpForce.Y) <= MaxY)
////	//				break;
////
////	//			JumpForce = FVector::ZeroVector;
////	//		}
////	//	}
////	//}
////
////	return JumpForce;
////}
//
////void UPA_PathComponent_Human::SetMovementComponent(UNavMovementComponent* MoveComp)
////{
////	Super::SetMovementComponent(MoveComp);
////
////	CharacterMoveComponent = Cast<UCharacterMovementComponent>(MovementComp);
////}
//
//bool UPA_PathComponent_Human::NavPoly_GetAllPolys(TArray<NavNodeRef>& Polys)
//{
//	if (!CharacterMoveComponent) return false;
//
//	const ANavigationData* NavData = PAGetNavData();
//
//	if (!NavData) return false;
//
//	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
//
//	if (!NavMesh) return false;
//
//	TArray<FNavPoly> EachPolys;
//
//	for (int32 Index = 0; Index < NavMesh->GetNavMeshTilesCount(); Index++)
//	{
//		if (!TileIsValid(NavMesh, Index))
//		{
//			continue;
//		}
//
//		NavMesh->GetPolysInTile(Index, EachPolys);
//	}
//
//	for (int32 Index = 0; Index < EachPolys.Num(); Index++)
//	{
//		Polys.Add(EachPolys[Index].Ref);
//	}
//
//	return true;
//}
//
//NavNodeRef UPA_PathComponent_Human::NavPoly_FindNearest(const FVector& Loc, const FVector& Extent)
//{
//	if (!MovementComp) return false;
//
//	const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
//
//	const ANavigationData* NavData = GetWorld()->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
//
//	if (NavData == NULL)
//	{
//		NavData = GetWorld()->GetNavigationSystem()->GetMainNavData();
//	}
//
//	const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
//	if (!NavMesh)
//	{
//		UE_LOG(AnyLog, Error, TEXT("Cast to recast nav mesh failed"));
//		return false;
//	}
//
//	return NavMesh->FindNearestPoly(Loc, Extent, NavData->GetDefaultQueryFilter());
//}
//
////FVector UPA_PathComponent_Human::GetCurrentDirection() const
////{
////	AAIController* AIController = Cast<AAIController>(GetOwner());
////	APA_AI_CharacterBase* Character = NULL;
////	if(AIController)
////		Character = Cast<APA_AI_CharacterBase>(AIController->GetCharacter());
////
////	FVector MoveGoal = GetMoveGoal() != NULL ? GetMoveGoal()->GetActorLocation() : OriginalMoveRequestGoalLocation;
////	if (MoveGoal == FVector::ZeroVector
////		|| Character == NULL)
////	{
////		if (CurrentDestination.Base)
////		{
////			// calculate direction to based destination
////			const FVector SegmentStartLocation = *Path->GetPathPointLocation(MoveSegmentStartIndex);
////			const FVector SegmentEndLocation = *CurrentDestination;
////
////			return (SegmentEndLocation - SegmentStartLocation).GetSafeNormal();
////		}	
////		else
////			// use cached direction of current path segment
////			return MoveSegmentDirection;
////	}
////	else
////	{ 
////		FVector GoalDirection = MoveGoal - Character->GetActorLocation();
////		return GoalDirection.Y > 0.f ? FVector(0.f, 1.f, 0.f) : FVector(0.f, -1.f, 0.f);
////	}
////
////	// use cached direction of current path segment
////	return MoveSegmentDirection;
////}