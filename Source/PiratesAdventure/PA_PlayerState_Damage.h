// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "PA_PlayerState_Base.h"
#include "PA_PlayerState_Damage.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (PlayerState), meta = (BlueprintSpawnableComponent, DisplayName = "Player State - Damage"))
class PIRATESADVENTURE_API UPA_PlayerState_Damage : public UPA_PlayerState_Base
{
	GENERATED_BODY()

public:

	UPA_PlayerState_Damage();

	// Called when the game starts
	virtual void InitializeComponent() override;

	// State functions
	virtual void BeginState();
	virtual void EndState(EPlayerStateType NewState);	
};
