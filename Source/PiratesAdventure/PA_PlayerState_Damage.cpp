// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.


#include "PA_PlayerState_Damage.h"

UPA_PlayerState_Damage::UPA_PlayerState_Damage()
{
	StateID = EPlayerStateType::GPS_GameDamage;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPA_PlayerState_Damage::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPA_PlayerState_Damage::BeginState()
{

}

void UPA_PlayerState_Damage::EndState(EPlayerStateType NewState)
{
	Super::EndState(NewState);
}