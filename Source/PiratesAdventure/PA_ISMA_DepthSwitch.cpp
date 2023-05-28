// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_DepthSwitch.h"
#include "PiratesAdventure.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "PiratesAdventureCharacter.h"
#include "Engine/InputDelegateBinding.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"

APA_ISMA_DepthSwitch::APA_ISMA_DepthSwitch()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	//bInitialized = false;

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SourceComponent);

	TriggerBottom = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBottom"));
	TriggerBottom->ShapeColor = FColor::Green;
	TriggerBottom->SetGenerateOverlapEvents(true);
	TriggerBottom->SetBoxExtent(FVector(100.f));
	//TriggerBottom->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TriggerBottom->SetupAttachment(RootComponent);
	TriggerBottom->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_DepthSwitch::OnBeginDepthSwitchOverlap);
	TriggerBottom->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_DepthSwitch::OnEndDepthSwitchOverlap);

	TriggerTop = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerTop"));
	TriggerTop->ShapeColor = FColor::White;
	TriggerTop->SetGenerateOverlapEvents(true);
	TriggerTop->SetBoxExtent(FVector(100.f));
	//TriggerTop->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TriggerTop->SetupAttachment(RootComponent);
	TriggerTop->AddRelativeLocation(FVector(0.f, 200.f, 0.f));
	TriggerTop->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_DepthSwitch::OnBeginDepthSwitchOverlap);
	TriggerTop->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_DepthSwitch::OnEndDepthSwitchOverlap);

	LocationForAscend = CreateDefaultSubobject<UArrowComponent>(TEXT("LocationForAscend"));
	LocationForAscend->SetArrowColor(TriggerBottom->ShapeColor);
	LocationForAscend->AddRelativeRotation(FRotator(0.f, 0.f, 0.f));
	LocationForAscend->ArrowSize = 2.f;

	LocationForDescend = CreateDefaultSubobject<UArrowComponent>(TEXT("LocationForDescend"));
	LocationForDescend->SetArrowColor(TriggerTop->ShapeColor);
	LocationForDescend->AddRelativeRotation(FRotator(0.f, 0.f, 0.f));
	LocationForDescend->ArrowSize = 2.f;
}

void APA_ISMA_DepthSwitch::OnConstruction(const FTransform& Transform)
{
	//if (!bInitialized)
	//{
	//	TriggerTop->SetRelativeLocation(FVector(0.f, 200.f, 0.f));
	//	bInitialized = true;
	//}

	TriggerBottom->SetRelativeLocation(FVector::ZeroVector);

	LocationForAscend->SetRelativeLocation(TriggerBottom->RelativeLocation);
	LocationForDescend->SetRelativeLocation(TriggerTop->RelativeLocation);

	LocationForAscend->SetRelativeRotation(FRotationMatrix::MakeFromX(LocationForDescend->RelativeLocation - LocationForAscend->RelativeLocation).Rotator());
	LocationForDescend->SetRelativeRotation(FRotationMatrix::MakeFromX(LocationForAscend->RelativeLocation - LocationForDescend->RelativeLocation).Rotator());
}

//void APA_ISMA_DepthSwitch::Tick(float DeltaSeconds)
//{
//	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
//
//	if (Character)
//	{
//		//TODO: THERE'S STILL SOME PROBLEMS WITH THIS. INTRODUCE A SEPERATE TARGET VARIABLE THAT ONLY GETS RESET WHEN THE PLAYER REACHES THE POSITION. THE
//		//OTHER ONE CONSTANTLY GETS RESET UPON EXITING THE TRIGGER BOXES.
//
//		UPathFollowingComponent* PFollowComp = nullptr;
//		Character->GetController()->InitNavigationControl(PFollowComp);
//
//		if (PFollowComp
//			&& PFollowComp->GetStatus() == EPathFollowingStatus::Idle)
//		{
//			Character->EnableMovement();
//			SetActorTickEnabled(false);
//		}
//
//		//UE_LOG(AnyLog, Log, TEXT("DISTANCE TO TARGET %f"), FVector::Distance(Character->GetActorLocation(), TargetLocation));
//		//UE_LOG(AnyLog, Log, TEXT("LOCATION %s TARGET %s"), *(Character->GetActorLocation().ToString()), *(TargetLocation.ToString()));
//
//		//if (FVector::Distance(Character->GetActorLocation(), TargetLocation) <= 20.f)
//		//{
//		//	Character->EnableMovement();
//		//	SetActorTickEnabled(false);
//		//}
//	}
//}

void APA_ISMA_DepthSwitch::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (UWorld* World = GetWorld())
	{
		APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(World->GetFirstPlayerController()->GetCharacter());

		if (Character)
		{
			OnReceiveContext(Character, TargetLocation);
		}
	}
}

void APA_ISMA_DepthSwitch::OnBeginDepthSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character
		&& OtherComp == Character->GetCapsuleComponent())
	{
		Character->EnableMovement();

		if (OverlappedComponent == TriggerBottom)
		{
			TargetLocation = TriggerTop->GetRelativeTransform().TransformPosition(SourceComponent->RelativeLocation);

			FText HUDText = NSLOCTEXT("ContextActions", "UseDepthSwitchFromBottom", "Go Up"); 
			Character->AddActorToContextArray(this, HUDText);
		}
		else
		{
			TargetLocation = TriggerBottom->GetRelativeTransform().TransformPosition(SourceComponent->RelativeLocation);

			FText HUDText = NSLOCTEXT("ContextActions", "UseDepthSwitchFromTop", "Go Down");
			Character->AddActorToContextArray(this, HUDText);
		}
	}
}

void APA_ISMA_DepthSwitch::OnEndDepthSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character
		&& OtherComp == Character->GetCapsuleComponent())
	{
		Character->RemoveActorFromContextArray(this);
		TargetLocation = FVector::ZeroVector;
	}
}