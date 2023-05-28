// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Tool_Flintlock.h"
#include "PiratesAdventure.h"
#include "PA_Tool_Bullet.h"

UPA_Tool_Flintlock::UPA_Tool_Flintlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bToolHasAiming = true;
	bToolIsChargeBased = false;
	ToolType = EPlayerToolType::TOOL_GUN;
}

void UPA_Tool_Flintlock::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPA_Tool_Flintlock::StartAimTool(float X, float Y)
{
	bToolIsActive = true;
	bToolIsAiming = true;

	ReceiveStartAimTool(X, Y);
}

void UPA_Tool_Flintlock::StopAimTool()
{
	ReceiveStopAimTool();
	bToolIsActive = false;
	bToolIsAiming = false;
}

void UPA_Tool_Flintlock::StartTool()
{
	Super::StartTool();
}

void UPA_Tool_Flintlock::StopTool()
{
	Super::StopTool();
}