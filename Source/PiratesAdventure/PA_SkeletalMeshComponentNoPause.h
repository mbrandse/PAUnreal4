// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

// A skeletal mesh component which will continue playing even when the game is paused. 

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PA_SkeletalMeshComponentNoPause.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent, DisplayName = "Skeletal Mesh Component No Pause"))
class PIRATESADVENTURE_API UPA_SkeletalMeshComponentNoPause : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:

	//always return true, so we can tick animation even when the game is paused
	bool ShouldTickPose() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void ForceTickAnimation(float DeltaTime, bool bNeedsValidRootMotion, bool bDirectCall = false, bool bCallPose = false);
};
