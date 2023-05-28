// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimNotify_PlayerStopDamage.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Weapon_Base.h"

void UPA_AnimNotify_PlayerStopDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//first attempt to get player. 
	APiratesAdventureCharacter* PlayerCharacter = Cast<APiratesAdventureCharacter>(MeshComp->GetAttachmentRootActor());
	if (PlayerCharacter)
	{
		//While implementing in UE5, make it so we can actually use the weapon base for everyone. 
		if (PlayerCharacter->EquippedWeapon)
		{
			PlayerCharacter->EquippedWeapon->StopSwingDamage();
		}

		return;
	}

	//if that doesn't work, try to get any character containing a rama weapon
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetAttachmentRootActor());
	if (Character)
	{
		URamaMeleeWeapon* EquippedWeapon = Cast<URamaMeleeWeapon>(Character->GetComponentByClass(URamaMeleeWeapon::StaticClass()));
		if (EquippedWeapon)
			EquippedWeapon->StopSwingDamage();

		return;
	}
}


