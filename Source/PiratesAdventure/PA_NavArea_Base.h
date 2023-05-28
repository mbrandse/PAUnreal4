// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NavAreas/NavArea.h"
#include "NavMesh/RecastNavMesh.h"
#include "PA_NavArea_Base.generated.h"

/**
 * 
 */
UENUM()
namespace ENavAreaFlag
{
	enum Type
	{
		Default,
		Jump,
		Crouch, 
		Climb, 
		Shimmy
	};
}

namespace FNavAreaHelper
{
	FORCEINLINE bool IsSet(uint16 Flags, ENavAreaFlag::Type Bit) { return (Flags & (1 << Bit)) != 0; }
	FORCEINLINE void Set(uint16 Flags, ENavAreaFlag::Type Bit) { Flags |= (1 << Bit); }

	FORCEINLINE bool IsNavLink(const FNavPathPoint& PathVert) { return (FNavMeshNodeFlags(PathVert.Flags).PathFlags & RECAST_STRAIGHTPATH_OFFMESH_CONNECTION) != 0; }
	FORCEINLINE bool HasJumpFlag(const FNavPathPoint& PathVert) { return IsSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, ENavAreaFlag::Jump); }
	FORCEINLINE bool HasCrouchFlag(const FNavPathPoint& PathVert) { return IsSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, ENavAreaFlag::Crouch); }
	FORCEINLINE bool HasClimbFlag(const FNavPathPoint& PathVert) { return IsSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, ENavAreaFlag::Climb); }
	FORCEINLINE bool HasShimmyFlag(const FNavPathPoint& PathVert) { return IsSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, ENavAreaFlag::Shimmy); }
}

UCLASS()
class PIRATESADVENTURE_API UPA_NavArea_Base : public UNavArea
{
	GENERATED_BODY()
	
	
	
	
};
