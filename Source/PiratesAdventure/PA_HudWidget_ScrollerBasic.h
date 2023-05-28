// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "PA_HudWidget_ContentsBase.h"
#include "PA_HudWidget_ScrollerBasic.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudWidget_ScrollerBasic : public UPA_HudWidget_ContentsBase
{
	GENERATED_BODY()	
		
	virtual void ConstructContentsWidget(TArray<FInventoryItem> Items) override;
	
	
};
