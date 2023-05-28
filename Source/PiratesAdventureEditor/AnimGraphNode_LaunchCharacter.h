// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimGraphDefinitions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PA_AnimNode_LaunchCharacter.h"
#include "AnimGraphNode_LaunchCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTUREEDITOR_API UAnimGraphNode_LaunchCharacter : public UAnimGraphNode_Base
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Animation)
		FPA_AnimNode_LaunchCharacter Node;

public:

	UAnimGraphNode_LaunchCharacter();

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface
};
