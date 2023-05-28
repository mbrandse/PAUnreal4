// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PA_ISMA_ChangeDirection.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_ChangeDirection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_ChangeDirection();

	//Custom version of EnableInput, that automatically absorbs the Context button
	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void EnableContextInput(class APlayerController* PlayerController);

	virtual void OnContext();

	UFUNCTION()
		void OnBeginDirectionSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndDirectionSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Depth Switch", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveContext();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Trigger;

	UPROPERTY()
		FVector EntryDirection;

	UPROPERTY()
		class USceneComponent* SourceComponent;
	
};
