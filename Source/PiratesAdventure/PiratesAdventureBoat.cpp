// Fill out your copyright notice in the Description page of Project Settings.

#include "PiratesAdventureBoat.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_Boat.h"

APiratesAdventureBoat::APiratesAdventureBoat()
{
	GetSkeletalMeshComponent()->SetGenerateOverlapEvents(true);
	GetSkeletalMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &APiratesAdventureBoat::OnBeginOverlapWithBoat);
}

void APiratesAdventureBoat::OnBeginOverlapWithBoat(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(AnyLog, Log, TEXT("STARTED BOAT OVERLAP %s"), *(OtherActor->GetName()));

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);
	if (Character && Character->GetCurrentPlayerState() != EPlayerStateType::GPS_GameBoat)
	{
		Character->SetNewPlayerState(EPlayerStateType::GPS_GameBoat);
		UPA_PlayerState_Boat* PlayerState = Cast<UPA_PlayerState_Boat>(Character->GetRegisteredState(EPlayerStateType::GPS_GameBoat));
		if (PlayerState)
		{
			//PlayerState->SetBoatInstance(this);

			//Character->SetBase(GetSkeletalMeshComponent());
			Character->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

			//EnableInput(Character->GetController());
			//GetSkeletalMeshComponent()->SetSimulatePhysics(true);
			//GetSkeletalMeshComponent()->WakeAllRigidBodies();
			//Character->GetMesh()->SetSimulatePhysics(true);
			//Character->GetMesh()->WakeAllRigidBodies();
		}
	}
}

void APiratesAdventureBoat::SetBoatOwner(APiratesAdventureCharacter* NewBoatOwner)
{
	if (NewBoatOwner)
	{
		BoatOwner = NewBoatOwner;
		//BoatOwner->SetBase(GetSkeletalMeshComponent());

		if (BoatOwner->GetRegisteredState(EPlayerStateType::GPS_GameBoat))
		{
			BoatOwner->SetNewPlayerState(EPlayerStateType::GPS_GameBoat);
		}
	}
}