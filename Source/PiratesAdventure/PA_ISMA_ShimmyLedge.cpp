// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_ShimmyLedge.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_Shimmy.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/InputDelegateBinding.h"
#include "Navigation/PathFollowingComponent.h"

APA_ISMA_ShimmyLedge::APA_ISMA_ShimmyLedge()
{
	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SourceComponent);

	ShimmyPath = CreateDefaultSubobject<USplineComponent>(TEXT("ShimmyPath"));
	ShimmyPath->SetupAttachment(RootComponent);
	ShimmyPath->SetMobility(EComponentMobility::Static);

	StartingPointA = CreateDefaultSubobject<UBillboardComponent>(TEXT("StartingPointA"));
	StartingPointA->SetMobility(EComponentMobility::Static);
	StartingPointA->bHiddenInGame = true;
	StartingPointA->SetupAttachment(RootComponent);

	EntryPointA = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryPointA"));
	EntryPointA->ShapeColor = FColor::Red;
	EntryPointA->SetGenerateOverlapEvents(true);
	EntryPointA->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_ShimmyLedge::OnBeginEntryPointAOverlap);
	EntryPointA->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_ShimmyLedge::OnEndEntryPointAOverlap);
	EntryPointA->SetMobility(EComponentMobility::Static);
	EntryPointA->SetupAttachment(StartingPointA);

	StartingPointB = CreateDefaultSubobject<UBillboardComponent>(TEXT("StartingPointB"));
	StartingPointB->SetMobility(EComponentMobility::Static);
	StartingPointB->bHiddenInGame = true;
	StartingPointB->SetupAttachment(RootComponent);

	EntryPointB = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryPointB"));
	EntryPointB->ShapeColor = FColor::Blue;
	EntryPointB->SetGenerateOverlapEvents(true);
	EntryPointB->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_ShimmyLedge::OnBeginEntryPointBOverlap);
	EntryPointB->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_ShimmyLedge::OnEndEntryPointBOverlap);
	EntryPointB->SetMobility(EComponentMobility::Static);
	EntryPointA->SetupAttachment(StartingPointB);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APA_ISMA_ShimmyLedge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APA_ISMA_ShimmyLedge::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingActor)
	{
		//UPathFollowingComponent* PFollowComp = nullptr;
		//OverlappingActor->GetController()->InitNavigationControl(PFollowComp);
		//UPathFollowingComponent* PFollowComp = InitNavigationControl(*OverlappingActor->GetController());
		UPathFollowingComponent* PFollowComp = OverlappingActor->GetController()->FindComponentByClass<UPathFollowingComponent>();

		if (PFollowComp
			&& PFollowComp->GetStatus() != EPathFollowingStatus::Moving)
		{
			UPA_PlayerState_Shimmy* ShimmyState = Cast<UPA_PlayerState_Shimmy>(OverlappingActor->GetRegisteredState(EPlayerStateType::GPS_GameShimmy));

			if (ShimmyState && ShimmyState->CanEnterState())
			{
				//ShimmyState->SetShimmyInstance(this);
				OnReceiveContext();
				OverlappingActor->SetNewPlayerState(EPlayerStateType::GPS_GameShimmy);
			}
		}
	}
}

void APA_ISMA_ShimmyLedge::OnBeginEntryPointAOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	EntryIndex = 0;

	//we need to make sure only the main capsule is responsible for hits, otherwise the frontal hit capsule will be taken into account as well.
	if (Character 
		&& Character->GetCapsuleComponent() == OtherComp)  
	{
		FText HUDText = NSLOCTEXT("ContextActions", "ShimmyFromEntryPointA", "Sidle"); 
		Character->AddActorToContextArray(this, HUDText);
		OverlappingActor = Character;
	}
}

void APA_ISMA_ShimmyLedge::OnEndEntryPointAOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character
		&& Character->GetCapsuleComponent() == OtherComp)
	{
		Character->RemoveActorFromContextArray(this);
	}
}

void APA_ISMA_ShimmyLedge::OnBeginEntryPointBOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	//EntryIndex = ShimmyPath->GetNumberOfSplinePoints() - 1;

	//if (Character
	//	&& Character->GetCapsuleComponent() == OtherComp)
	//{
	//	FText HUDText = NSLOCTEXT("ContextActions", "ShimmyFromEntryPointB", "Sidle");
	//	Character->AddActorToContextArray(this, HUDText);

	//	//To make sure we don't get into issues with interface buttons (since they share the context buttons), we should use tick to see whether the interface is active or not.
	//	//SetActorTickEnabled(true);
	//}
}

void APA_ISMA_ShimmyLedge::OnEndEntryPointBOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	//if (Character
	//	&& Character->GetCapsuleComponent() == OtherComp)
	//{
	//	Character->RemoveActorFromContextArray(this);

	//	//SetActorTickEnabled(false);

	//	//the default DisableInput implementation already checks whether we have made an inputcomponent or not.
	//	//if (OtherComp == Character->GetCapsuleComponent())
	//	//	DisableInput(GetWorld()->GetFirstPlayerController());
	//}
}