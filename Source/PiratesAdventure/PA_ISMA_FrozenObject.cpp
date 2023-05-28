// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_ISMA_FrozenObject.h"
#include "PiratesAdventure.h"

// Sets default values
APA_ISMA_FrozenObject::APA_ISMA_FrozenObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APA_ISMA_FrozenObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APA_ISMA_FrozenObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APA_ISMA_FrozenObject::SetActorsWithinRange(TArray<FHitResult> HitResults)
{
	ActorsWithinRange = HitResults;
}