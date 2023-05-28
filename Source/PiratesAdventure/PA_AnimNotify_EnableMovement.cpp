// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimNotify_EnableMovement.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"

void UPA_AnimNotify_EnableMovement::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(MeshComp->GetAttachmentRootActor());

	if (Character)
	{
		Character->EnableMovement();
	}
}