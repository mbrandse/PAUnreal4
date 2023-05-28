// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_Interface_ContextAction.h"
#include "PA_StaticLibrary.h"
#include "PA_ISMA_Ladder.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_Ladder : public AActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_Ladder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerTop;

	UFUNCTION()
		void OnLadderBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnLadderEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent* SourceComponent;

	UPROPERTY()
		bool bEnteredFromTop;

	UPROPERTY()
		bool bExitedFromTop;

	UPROPERTY()
		bool bIsClimbingLadder; //A bool for when the character has finished all preparations and is actually climbing the ladder. To make sure we don't call StopContext before we are actually climbing.

	UPROPERTY()
		class APiratesAdventureCharacter* OverlappingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder")
		FVector TopTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder")
		FVector BottomTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing State")
		float LadderTopExitAnimationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing State")
		float LadderTopEnterAnimationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing State")
		float LadderAnimationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder")
		float LadderHeight;

	//Context related actions (including interface ones)
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ladder", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveStartContextAction(class APiratesAdventureCharacter* Character, bool bFromTop);

	void StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ladder", meta = (DisplayName = "On Stop Context Action"))
		void OnReceiveStopContextAction(class APiratesAdventureCharacter* Character, bool bFromTop);

	//callbacks and move end functions
	UFUNCTION(BlueprintCallable, Category = "Ladder")
		void OnMoveFromLadderCompleted();

	//UFUNCTION(BlueprintCallable, Category = "Ladder")
	//	void OnLatentMoveFromLadderCompleted();

	UFUNCTION(BlueprintCallable, Category = "Ladder")
		void OnMoveToLadderCompleted();

	//UFUNCTION(BlueprintCallable, Category = "Ladder")
	//	void OnLatentMoveToLadderCompleted();
	
	/*
	State None: used to determine whether ladder is in use or not. If state is none, ladder is not being used.
	State A: Bottom Move to ladder (start using ladder)
	State B: Top Move to ladder (start using ladder)
	State C: Bottom Move from ladder (stop using ladder)
	state D: Top move from ladder (stop using ladder)
	*/
	EISMAState CurrentLadderState;
};
