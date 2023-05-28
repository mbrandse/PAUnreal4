// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PA_PlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_PlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public: 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayerStart")
		int32 PlayerStartID;
	
	
};
