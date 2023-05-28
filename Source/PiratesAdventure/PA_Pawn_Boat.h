// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
//#include "GameFramework/Pawn.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PA_Pawn_Boat.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_Pawn_Boat : public AActor//APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APA_Pawn_Boat();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//Boat Components
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Components", meta = (AllowPrivateAccess = true))
	//	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Components", meta = (AllowPrivateAccess = true))
		class USkeletalMeshComponent* BoatMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Components", meta = (AllowPrivateAccess = true))
		class USkeletalMeshComponent* FlagMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Components", meta = (AllowPrivateAccess = true))
		class USkeletalMeshComponent* SailMesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Components", meta = (AllowPrivateAccess = true))
	//	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Components", meta = (AllowPrivateAccess = true))
		class UBoxComponent* EntryCollisionBox;

	UFUNCTION()
		void OnBeginOverlapWithBoat(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION(BlueprintCallable, Category = "Boat Functions")
		void ApplyImpulseToBoat(float Value);
};
