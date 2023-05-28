// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/SkeletalMeshActor.h"
#include "PiratesAdventureCharacter.h"
#include "PiratesAdventureBoat.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APiratesAdventureBoat : public ASkeletalMeshActor
{
	GENERATED_BODY()
	
public:

	APiratesAdventureBoat();

	UPROPERTY()
		APiratesAdventureCharacter* BoatOwner;

	UFUNCTION()
		void OnBeginOverlapWithBoat(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Boat Functions")
		void SetBoatOwner(APiratesAdventureCharacter* NewBoatOwner);
	
	
	
};
