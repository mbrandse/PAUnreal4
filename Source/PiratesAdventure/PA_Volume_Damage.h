// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "PA_StaticLibrary.h"
#include "PA_Volume_Damage.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_Volume_Damage : public APhysicsVolume
{
	GENERATED_BODY()

public:

	APA_Volume_Damage();

	// Called when actor enters a volume
	virtual void ActorEnteredVolume(class AActor* Other);

	// Called when actor leaves a volume, Other can be NULL
	virtual void ActorLeavingVolume(class AActor* Other);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		bool bIsContinuousDamage;

	//If this volume's continuous damage can be nulled by wind, the main character won't receive damage when using the chosen element with Lantarn
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		EPlayerWeaponType NulledElement = EPlayerWeaponType::NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		bool bSkipEntryDamage;

	UPROPERTY()
		TMap<class ACharacter*, int32> OverlappingActors;

	UPROPERTY()
		class APiratesAdventureCharacter* OverlappingPlayer;

	//If this contains any classes, the damage volume will limit itself to those classes only. Otherwise it just uses all actors.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		TArray< TSubclassOf<ACharacter> > CharacterClassLimits;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		float Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		const TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		float Interval;

	//if set to anything other than 0, will decrease air and only apply damage when air is 0. 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		float AirDecrease = 0.f;

	//If we want to apply damage to actors as well, add the damage type here.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Damage")
		const TSubclassOf<class UDamageType> ActorDamageType;

	FTimerHandle UpdateTimerHandle;

	void UpdateAllOverlappingActors();
	void CallDelayedCharacterCheckpoint();
	bool CheckForAllowedClass(ACharacter* ReferenceToCheck);
	
};
