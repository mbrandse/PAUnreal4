// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimNotify_PlayerStartDamage.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Weapon_Base.h"

void UPA_AnimNotify_PlayerStartDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//first attempt to get player. 
	APiratesAdventureCharacter* PlayerCharacter = Cast<APiratesAdventureCharacter>(MeshComp->GetAttachmentRootActor());
	if (PlayerCharacter)
	{
		if (PlayerCharacter->EquippedWeapon)
		{
			PlayerCharacter->EquippedWeapon->StartSwingDamage();
		}

		return;
	}

	//if that doesn't work, try to get any character containing a rama weapon
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetAttachmentRootActor());
	if (Character)
	{
		URamaMeleeWeapon* EquippedWeapon = Cast<URamaMeleeWeapon>(Character->GetComponentByClass(URamaMeleeWeapon::StaticClass()));
		if (EquippedWeapon)
			EquippedWeapon->StartSwingDamage();

		return;
	}
}
