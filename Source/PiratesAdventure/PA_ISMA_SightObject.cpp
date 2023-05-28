// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_SightObject.h"
#include "PiratesAdventure.h"

// Sets default values
APA_ISMA_SightObject::APA_ISMA_SightObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void APA_ISMA_SightObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APA_ISMA_SightObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

