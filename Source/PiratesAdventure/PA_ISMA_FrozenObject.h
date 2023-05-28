// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "GameFramework/Actor.h"
#include "PA_ISMA_FrozenObject.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_ISMA_FrozenObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_ISMA_FrozenObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FHitResult> ActorsWithinRange;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetActorsWithinRange(TArray<FHitResult> HitResults);

	UFUNCTION(BlueprintCallable, Category = "Frozen Object")
		TArray<FHitResult> GetActorsWithinRange() { return ActorsWithinRange; };
	
};
