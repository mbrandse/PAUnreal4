// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "PA_Tool_Base.h"
#include "PA_Tool_Grenade.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_Tool_Grenade : public UPA_Tool_Base
{
	GENERATED_BODY()
	
public:

	UPA_Tool_Grenade(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void StartTool() override;
	virtual void StopTool() override;
	virtual void StartAimTool(float X, float Y);
	virtual void StopAimTool();
};