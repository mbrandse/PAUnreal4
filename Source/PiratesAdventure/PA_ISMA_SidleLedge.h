// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_Interface_ContextAction.h"
#include "PA_ISMA_SidleLedge.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_SidleLedge : public AActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_SidleLedge();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Shimmy", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveContext();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sidle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SourceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sidle", meta = (AllowPrivateAccess = "true"))
		class USplineComponent* SidlePath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sidle", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* StartingPointA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sidle", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* StartingPointB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sidle", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* EntryPointA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sidle", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* EntryPointB;

	////Entry index is the point of the spline at which the player entered. Necessary to remember to prevent the player from instantly exiting the spline. 
	UPROPERTY()
		int32 EntryIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sidle")
		TArray<FRotator> SidleDirections;

	UPROPERTY()
		class APiratesAdventureCharacter* OverlappingActor;

	UFUNCTION()
		void OnSidleBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnSidleEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Begin Context Action Interface
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);
	//End Context Action Interface

	
	
};
