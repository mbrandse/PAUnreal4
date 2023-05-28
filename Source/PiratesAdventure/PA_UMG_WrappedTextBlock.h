// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "Components/TextBlock.h"
#include "PA_UMG_WrappedTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_UMG_WrappedTextBlock : public UTextBlock
{
	GENERATED_BODY()
	
public:

	UPA_UMG_WrappedTextBlock();
	
	void SetMargin(FMargin NewMargin) { Margin = NewMargin; };
};
