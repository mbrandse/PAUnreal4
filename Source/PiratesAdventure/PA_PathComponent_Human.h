// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/PathFollowingComponent.h"
//#include "NavMesh/RecastNavMesh.h"
//#include "NavigationSystem.h"
//#include "NavigationData.h"
//#include "AI/Navigation/NavigationTypes.h"
#include "PA_PathComponent_Human.generated.h"

//used to determine what kind of jump we should consider.
UENUM(BlueprintType)
enum class EJumpAction : uint8
{
	//Nothing could be found to warrant a jump action
	JA_NONE UMETA(DisplayName = "No Jump Action"), 
	//Found a wall, so jump like it's a wall
	JA_WALL UMETA(DisplayName = "Wall Jump Action"), 
	//Needs additional calculation to determine what to do
	JA_OTHER UMETA(DisplayName = "Other Jump Action"),
};

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_PathComponent_Human : public UPathFollowingComponent
{
	GENERATED_BODY()

protected:

	class UCharacterMovementComponent* CharacterMoveComponent;

public:

	UPA_PathComponent_Human();

	////follow current path segment
	//virtual void FollowPathSegment(float DeltaTime) override;

	//////a slightly customized version of the original FollowPathSegment, so we can implement our own velocity (to use instead of calling Super on the FollowPathSegment
	////void FollowPathSegmentNonJump(float DeltaTime);

	////used to detect properties of a path's segment a character is about the follow
	//virtual void SetMoveSegment(int32 SegmentStartIndex) override;

	////check state of path following, update segment if needed
	//virtual void UpdatePathSegment() override;

	//////set character movement component
	////virtual void SetMovementComponent(UNavMovementComponent* MoveComp) override;

	//////How far ahead the AI will check for new accessible areas.
	////UPROPERTY(EditAnywhere, Category = "Pathfinding")
	////	float FrontalCheckDistance;

	//////How much difference in Z Value is still considered part of the floor, so that the AI won't jump over small bumps
	////UPROPERTY(EditAnywhere, Category = "Pathfinding")
	////	float FloorHeightTolerance;

	//////The maximum ledge height an AI can jump. Expressed in Z distance from current position. 
	////UPROPERTY(EditAnywhere, Category = "Pathfinding")
	////	float MaxJumpHeight;

	//////The distance from the ledge where the AI will start the jump. Don't make this value too low, since it may prevent the AI from jumping altogether. 
	////UPROPERTY(EditAnywhere, Category = "Pathfinding")
	////	float JumpStartTolerance;

	//////If true, the AI can only fall down ledges that are not higher than MaxJumpHeight. Otherwise, it will use MaxFallHeight for detection. 
	////UPROPERTY(EditAnywhere, Category = "Pathfinding")
	////	bool bCanFallOnlyReachableDistances;

	//////The maximum distance an AI will drop down to out of own accord. Expressed in Z distance from current position. 
	////UPROPERTY(EditAnywhere, Category = "Pathfinding")
	////	float MaxFallHeight;

	//////HorizontalVelocity is to make sure the AI will maintain steady horizontal velocity for as long as it's falling, to eliminate weird jumps.
	////UPROPERTY()
	////	float HorizontalVelocity = 0.f;

	////If true, draws debug primitives to indicate the paths that have been found by the AI. 
	//UPROPERTY(EditAnywhere, Category = "Pathfinding")
	//	bool bDebug;

	////bool CollisionOverlapsStaticPrimitive(class UCapsuleComponent* CollisionComponent);

	////FVector NewStartingLocationAfterJump;

	////This is to make sure we only make a single wall trace for every segment, since if the initial trace returns false, we can safely assume there simply is no wall. 
	////bool bRequestedWallTrace;
	//bool bRequestedJump; 

	////FVector GetNecessaryJumpForce(FVector StartLocation, FVector EndLocation, float MinimumArc = 3.f, float MaximumArc = 7.f, int32 EvaluationSteps = 8);

	////FVector GetCurrentDirection() const;

	//UPROPERTY(BlueprintReadOnly, Category = "Pathfinding")
	//	EJumpAction CurrentJumpAction;

	UPROPERTY(BlueprintReadOnly, Category = "Pathfinding")
		bool bMakeSmallJump;

	////UFUNCTION(BlueprintCallable, Category = "PathFinding")
	////	bool CalculateWallJump(FVector StartLocation, FVector Direction, FVector& RequiredJumpForce, bool bDebugJump = false);

	////UPROPERTY()
	////	FVector WallJumpForce;

	//Calculate small jump only checks right in front of the character to see if we have any small distance jumps that can be made. It's fairly cheap.
	UFUNCTION(BlueprintCallable, Category = "PathFinding")
		bool CalculateSmallJump(FVector CompareExtents, FVector& RequiredJumpForce, FVector JumpForceMultiplyer = FVector(1.f, 1.f, 1.f), bool bDebugJump = false);

	///*Calculate large jump calculates all possible jump paths for the current jump, and then returns the one with the lowest path cost. Expensive, only use when no other jumps are possible.
	//* CompareXExtents - for a large check, the Y and Z are already determined. We only need X; 
	//* bGetRandomPath - If true, we ignore path cost and just take a random path out of the paths deemed viable (better for performance, but less accurate possibly because its random).
	//*/
	////UFUNCTION(BlueprintCallable, Category = "PathFinding")
	////	bool CalculateLargeJump(float CompareXExtents, TArray<FVector>& PotentialLocations, bool bGetRandomPath, bool bDebugJump = false);

	//UFUNCTION(BlueprintCallable, Category = "PathFinding")
	//	bool GetClosestLocationInRange(FVector CompareExtents, FVector CompareLocation, FVector& Location);

//	//
//	//RAMA navigation functions - https://wiki.unrealengine.com/AI_Navigation_in_C%2B%2B,_Customize_Path_Following_Every_Tick
//	//
//protected:
//
//	FORCEINLINE const ANavigationData* GetMainNavData(FNavigationSystem::ECreateIfEmpty CreateNewIfNoneFound)
//	{
//		UNavigationSystem* NavSys = Cast<UNavigationSystem>(GetWorld()->GetNavigationSystem());
//		if (!NavSys) return NULL;
//		return NavSys->GetMainNavData(CreateNewIfNoneFound);
//	}
//
//	//Choose Which Nav Data To Use
//	FORCEINLINE const ANavigationData* PAGetNavData() const
//	{
//		if (!MovementComp)
//		{
//			return NULL;
//			//GetMainNavData();
//		}
//
//		const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
//		const ANavigationData* NavData = GetWorld()->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
//		if (NavData == NULL)
//		{
//			//VSCREENMSG("ERROR USING MAIN NAV DATA");
//			return NULL;
//			//NavData = GetMainNavData();
//		}
//
//		return NavData;
//	}
//
//	FORCEINLINE bool TileIsValid(const ARecastNavMesh* NavMesh, int32 TileIndex) const
//	{
//		if (!NavMesh) return false;
//
//		const FBox TileBounds = NavMesh->GetNavMeshTileBounds(TileIndex);
//
//		return TileBounds.IsValid != 0;
//	}
//
//	bool NavPoly_GetAllPolys(TArray<NavNodeRef>& Polys);
//
//	NavNodeRef NavPoly_FindNearest(const FVector& Loc, const FVector& Extent);	
};
