// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_ISMA_Door.h"
#include "PiratesAdventure.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "PiratesAdventureCharacter.h"
#include "Engine/InputDelegateBinding.h"
#include "Navigation/PathFollowingComponent.h"
#include "PA_PlayerSpringArmComponent.h"
#include "PA_BlueprintFunctionLibrary.h"

APA_ISMA_Door::APA_ISMA_Door(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	//SetRootComponent(SourceComponent);

	//GetStaticMeshComponent()->SetupAttachment(RootComponent);
	//GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	TriggerDoorA = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerDoorA"));
	TriggerDoorA->SetupAttachment(SourceComponent);
	TriggerDoorA->ShapeColor = FColor::Red;
	TriggerDoorA->SetGenerateOverlapEvents(true);
	TriggerDoorA->SetBoxExtent(FVector(128.f));
	TriggerDoorA->AddRelativeLocation(FVector(400.f, 0.f, 0.f));
	TriggerDoorA->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_Door::OnBeginDoorOverlap);
	TriggerDoorA->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_Door::OnEndDoorOverlap);

	TriggerDoorB = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerTop"));
	TriggerDoorB->SetupAttachment(SourceComponent);
	TriggerDoorB->ShapeColor = FColor::Blue;
	TriggerDoorB->SetGenerateOverlapEvents(true);
	TriggerDoorB->SetBoxExtent(FVector(128.f));
	TriggerDoorB->AddRelativeLocation(FVector(-400.f, 0.f, 0.f));
	TriggerDoorB->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_Door::OnBeginDoorOverlap);
	TriggerDoorB->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_Door::OnEndDoorOverlap);

	//OpenDoorASpriteComponent = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("OpenDoorASprite"));
	OpenDoorASpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("OpenDoorASprite"));
	OpenDoorASpriteComponent->SetupAttachment(SourceComponent);
	OpenDoorASpriteComponent->AddRelativeLocation(FVector(-100.f, 100.f, 0.f));
	OpenDoorASpriteComponent->bHiddenInGame = true;

	//OpenDoorBSpriteComponent = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("OpenDoorBSprite"));
	OpenDoorBSpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("OpenDoorBSprite"));
	OpenDoorBSpriteComponent->SetupAttachment(SourceComponent);
	OpenDoorBSpriteComponent->AddRelativeLocation(FVector(100.f, 100.f, 0.f));
	OpenDoorBSpriteComponent->bHiddenInGame = true;

	//DoorASpriteComponent = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("DoorASprite"));
	DoorASpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("DoorASprite"));
	DoorASpriteComponent->SetupAttachment(TriggerDoorA);
	DoorASpriteComponent->bHiddenInGame = true;

	//DoorBSpriteComponent = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("DoorBSprite"));
	DoorBSpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("DoorBSprite"));
	DoorBSpriteComponent->SetupAttachment(TriggerDoorB);
	DoorBSpriteComponent->bHiddenInGame = true;

	//TODO: add functionality for keeping door unlocked
	bDoorIsUnlocked = false;
	bFinishedMoveToDoor = true;
}

void APA_ISMA_Door::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Since this works with level streaming, we need to use the tick to finalize settings.
	if (OverlappingCharacter
		&& bFinishedMoveToDoor)
	{
		if (!bShouldTeleport
			|| bSkipLevelStreaming)
		{
			if (!bKeepCurrentCameraSettings)
			{
				OverlappingCharacter->GetCameraBoom()->ResetCameraToDefault();
			}

			if (OverlappingCharacter->GetCameraBoom()->IsManuallyTranslating())
				OverlappingCharacter->GetCameraBoom()->StopManualCameraTranslation();

			OverlappingCharacter->GetCameraBoom()->SetCameraLock(false);
			bSkipLevelStreaming = false;

			//Manually activate the overlap volumes, but only in the case of returning back from the door
			FHitResult EmptyHit;
			OnBeginDoorOverlap(TriggerDoorA, OverlappingCharacter, OverlappingCharacter->GetCapsuleComponent(), 0, false, EmptyHit);

			TargetLocation = FVector::ZeroVector;
			MidTargetLocation = FVector::ZeroVector;
			SetActorTickEnabled(false);
		}
		else
		{
			//if we are dependent on a new streamed level, let's check here whether it's loaded or not.
			if (MapToLoad != NAME_None
				&& !UPA_BlueprintFunctionLibrary::GetLevelIsStreamed(this, MapToLoad))
				return;

			UWorld* World = GetWorld();
			if (!World) return;

			APA_ISMA_Door* CorrespondingDoorActor = NULL;
			FVector TransitionTarget;

			for (TActorIterator<APA_ISMA_Door> DoorItr(World); DoorItr; ++DoorItr)
			{
				APA_ISMA_Door *Door = *DoorItr;

				//UE_LOG(AnyLog, Log, TEXT("DOOR UID IS %d"), Door->DoorUID);

				if (Door != this
					&& Door->DoorUID == DoorUID)
				{
					CorrespondingDoorActor = Door;
					CorrespondingDoorActor->bSkipLevelStreaming = true;
					break;
				}
			}

			if (CorrespondingDoorActor)
			{
				//First Initialize the new camera
				TransitionTarget = CorrespondingDoorActor->TriggerDoorA->GetComponentToWorld().GetLocation();
				TransitionTarget.Z += OverlappingCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				FRotator CurrentCamRot = OverlappingCharacter->GetCameraBoom()->GetDesiredRotation();
				float CurrentCamDist = OverlappingCharacter->GetCameraBoom()->DesiredTargetArmLength;

				if (bShouldTransitionTeleport)
				{
					OverlappingCharacter->GetCameraBoom()->StartManualCameraTranslation(TransitionTarget, CurrentCamRot, CurrentCamDist, false, 5.f, true, false);
				}
				else
				{
					//Is this enough? Maybe we want to have a separate function to make the movement really instantaneous. 
					OverlappingCharacter->GetCameraBoom()->StartManualCameraTranslation(TransitionTarget, CurrentCamRot, CurrentCamDist, true, 5.f, true, false);
				}

				//Then move the player
				FVector TeleportLoc = CorrespondingDoorActor->TriggerDoorB->GetComponentToWorld().GetTranslation();
				TeleportLoc.Z += OverlappingCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				OverlappingCharacter->SetActorLocation(TeleportLoc, false, nullptr, ETeleportType::TeleportPhysics);
			}
			else
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("CORRESPONDING DOOR UID WASN'T FOUND! BE SURE TO ADD THE DOOR TO THE LEVEL."));
			}

			TargetLocation = FVector::ZeroVector;
			MidTargetLocation = FVector::ZeroVector;
			SetActorTickEnabled(false);
		}
	}
}

void APA_ISMA_Door::StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer)
{
	if (OverlappingCharacter)
	{
		if (TargetLocation != FVector::ZeroVector
			&& MidTargetLocation != FVector::ZeroVector)
		{
			//first check if our door is actually locked/unlocked
			if (RequiredKey != NULL && !bDoorIsUnlocked)
			{
				if (OverlappingCharacter->HasItemInInventory(RequiredKey))
				{
					bDoorIsUnlocked = true;
				}
				else
				{
					return;
				}
			}

			bFinishedMoveToDoor = false;
			OverlappingCharacter->RemoveActorFromContextArray(this, true); //since we can safely assume the door transports us elsewhere, let's empty out all the context actions and start anew.
			SetActorTickEnabled(true);
			OnReceiveContext();

			if (!bKeepCurrentCameraSettings
				&& !bShouldTeleport)
			{
				if (OverlappingCharacter->GetCameraBoom()->IsManuallyTranslating())
					OverlappingCharacter->GetCameraBoom()->StopManualCameraTranslation();

				OverlappingCharacter->GetCameraBoom()->SetDesiredArmLength(NewTargetArmLength);
			}
			else
			{
				OverlappingCharacter->GetCameraBoom()->SetCameraLock(true);
			}

			//if we want to stream a new level, start the loading from here, but only if we are not moving in reverse.
			//If moving in reverse, we can assume we already have done all the streaming necessary.
			if (MapToLoad != NAME_None
				&& !bSkipLevelStreaming)
			{
				TArray<FName> MapsLoaded;
				UPA_BlueprintFunctionLibrary::StreamLevelByName(OverlappingCharacter, MapToLoad, MapsLoaded);
			}
			//TODO: PERHAPS UNLOAD A STREAM LEVEL IF THE PLAYER IS MOVING IN REVERSE

			OnReceiveMoveToDoor(OverlappingCharacter, MidTargetLocation, TargetLocation, bPlayerIsMovingInReverse);
		}
	}
}

void APA_ISMA_Door::OnBeginDoorOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OverlappingCharacter = Cast<APiratesAdventureCharacter>(OtherActor);

	if (OverlappingCharacter
		&& OtherComp == OverlappingCharacter->GetCapsuleComponent()
		&& bFinishedMoveToDoor)
	{

		//If using teleport, we should always assume that Trigger A is the main trigger, and trigger B is simply there for transition sake.
		if (OverlappedComponent == TriggerDoorA)
		{
			//UE_LOG(AnyLog, Log, TEXT("OVERLAPPING TRIGGER A"));
			MidTargetLocation = OpenDoorASpriteComponent->GetComponentToWorld().GetTranslation();
			TargetLocation = TriggerDoorB->GetComponentToWorld().GetTranslation();
			
			if (RequiredKey != NULL && !bDoorIsUnlocked)
			{
				if (OverlappingCharacter->HasItemInInventory(RequiredKey))
				{
					FText HUDText = NSLOCTEXT("ContextActions", "UnlockDoor", "Unlock Door");
					OverlappingCharacter->AddActorToContextArray(this, HUDText, FLinearColor::Green);
				}
				else
				{
					FText HUDText = NSLOCTEXT("ContextActions", "LockedDoor", "Locked");
					OverlappingCharacter->AddActorToContextArray(this, HUDText, FLinearColor::Red, false);
				}
			}
			else
			{
				FText HUDText = NSLOCTEXT("ContextActions", "UseDoor", "Open Door");
				OverlappingCharacter->AddActorToContextArray(this, HUDText);
			}

			bPlayerIsMovingInReverse = false;	
		}
		else
		{
			MidTargetLocation = OpenDoorBSpriteComponent->GetComponentToWorld().GetTranslation();
			TargetLocation = TriggerDoorA->GetComponentToWorld().GetTranslation();

			bPlayerIsMovingInReverse = true;

			//Only add to the context array if this is not a teleport door. Otherwise, this will just be used as a stop and won't need a call to the context array. 
			if (!bShouldTeleport)
			{
				if (RequiredKey != NULL && !bDoorIsUnlocked)
				{
					if (OverlappingCharacter->HasItemInInventory(RequiredKey))
					{
						FText HUDText = NSLOCTEXT("ContextActions", "UnlockDoor", "Unlock Door");
						OverlappingCharacter->AddActorToContextArray(this, HUDText);
					}
					else
					{
						FText HUDText = NSLOCTEXT("ContextActions", "LockedDoor", "Locked");
						OverlappingCharacter->AddActorToContextArray(this, HUDText, FLinearColor::Red);
					}
				}
				else
				{
					FText HUDText = NSLOCTEXT("ContextActions", "UseDoor", "Open Door");
					OverlappingCharacter->AddActorToContextArray(this, HUDText);
				}
			}
			else if(bSkipLevelStreaming)
			{
				//if a teleport, we need to force start it, rather than using input.
				StartContextAction(OverlappingCharacter);
			}
		}
	}
}

void APA_ISMA_Door::OnEndDoorOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappingCharacter == OtherActor
		&& OtherComp == OverlappingCharacter->GetCapsuleComponent())
	{
		OverlappingCharacter->RemoveActorFromContextArray(this);
	}
}