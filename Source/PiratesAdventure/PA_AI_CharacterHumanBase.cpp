// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AI_CharacterHumanBase.h"
#include "PiratesAdventure.h"
#include "PA_AI_ControllerHumanBase.h"
//#include "PA_PathComponent_Human.h"
//#include "PA_DamageType_Base.h"

// Sets default values
APA_AI_CharacterHumanBase::APA_AI_CharacterHumanBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIState = EAIState::STATE_NORMAL;

}

// Called when the game starts or when spawned
void APA_AI_CharacterHumanBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APA_AI_CharacterHumanBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void APA_AI_CharacterHumanBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}