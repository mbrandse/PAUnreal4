// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_StartCombo.h"
#include "PiratesAdventureEditor.h"

#define LOCTEXT_NAMESPACE "AnimGraphNode_StartCombo"

UAnimGraphNode_StartCombo::UAnimGraphNode_StartCombo()
{

}

FLinearColor UAnimGraphNode_StartCombo::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.5f);
}

FString UAnimGraphNode_StartCombo::GetNodeCategory() const
{
	return TEXT("PiratesAdventure");
}

FText UAnimGraphNode_StartCombo::GetTooltipText() const
{
	return LOCTEXT("StartComboNode_Tooltip", "EDITOR: Starts or continues a combo attack. Handles the start and end for weapon hit detection and determines how much damage is dealt. The start combo node also handles combo variables in actors holding a Melee Attack Interface");
}

FText UAnimGraphNode_StartCombo::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("StartComboNode_Title", "Start Combo");
}

#undef LOCTEXT_NAMESPACE