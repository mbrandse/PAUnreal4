// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimGraphDefinitions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PA_AnimNode_StartCombo.h"
#include "AnimGraphNode_StartCombo.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTUREEDITOR_API UAnimGraphNode_StartCombo : public UAnimGraphNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Attacks")
		FPA_AnimNode_StartCombo Node;

	UAnimGraphNode_StartCombo();
	
//public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	
};
