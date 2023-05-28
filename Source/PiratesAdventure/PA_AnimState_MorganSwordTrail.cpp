// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimState_MorganSwordTrail.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Weapon_Base.h"
#include "Components/SkeletalMeshComponent.h"

UParticleSystem* UPA_AnimState_MorganSwordTrail::GetOverridenPSTemplate(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	UParticleSystem* NewTemplate = nullptr;

	if (!MeshComp) return PSTemplate;

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		UPA_Weapon_Base* Weapon = Character->GetEquippedWeapon();
		if (Weapon)
		{
			switch (Weapon->CurrentWeaponType)
			{
			case EPlayerWeaponType::WEAPON_FIRE:
				NewTemplate = PSFireTemplate;
				break;

			case EPlayerWeaponType::WEAPON_ICE:
				NewTemplate = PSIceTemplate;
				break;

			case EPlayerWeaponType::WEAPON_WIND:
				NewTemplate = PSWindTemplate;
				break;

			case EPlayerWeaponType::WEAPON_WOOD:
				NewTemplate = PSWoodTemplate;
				break;

			default:
				NewTemplate = PSDefaultTemplate;
			}
		}
	}

	return NewTemplate;
}