// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_SidleLedge.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "PA_PlayerState_Shimmy.h"
#include "PiratesAdventureCharacter.h"

APA_ISMA_SidleLedge::APA_ISMA_SidleLedge()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SourceComponent);

	SidlePath = CreateDefaultSubobject<USplineComponent>(TEXT("SidlePath"));
	SidlePath->SetupAttachment(RootComponent);
	SidlePath->SetMobility(EComponentMobility::Static);

	StartingPointA = CreateDefaultSubobject<UBillboardComponent>(TEXT("StartingPointA"));
	StartingPointA->SetMobility(EComponentMobility::Static);
	StartingPointA->bHiddenInGame = true;
	StartingPointA->SetupAttachment(RootComponent);

	EntryPointA = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryPointA"));
	EntryPointA->ShapeColor = FColor::Red;
	EntryPointA->SetGenerateOverlapEvents(true);
	EntryPointA->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_SidleLedge::OnSidleBeginOverlap);
	EntryPointA->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_SidleLedge::OnSidleEndOverlap);
	EntryPointA->SetMobility(EComponentMobility::Static);
	EntryPointA->SetupAttachment(StartingPointA);

	StartingPointB = CreateDefaultSubobject<UBillboardComponent>(TEXT("StartingPointB"));
	StartingPointB->SetMobility(EComponentMobility::Static);
	StartingPointB->bHiddenInGame = true;
	StartingPointB->SetupAttachment(RootComponent);

	EntryPointB = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryPointB"));
	EntryPointB->ShapeColor = FColor::Blue;
	EntryPointB->SetGenerateOverlapEvents(true);
	EntryPointB->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_SidleLedge::OnSidleBeginOverlap);
	EntryPointB->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_SidleLedge::OnSidleEndOverlap);
	EntryPointB->SetMobility(EComponentMobility::Static);
	EntryPointB->SetupAttachment(StartingPointB);
}

void APA_ISMA_SidleLedge::BeginPlay()
{
	Super::BeginPlay();
}

void APA_ISMA_SidleLedge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APA_ISMA_SidleLedge::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
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
				ShimmyState->SetSidleData(SidlePath, SidleDirections, 0);
				OnReceiveContext();
				OverlappingActor->SetNewPlayerState(EPlayerStateType::GPS_GameShimmy);
			}
		}
	}
}

void APA_ISMA_SidleLedge::OnSidleBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character
		&& Character->GetCapsuleComponent() == OtherComp
		&& SidlePath)
	{
		FText HUDText = NSLOCTEXT("ContextActions", "ShimmyFromEntryPointA", "Sidle");
		Character->AddActorToContextArray(this, HUDText);
		OverlappingActor = Character;

		EntryIndex = OverlappedComponent == EntryPointA ? 0 : SidlePath->GetNumberOfSplinePoints() - 1;
	}
}

void APA_ISMA_SidleLedge::OnSidleEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character
		&& Character->GetCapsuleComponent() == OtherComp)
	{
		Character->RemoveActorFromContextArray(this);
		OverlappingActor = NULL;
	}
}