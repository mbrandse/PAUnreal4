// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimGraphDefinitions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PA_AnimNode_TranslateRotateMesh.h"
#include "AnimGraphNode_TranslateRotateMesh.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTUREEDITOR_API UAnimGraphNode_TranslateRotateMesh : public UAnimGraphNode_Base
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = SkeletalControl)
		FPA_AnimNode_TranslateRotateMesh Node;

public:

	UAnimGraphNode_TranslateRotateMesh();

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface	
};
