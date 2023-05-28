// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_CarryObject.h"
#include "PiratesAdventure.h"
#include "Components/BoxComponent.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_CarryThrow.h"
#include "Kismet/KismetSystemLibrary.h"

APA_ISMA_CarryObject::APA_ISMA_CarryObject()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bIsCalculatingPhysics = false;
	bWaitForPhysicsCalculationEnd = false;
	bGotOverlapDuringPhysicsCalculation = false;

	ObjectWeightType = EObjectWeightType::OWT_Light;

	LightObjectMutator.DefaultMutator();
	LightObjectMutator.MutatorName = "LightObject";
	HeavyObjectMutator.DefaultMutator();
	HeavyObjectMutator.MutatorName = "HeavyObject";

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Movable);
	SetRootComponent(SourceComponent);

	GetStaticMeshComponent()->SetupAttachment(RootComponent);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->ShapeColor = FColor::Red;
	Trigger->SetGenerateOverlapEvents(true);
	Trigger->SetBoxExtent(FVector(128.f, 128.f, 32.f));
	Trigger->SetupAttachment(GetStaticMeshComponent());
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_CarryObject::OnCarryBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_CarryObject::OnCarryEndOverlap);
}

void APA_ISMA_CarryObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APA_ISMA_CarryObject::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingActor)
	{

	}
}

void APA_ISMA_CarryObject::StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingActor)
	{
		
	}
}

void APA_ISMA_CarryObject::OnFinishLineUpCharacter()
{
	
}

void APA_ISMA_CarryObject::OnCarryBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
}

void APA_ISMA_CarryObject::OnCarryEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void APA_ISMA_CarryObject::StartCarry()
{
	
}

void APA_ISMA_CarryObject::StopCarry()
{
	
}

void APA_ISMA_CarryObject::EnablePhysics()
{
	
}

void APA_ISMA_CarryObject::DisablePhysics()
{
	
}