// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PA_ISMA_SightObject.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_SightObject : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_SightObject();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Sight Functions")
		void OnStartSightOverlap();

	UFUNCTION(BlueprintImplementableEvent, Category = "Sight Functions")
		void OnStopSightOverlap();
	
};
