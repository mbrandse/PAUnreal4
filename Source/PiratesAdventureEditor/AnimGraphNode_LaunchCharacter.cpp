// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_LaunchCharacter.h"
#include "PiratesAdventureEditor.h"

#define LOCTEXT_NAMESPACE "AnimGraphNode_LaunchCharacter"

UAnimGraphNode_LaunchCharacter::UAnimGraphNode_LaunchCharacter()
{

}

//Title Color!
FLinearColor UAnimGraphNode_LaunchCharacter::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.5f);
}

//Node Category
FString UAnimGraphNode_LaunchCharacter::GetNodeCategory() const
{
	return FString("Pirates Adventure");
}

//Node Tooltip
FText UAnimGraphNode_LaunchCharacter::GetTooltipText() const
{
	return LOCTEXT("PA_AnimGraphNode_LaunchCharacter_ToolTip", "EDITOR: Launch Character allows you to launch a character from the start of the animation. Only usable for classes deriving from the character class.");
}

//Node Title(Needs to be updated now since node is much more versatile)
FText UAnimGraphNode_LaunchCharacter::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimGraphNode_LaunchCharacter_Title", "Launch Character");

}

#undef LOCTEXT_NAMESPACE