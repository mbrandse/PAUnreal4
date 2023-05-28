// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Tool_Eye.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "PA_ISMA_SightObject.h"

UPA_Tool_Eye::UPA_Tool_Eye(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bToolHasAiming = false;
	bToolIsChargeBased = true;
	ToolType = EPlayerToolType::TOOL_EYE;
}

void UPA_Tool_Eye::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPA_Tool_Eye::StartTool()
{
	if (CanStartTool())
	{
		if (!bToolIsActive)
		{
			bToolIsActive = true;
			OnStartEyeAnimation();
			return;
		}
	}

	if (bToolIsActive)
	{
		StopTool();
	}

	Super::StartTool();
}

void UPA_Tool_Eye::StopTool()
{
	Super::StopTool();

	bToolIsActive = false;
	OnEndEyeAnimation();
}