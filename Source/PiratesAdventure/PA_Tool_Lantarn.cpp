// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Tool_Lantarn.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "Components/PointLightComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


UPA_Tool_Lantarn::UPA_Tool_Lantarn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bToolHasAiming = false;
	bToolIsChargeBased = true;
	ToolType = EPlayerToolType::TOOL_LANTARN;
	SetVisibility(false);
}

void UPA_Tool_Lantarn::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPA_Tool_Lantarn::InitializeTool()
{
	Super::InitializeTool();
}

void UPA_Tool_Lantarn::DeinitializeTool()
{
	Super::DeinitializeTool();
}

void UPA_Tool_Lantarn::StartTool()
{
	//TODO: DON'T STOP THE LANTARN WHEN YOU HAVE NO CHARGE. INSTEAD, JUST TURN OFF THE LIGHT! 
	if (CanStartTool())
	{
		if (!bToolIsActive)
		{
			bToolIsActive = true;

			Super::StartTool();
			return;
		}
	}

	//We need to seperate this, because we need to be able to stop the tool even if we don't have enough ammo
	if (bToolIsActive)
	{
		StopTool();
	}
}

void UPA_Tool_Lantarn::StopTool()
{
	Super::StopTool();

	bToolIsActive = false;
}