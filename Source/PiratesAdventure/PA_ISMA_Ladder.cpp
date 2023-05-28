// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_Ladder.h"
#include "PiratesAdventure.h"
#include "Components/BoxComponent.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_Climbing.h"
#include "PA_PlayerState_Normal.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APA_ISMA_Ladder::APA_ISMA_Ladder()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bIsClimbingLadder = false;

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SourceComponent);

	TriggerBottom = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBottom"));
	TriggerBottom->ShapeColor = FColor::Red;
	TriggerBottom->SetGenerateOverlapEvents(true);
	TriggerBottom->SetBoxExtent(FVector(32.f));
	TriggerBottom->SetupAttachment(RootComponent);
	TriggerBottom->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_Ladder::OnLadderBeginOverlap);
	TriggerBottom->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_Ladder::OnLadderEndOverlap);

	TriggerTop = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerTop"));
	TriggerTop->ShapeColor = FColor::Blue;
	TriggerTop->SetGenerateOverlapEvents(true);
	TriggerTop->SetBoxExtent(FVector(32.f));
	TriggerTop->SetupAttachment(RootComponent);
	TriggerTop->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	TriggerTop->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_Ladder::OnLadderBeginOverlap);
	TriggerTop->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_Ladder::OnLadderEndOverlap);
}

void APA_ISMA_Ladder::BeginPlay()
{
	Super::BeginPlay();

	if (TopTargetLocation == FVector::ZeroVector)
		TopTargetLocation = TriggerTop->GetComponentTransform().GetTranslation();

	if (BottomTargetLocation == FVector::ZeroVector)
		BottomTargetLocation = TriggerBottom->GetComponentTransform().GetTranslation();
	
}

void APA_ISMA_Ladder::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingActor)
	{
		//FVector TargetLoc = GetActorLocation();
		//FRotator TargetRot = GetActorRotation();
		//TargetRot.Yaw += 180.f;

		//if (!bEnteredFromTop)
		//{
		//	TargetLoc.Z += OverlappingActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		//	TargetLoc += GetActorForwardVector() * LadderAnimationOffset;
		//}
		//else
		//{
		//	TargetLoc.Z = TopTargetLocation.Z;
		//	TargetLoc.Z += OverlappingActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		//	TargetLoc -= GetActorForwardVector() * LadderAnimationOffset;
		//}	

		OnReceiveStartContextAction(OverlappingActor, bEnteredFromTop);
	}
}

void APA_ISMA_Ladder::StopContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingActor
		&& bIsClimbingLadder)
	{
		//FVector TargetLoc;

		//if (!bExitedFromTop)
		//{
		//	TargetLoc = BottomTargetLocation;
		//	TargetLoc.Z += OverlappingActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		//	TargetLoc += GetActorForwardVector() * OverlappingActor->GetCapsuleComponent()->GetScaledCapsuleRadius();
		//}
		//else
		//{
		//	TargetLoc = TopTargetLocation;
		//	TargetLoc.Z += OverlappingActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		//	TargetLoc += GetActorForwardVector() * OverlappingActor->GetCapsuleComponent()->GetScaledCapsuleRadius();
		//}

		//OverlappingActor->SetActorEnableCollision(true);
		//OverlappingActor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		OnReceiveStopContextAction(OverlappingActor, bExitedFromTop);
	}
}

void APA_ISMA_Ladder::OnMoveToLadderCompleted()
{

}

void APA_ISMA_Ladder::OnMoveFromLadderCompleted()
{
	if (OverlappingActor)
	{
		UPA_PlayerState_Normal* NormalState = Cast<UPA_PlayerState_Normal>(OverlappingActor->GetRegisteredState(EPlayerStateType::GPS_GameNormal));
		if (NormalState)
		{
			OverlappingActor->SetNewPlayerState(EPlayerStateType::GPS_GameNormal);

			//We will assume that once this function is called, we are back in our trigger volume, so let's call the overlap manually.
			UBoxComponent* TriggeredBox = !bExitedFromTop ? TriggerBottom : TriggerTop;
			FHitResult EmptyHit;
			OnLadderBeginOverlap(TriggeredBox, OverlappingActor, OverlappingActor->GetCapsuleComponent(), 0, false, EmptyHit);
		}
	}
}

void APA_ISMA_Ladder::OnLadderBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APiratesAdventureCharacter* PlayerCharacter = Cast<APiratesAdventureCharacter>(OtherActor);

	if (PlayerCharacter
		&& PlayerCharacter->GetCapsuleComponent() == OtherComp
		/*&& CurrentLadderState == EISMAState::ISMASTATE_None*/)
	{
		OverlappingActor = PlayerCharacter;
		if (OverlappedComponent == TriggerBottom)
		{
			FText HUDText = NSLOCTEXT("ContextActions", "UseLadderFromBottom", "Go Up");
			PlayerCharacter->AddActorToContextArray(this, HUDText);
			bEnteredFromTop = false;
		}
		else
		{
			FText HUDText = NSLOCTEXT("ContextActions", "UseLadderFromBottom", "Go Down");
			PlayerCharacter->AddActorToContextArray(this, HUDText);
			bEnteredFromTop = true;
		}
	}
}

void APA_ISMA_Ladder::OnLadderEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiratesAdventureCharacter* PlayerCharacter = Cast<APiratesAdventureCharacter>(OtherActor);

	if (PlayerCharacter
		&& PlayerCharacter->GetCapsuleComponent() == OtherComp)
	{
		PlayerCharacter->RemoveActorFromContextArray(this);
	}
}