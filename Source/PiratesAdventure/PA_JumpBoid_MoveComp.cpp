// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.


#include "PA_JumpBoid_MoveComp.h"
#include "PA_JumpBoid_Boid.h"

UPA_JumpBoid_MoveComp::UPA_JumpBoid_MoveComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPA_JumpBoid_MoveComp::PostBoidBeginPlay()
{
	Owner = Cast<APA_JumpBoid_Boid>(GetOwner());
}

void UPA_JumpBoid_MoveComp::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}