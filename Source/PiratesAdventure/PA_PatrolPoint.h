// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_PatrolPoint.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_PatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_PatrolPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	
	
};
