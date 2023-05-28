// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_ISMA_BaseTarget.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_BaseTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_BaseTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Static Mesh Target")
		bool bIsActivated;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Static Mesh Target")
		void ActivateTarget();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Static Mesh Target")
		void DeactivateTarget();

	
	
};
