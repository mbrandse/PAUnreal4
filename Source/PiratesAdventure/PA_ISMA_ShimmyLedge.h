// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "PA_Interface_ContextAction.h"
#include "PA_ISMA_ShimmyLedge.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_ISMA_ShimmyLedge : public AActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()

public:

	APA_ISMA_ShimmyLedge();

	virtual void Tick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shimmy", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveContext();

	UPROPERTY()
		class APiratesAdventureCharacter* OverlappingActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shimmy", meta = (AllowPrivateAccess = "true"))
		class USplineComponent* ShimmyPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shimmy", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* StartingPointA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shimmy", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* StartingPointB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shimmy", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* EntryPointA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shimmy", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* EntryPointB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shimmy", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SourceComponent;

	//How many points there are between the start and the end of the spline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shimmy")
		int32 ShimmyMidPoints = 0;
	
	//Entry index is the point of the spline at which the player entered. Necessary to remember to prevent the player from instantly exiting the spline. 
	UPROPERTY()
		int32 EntryIndex;

	UFUNCTION()
		void OnBeginEntryPointAOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndEntryPointAOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnBeginEntryPointBOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndEntryPointBOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	//Begin Context Action Interface
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	//End Context Action Interface
};
