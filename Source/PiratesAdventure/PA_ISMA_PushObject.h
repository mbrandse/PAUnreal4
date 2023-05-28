// Fill out your copyright notice in the Description page of Project Settings.

//TODO: OVERHAUL THE PUSH OBJECT, AS IT DOESN'T WORK THE WAY WE WANT IT TO.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PA_ISMA_PushObject.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_ISMA_PushObject : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	APA_ISMA_PushObject();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	//since circular collision components will force the character into weird directions, we make sure pushable objects have a cubical collision component
	UPROPERTY()
		class APiratesAdventureCharacter* PushOwner;

	//since circular collision components will force the character into weird directions, we make sure pushable objects have a cubical collision component
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Push Object Properties", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionComponent;

	//if the pushable object can only be pushed by pressure, it means the player needs to use the gun to move it.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Push Object Properties")
		bool bCanOnlyBePushedByPressure;

	//Custom version of EnableInput, that automatically absorbs the Context button
	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void EnableContextInput(class APlayerController* PlayerController);

	virtual void OnStartContext();
	virtual void OnStopContext();

	UFUNCTION()
		void OnBeginPushOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndPushOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);	
};
