// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_Volume_Water.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "Kismet/KismetMathLibrary.h"

APA_Volume_Water::APA_Volume_Water()
{
	WaterMutator.DefaultMutator();

	WaterMutator.MutatorName = "WaterMutator";
	WaterMutator.GravityScale = 0.35f;
	WaterMutator.MaxWalkSpeed = 0.667f;
	WaterMutator.RotationRate = FRotator(0.f, 0.75f, 0.f);
	WaterMutator.MaxCrouchSpeed = 0.5f;
	WaterMutator.JumpSingleHeight = 0.667f;
	WaterMutator.JumpDoubleHeight = 0.8;
	WaterMutator.CustomTimeDilation = 0.667f;

	bPlayerCanBeAffectedByVolume = true;
	bBulletsCanBeAffectedByVolume = true;
}

void APA_Volume_Water::ActorEnteredVolume(class AActor* Other)
{
	APiratesAdventureCharacter* Player = Cast<APiratesAdventureCharacter>(Other);

	if (Player)
	{
		if (bPlayerCanBeAffectedByVolume)
		{
			Player->SetPlayerMutator(WaterMutator);
			if (WaterEntryParticle)
				UGameplayStatics::SpawnEmitterAttached(WaterEntryParticle, Player->GetMesh(), "RootSocket", FVector::ZeroVector, FRotator::ZeroRotator);

			Player->CurrentWaterVolume = this;

			//if (AirDecrease != 0.f)
			//	Player->AirDecrease = AirDecrease;
		}

		return;
	}

	APA_Tool_Bullet* EnteredBullet = Cast<APA_Tool_Bullet>(Other);
	if (EnteredBullet)
	{
		if (bBulletsCanBeAffectedByVolume)
		{
			EnteredBullet->OnEnterVolume(this);
		}

		return;
	}
}

void APA_Volume_Water::ActorLeavingVolume(class AActor* Other)
{
	APiratesAdventureCharacter* Player = Cast<APiratesAdventureCharacter>(Other);

	if (Player)
	{
		if (bPlayerCanBeAffectedByVolume)
			Player->RemovePlayerMutator("WaterMutator");

		Player->CurrentWaterVolume = NULL;

		//if (AirDecrease != 0.f)
		//	Player->AirDecrease = 0.f;
	}
}

void APA_Volume_Water::ActivateVolume()
{
	bPlayerCanBeAffectedByVolume = true;

	if (GetWorld())
	{
		APiratesAdventureCharacter* Player = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if(Player)
			ActorEnteredVolume(Player);
	}
}

void APA_Volume_Water::DeactivateVolume()
{
	if (GetWorld())
	{
		APiratesAdventureCharacter* Player = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			//Set bCanBeAffectedByVolume temporarily to true so we can remove all mutators from the character before turning it off.
			bPlayerCanBeAffectedByVolume = true;
			ActorLeavingVolume(Player);
		}
	}

	bPlayerCanBeAffectedByVolume = false;
}

bool APA_Volume_Water::IsPlayerWithinVolume()
{
	if (GetWorld())
	{
		APiratesAdventureCharacter* Player = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			bool bTopOverlap, bBottomOverlap;
			FVector ActorLocation = Player->GetActorLocation();
			float ActorHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			bTopOverlap = UKismetMathLibrary::IsPointInBoxWithTransform(ActorLocation + FVector(0, 0, ActorHeight), GetActorTransform(), GetBounds().BoxExtent);
			bBottomOverlap = UKismetMathLibrary::IsPointInBoxWithTransform(ActorLocation - FVector(0, 0, ActorHeight), GetActorTransform(), GetBounds().BoxExtent);

			return bTopOverlap && bBottomOverlap;
		}
	}

	return false;
}

bool APA_Volume_Water::IsActorWithinVolume(class AActor* Actor, bool bOnlyCollidingParts/* = false*/, FVector SampleOffset /* = FVector::ZeroVector */)
{
	if (Actor)
	{
		bool bTopOverlap, bBottomOverlap;
		FVector ActorLocation = Actor->GetActorLocation();
		FVector ActorBounds, ActorOrigin;
		Actor->GetActorBounds(bOnlyCollidingParts, ActorOrigin, ActorBounds);

		bTopOverlap = UKismetMathLibrary::IsPointInBoxWithTransform(ActorLocation + FVector(0, 0, ActorBounds.Z), GetActorTransform(), GetBounds().BoxExtent - SampleOffset);
		bBottomOverlap = UKismetMathLibrary::IsPointInBoxWithTransform(ActorLocation - FVector(0, 0, ActorBounds.Z), GetActorTransform(), GetBounds().BoxExtent - SampleOffset);

		return bTopOverlap && bBottomOverlap;
	}

	return false;
}