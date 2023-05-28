// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_Base.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"

// Sets default values for this component's properties
UPA_PlayerState_Base::UPA_PlayerState_Base()
{
	bWantsInitializeComponent = true;

	StateCooldownDuration = 0.f;
}

// Initializes the component, allowing us to register it with the player
void UPA_PlayerState_Base::InitializeComponent()
{
	Super::InitializeComponent();

	SetOwner(Cast<APiratesAdventureCharacter>(GetOwner()));
	if (!StateOwner)
	{
		UE_LOG(AnyLog, Error, TEXT("Couldn't register %s component with the player"), *(GetClass()->GetName()));
	}
	else
		StateOwner->RegisterStateComponent(this);
}

// Called when the game starts
void UPA_PlayerState_Base::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UPA_PlayerState_Base::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

//"Opens" the state every time it is called.
void UPA_PlayerState_Base::BeginState()
{

}

//"Closes" the state every time the state is switched and is responsible for the actual switching.
void UPA_PlayerState_Base::EndState(EPlayerStateType NewState)
{
	StateOwner->SwitchPlayerState(NewState);

	if (StateCooldownDuration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(StateCooldownHandle, this, &UPA_PlayerState_Base::StopStateCooldown, StateCooldownDuration, false);
	}
}

void UPA_PlayerState_Base::StopStateCooldown()
{
	//doesn't need content.
}

bool UPA_PlayerState_Base::CanEnterState()
{
	if (StateCooldownDuration <= 0.f)
		return true;

	return !GetWorld()->GetTimerManager().IsTimerActive(StateCooldownHandle);
}