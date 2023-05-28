// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "PA_BTS_HumanPathLocation.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_BTS_HumanPathLocation : public UBTService
{
	GENERATED_BODY()

public:

	UPA_BTS_HumanPathLocation();

	/** update next tick interval
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void SetOwner(AActor* ActorOwner);

	UPROPERTY(Transient)
		class APA_AI_ControllerHumanBase* AIController;

	UPROPERTY(Transient)
		class APA_AI_CharacterHumanBase* AIPawn;

	/** for now, so we can test it. */
	UPROPERTY(EditAnywhere, Category = "AI Pathing")
		FBlackboardKeySelector TargetLocation;

	/** distance threshold to accept as being at location */
	UPROPERTY(EditAnywhere, Category = "AI Pathing", meta = (ClampMin = "0.0"))
		float AcceptableRadius;
	
};
