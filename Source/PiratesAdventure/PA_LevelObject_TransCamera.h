// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "PA_LevelObject_TransCamera.generated.h"

UCLASS(HideCategories = (Actor, Input))
class PIRATESADVENTURE_API APA_LevelObject_TransCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_LevelObject_TransCamera();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
		void OnCompStartOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnCompEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		class APiratesAdventureCharacter* CameraOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Data")
		FCameraComponentTransform TargetCameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Data")
		float UpperLimit = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Data")
		float LowerLimit = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Data")
		class UCapsuleComponent* CollisionCapsule;

	UPROPERTY()
		FVector CameraOwnerEntryLocation;

	UPROPERTY()
		FCameraComponentTransform DefaultCameraTransform;
	
};
