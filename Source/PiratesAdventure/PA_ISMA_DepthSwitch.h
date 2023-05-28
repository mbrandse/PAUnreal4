// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PA_Interface_ContextAction.h"
#include "PA_ISMA_DepthSwitch.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_DepthSwitch : public AActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_DepthSwitch();

	// Called every frame
	//virtual void Tick(float DeltaSeconds) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
		void OnBeginDepthSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndDepthSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Depth Switch", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveContext(APiratesAdventureCharacter* Character, FVector TargetLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Switch")
		FTransform TriggerBottomTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Switch")
		FTransform TriggerTopTransform;

	UPROPERTY(VisibleDefaultsOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* LocationForDescend;

	UPROPERTY(VisibleDefaultsOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* LocationForAscend;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SourceComponent;

	UPROPERTY()
		FVector TargetLocation;

	//Start context action interface
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);
	//End interface	
};
