// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Tool_Grenade.h"
#include "PiratesAdventure.h"
#include "PA_Tool_Bullet.h"

UPA_Tool_Grenade::UPA_Tool_Grenade(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bToolHasAiming = true;
	bToolIsChargeBased = false;
	ToolType = EPlayerToolType::TOOL_GUN;
}

void UPA_Tool_Grenade::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPA_Tool_Grenade::StartAimTool(float X, float Y)
{
	bToolIsActive = true;
	bToolIsAiming = true;

	ReceiveStartAimTool(X, Y);
}

void UPA_Tool_Grenade::StopAimTool()
{
	ReceiveStopAimTool();
	bToolIsActive = false;
	bToolIsAiming = false;
}

void UPA_Tool_Grenade::StartTool()
{
	Super::StartTool();
}

void UPA_Tool_Grenade::StopTool()
{
	Super::StopTool();
}