// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PA_JumpBoid_MoveComp.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_JumpBoid_MoveComp : public UFloatingPawnMovement
{
	GENERATED_BODY()

public: 

	UPA_JumpBoid_MoveComp();

	void PostBoidBeginPlay();

protected:

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	   
	UPROPERTY()
		class APA_JumpBoid_Boid* Owner;
};
