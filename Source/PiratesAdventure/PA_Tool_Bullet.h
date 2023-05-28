// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_Tool_Bullet.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_Tool_Bullet : public AActor
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Bullet")
		void OnEnterVolume(class AActor* VolumeEntered);
	
	
};
