// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_Interface_GlobalCharacter.h"
#include "PiratesAdventure.h"

UPA_Interface_GlobalCharacter::UPA_Interface_GlobalCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void IPA_Interface_GlobalCharacter::UpdateScalarMaterialParameterSet(UMaterialParameterCollection* Collection, FName ParameterName, float Value)
{
	//empty implementation, consider adding the basic functionality here.
}