// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PA_Interface_ContextAction.h"
#include "PiratesAdventureCharacter.h"
#include "PA_ISMA_CarryObject.generated.h"

UENUM(BlueprintType)
enum class EObjectWeightType : uint8
{
	OWT_Light UMETA(DisplayName = "Light Object"),
	OWT_Heavy UMETA(DisplayName = "Heavy Object"),
};

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_ISMA_CarryObject : public AStaticMeshActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()

public: 

	APA_ISMA_CarryObject();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent* SourceComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Carry")
		EObjectWeightType ObjectWeightType;

	UPROPERTY(EditDefaultsOnly, Category = "Carry")
		FName AttachmentSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Carry", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Trigger;

	UFUNCTION()
		void OnCarryBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnCarryEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		class APiratesAdventureCharacter* OverlappingActor;

	//Context related actions (including interface ones)
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Carry", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveStartContextAction();	

	void StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Carry", meta = (DisplayName = "On Stop Context Action"))
		void OnReceiveStopContextAction();

	UFUNCTION(BlueprintCallable, Category = "Carry")
		void StartCarry();
	
	UFUNCTION(BlueprintCallable, Category = "Carry")
		void StopCarry();

	UFUNCTION(BlueprintCallable, Category = "Carry")
		void OnFinishLineUpCharacter();

	//Light carry variables
	UPROPERTY(EditDefaultsOnly, Category = "Carry")
		FPlayerMovementMutator LightObjectMutator;

	//Heavy carry variables
	UPROPERTY(EditDefaultsOnly, Category = "Carry")
		FPlayerMovementMutator HeavyObjectMutator;	

	UFUNCTION(BlueprintPure, Category = "Carry")
		class APiratesAdventureCharacter* GetOverlappingActor() { return OverlappingActor; };

	UFUNCTION(BlueprintCallable, Category = "Carry")
		void EnablePhysics();

	UFUNCTION(BlueprintCallable, Category = "Carry")
		void DisablePhysics();

	UPROPERTY()
		bool bIsCalculatingPhysics;

	UPROPERTY()
		bool bWaitForPhysicsCalculationEnd;

	UPROPERTY()
		bool bGotOverlapDuringPhysicsCalculation;
};
