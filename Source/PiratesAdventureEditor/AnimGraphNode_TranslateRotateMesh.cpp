// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_TranslateRotateMesh.h"
#include "PiratesAdventureEditor.h"

#define LOCTEXT_NAMESPACE "AnimGraphNode_TranslateRotateMesh"

UAnimGraphNode_TranslateRotateMesh::UAnimGraphNode_TranslateRotateMesh()
{

}

//Title Color!
FLinearColor UAnimGraphNode_TranslateRotateMesh::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.5f);
}

//Node Category
FString UAnimGraphNode_TranslateRotateMesh::GetNodeCategory() const
{
	return FString("Pirates Adventure");
}

//Node Tooltip
FText UAnimGraphNode_TranslateRotateMesh::GetTooltipText() const
{
	//return LOCTEXT("AnimGraphNode_RotateInPlace_ToolTip", "The Rotate Bone allows for arbitrary rotations to be added to already existing animation. TODO: more sophisticated rotation animation");
	return LOCTEXT("AnimNode_TranslateRotateMesh_ToolTip", "EDITOR: The Translate and Rotate Mesh transforms the mesh to the desired relative location");
}

//Node Title(Needs to be updated now since node is much more versatile)
FText UAnimGraphNode_TranslateRotateMesh::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimNode_TranslateRotateMesh_Title", "Translate and Rotate Mesh");

}

#undef LOCTEXT_NAMESPACE


