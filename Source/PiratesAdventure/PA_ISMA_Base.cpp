// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_Base.h"
#include "PiratesAdventure.h"
#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"
#include "PiratesAdventureCharacter.h"


// Sets default values
APA_ISMA_Base::APA_ISMA_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bTickDuringContextAction = false;
	bObjectIsActive = true;
	bBlockInput = true;

	//ContextPopupText = NSLOCTEXT("ContextActions", "", "");

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SourceComponent);

	//Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	//Trigger->ShapeColor = FColor::Red;
	//Trigger->bGenerateOverlapEvents = true;
	//Trigger->SetBoxExtent(FVector(128.f, 128.f, 128.f));
	//Trigger->SetupAttachment(RootComponent);
	//Trigger->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_Base::OnObjectBeginOverlap);
	//Trigger->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_Base::OnObjectEndOverlap);
	//Trigger->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void APA_ISMA_Base::BeginPlay()
{
	Super::BeginPlay();

	////In the case of multiple trigger components, add any trigger components with overlapping on to the OnObjectBeginOverlap
	//TArray<UShapeComponent*> TriggerComps;
	//GetComponents<UShapeComponent>(TriggerComps);
	//for (UShapeComponent* TriggerComp : TriggerComps)
	//{
	//	if (TriggerComp
	//		&& TriggerComp->bGenerateOverlapEvents
	//		&& TriggerComp != Trigger
	//		&& !TriggerComp->OnComponentBeginOverlap.IsBound()
	//		&& !TriggerComp->OnComponentEndOverlap.IsBound())
	//	{
	//		TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_Base::OnObjectBeginOverlap);
	//		TriggerComp->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_Base::OnObjectEndOverlap);
	//	}
	//}	
}

// Called every frame
void APA_ISMA_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APA_ISMA_Base::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingPlayer)
	{
		OnReceiveStartContextAction(OverlappingPlayer);

		if (bTickDuringContextAction
			&& !PrimaryActorTick.bStartWithTickEnabled
			&& PrimaryActorTick.bCanEverTick)
		{
			SetActorTickEnabled(true);
		}

		OverlappingPlayer->RemoveActorFromContextArray(this);
	}
}

void APA_ISMA_Base::StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingPlayer)
	{
		ForceObjectOverlap(OverlappingPlayer);
	}

	if (bTickDuringContextAction
		&& !PrimaryActorTick.bStartWithTickEnabled
		&& PrimaryActorTick.bCanEverTick)
	{
		SetActorTickEnabled(false);
	}
}

void APA_ISMA_Base::SetupContextButton(class APiratesAdventureCharacter* OverlappingPlayer, FText ContextPopupText, FLinearColor ContextPopupTextColor /* = FColor::White */, bool bShowContextIcon /* = true */)
{
	if (bObjectIsActive
		&& OverlappingPlayer
		&& !ContextPopupText.IsEmpty())
	{
		OverlappingPlayer->AddActorToContextArray(this, ContextPopupText, ContextPopupTextColor, bShowContextIcon);
	}
}

void APA_ISMA_Base::ResetContextButton(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (bObjectIsActive
		&& OverlappingPlayer)
	{
		OverlappingPlayer->RemoveActorFromContextArray(this);
	}
}

//void APA_ISMA_Base::OnObjectBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//	if (bObjectIsActive)
//	{
//		APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);
//
//		if (Character
//			&& Character->GetCapsuleComponent() == OtherComp
//			&& !ContextPopupText.IsEmpty())
//		{
//			OverlappingActor = Character;
//			Character->AddActorToContextArray(this, ContextPopupText, ContextPopupTextColor, bShowContextPopupIcon);
//		}
//	}
//}
//
//void APA_ISMA_Base::OnObjectEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	if (bObjectIsActive)
//	{
//		if (OverlappingActor
//			&& OverlappingActor == OtherActor
//			&& OverlappingActor->GetCapsuleComponent() == OtherComp)
//		{
//			OverlappingActor->RemoveActorFromContextArray(this);
//			OverlappingActor = NULL;
//		}
//	}
//}