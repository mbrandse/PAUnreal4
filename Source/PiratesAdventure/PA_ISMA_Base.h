// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_Interface_ContextAction.h"
#include "PA_ISMA_Base.generated.h"

UCLASS(HideCategories = (Replication, Input, HLOD, Collision))
class PIRATESADVENTURE_API APA_ISMA_Base : public AActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditDefaultsOnly, Category = "Interactive Object")
	//	FText ContextPopupText;

	//UPROPERTY(EditDefaultsOnly, Category = "Interactive Object")
	//	FLinearColor ContextPopupTextColor;

	//UPROPERTY(EditDefaultsOnly, Category = "Interactive Object")
	//	bool bShowContextPopupIcon;

	//Whether this object should tick when a context action is taking place. If the object has the tick enabled from the very start, this will be ignored.
	UPROPERTY(EditDefaultsOnly, Category = "Interactive Object")
		bool bTickDuringContextAction;

	//Sets whether the object is active or not. When inactive, it won't respond to any overlap events.
	UPROPERTY()
		bool bObjectIsActive;

	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Interactive Object", meta = (AllowPrivateAccess = true))
		class USceneComponent* SourceComponent;

	//UPROPERTY()
	//	class APiratesAdventureCharacter* OverlappingActor;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Interactive Object", meta = (AllowPrivateAccess = true))
	//	class UBoxComponent* Trigger;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Object", meta = (DisplayName = "Start Context Action"))
		void OnReceiveStartContextAction(class APiratesAdventureCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "Interactive Object")
		void StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	//UFUNCTION()
	//	void OnObjectBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//UFUNCTION()
	//	void OnObjectEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//UFUNCTION(BlueprintPure, Category = "Interactive Object")
	//	class APiratesAdventureCharacter* GetOverlappingActor() { return OverlappingActor; };	

	//Sets up the context button, so players can press the button to start the context action.
	UFUNCTION(BlueprintCallable, Category = "Interactive Object")
		void SetupContextButton(class APiratesAdventureCharacter* OverlappingPlayer, FText ContextPopupText, FLinearColor ContextPopupTextColor = FLinearColor::White, bool bShowContextIcon = true);

	//Resets the context button
	UFUNCTION(BlueprintCallable, Category = "Interactive Object")
		void ResetContextButton(class APiratesAdventureCharacter* OverlappingPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Object")
		void ForceObjectOverlap(class APiratesAdventureCharacter* OverlappingPlayer);

	UFUNCTION(BlueprintPure, Category = "Interactive Object")
		bool IsObjectActive() { return bObjectIsActive; };

	UFUNCTION(BlueprintCallable, Category = "Interactive Object")
		void ActivateObject() { bObjectIsActive = true; };

	UFUNCTION(BlueprintCallable, Category = "Interactive Object")
		void DeactivateObject() { bObjectIsActive = false; };
	
};
