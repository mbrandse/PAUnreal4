// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_Item_Base.h"
#include "PiratesAdventure.h"

// Sets default values
APA_Item_Base::APA_Item_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APA_Item_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APA_Item_Base::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

