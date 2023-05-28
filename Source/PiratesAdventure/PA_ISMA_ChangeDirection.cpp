// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_ChangeDirection.h"
#include "PiratesAdventure.h"
#include "Components/BoxComponent.h"
#include "PiratesAdventureCharacter.h"
#include "Engine/InputDelegateBinding.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PA_PlayerSpringArmComponent.h"
#include "PA_BlueprintFunctionLibrary.h"

// Sets default values
APA_ISMA_ChangeDirection::APA_ISMA_ChangeDirection()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SourceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SourceComponent"));
	SourceComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SourceComponent);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBottom"));
	Trigger->ShapeColor = FColor::Green;
	Trigger->SetGenerateOverlapEvents(true);
	Trigger->SetBoxExtent(FVector(100.f));
	//Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Trigger->SetupAttachment(RootComponent);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_ChangeDirection::OnBeginDirectionSwitchOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_ChangeDirection::OnEndDirectionSwitchOverlap);
}

void APA_ISMA_ChangeDirection::EnableContextInput(class APlayerController* PlayerController)
{
	if (PlayerController)
	{
		// If it doesn't exist create it and bind delegates
		if (!InputComponent)
		{
			InputComponent = NewObject<UInputComponent>(this);
			InputComponent->RegisterComponent();
			InputComponent->bBlockInput = bBlockInput;
			InputComponent->Priority = InputPriority;

			//in this bit we bind our own key for this instance
			InputComponent->BindAction("Context", IE_Pressed, this, &APA_ISMA_ChangeDirection::OnContext);

			if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
			{
				UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent);
			}
		}
		else
		{
			// Make sure we only have one instance of the InputComponent on the stack
			PlayerController->PopInputComponent(InputComponent);
		}

		PlayerController->PushInputComponent(InputComponent);
	}
}

void APA_ISMA_ChangeDirection::OnContext()
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (Character)
	{
		//TODO: THIS ELEMENT CAUSES ISSUES WITH ROTATIONAL SITUATIONS THAT HAVE BEEN HARDCODED. REPLACE ALL ROTATION CODE WITH CODE THAT'S BASED ON CAMERA ANGLE.

		//Set up Camera latent info
		FLatentActionInfo LatentCamInfo;
		LatentCamInfo.UUID = 1;
		LatentCamInfo.Linkage = 0;
		LatentCamInfo.ExecutionFunction = FName(TEXT("No target")); //consider having a callback so we can disable input during transitions
		LatentCamInfo.CallbackTarget = this;

		//Set up Character latent info
		FLatentActionInfo LatentInfo;
		LatentInfo.UUID = 0;
		LatentInfo.Linkage = 0;
		LatentInfo.ExecutionFunction = FName(TEXT("No target"));
		LatentInfo.CallbackTarget = this;

		UPA_PlayerSpringArmComponent* CameraBoom = Cast<UPA_PlayerSpringArmComponent>(Character->GetCameraBoom());

		if (CameraBoom)
		{
			UE_LOG(AnyLog, Log, TEXT("TRYING TO ROTATE CHARACTER"));

			//temp rotation hardcoded for testing. first complete this and then change the onoverlap to a hit, so we can use the impact location for dynamic rotation.
			FVector TargetLocation = GetActorLocation();
			TargetLocation.Z = Character->GetActorLocation().Z;
			FRotator TargetRotation = Character->GetActorRotation();

			FCameraComponentTransform TargetCamTransform;
			TargetCamTransform.Distance = Character->GetCameraBoom()->TargetArmLength;
			TargetCamTransform.Location = Character->GetCameraBoom()->RelativeLocation;

			//FRotator GeneralRotation = EntryDirection.GetAbs().Rotation();
			//TargetRotation.Yaw = GeneralRotation.Yaw + 90.f;
			//TargetCamTransform.Rotation = FRotator(-15.f, GeneralRotation.Yaw, 0.f);

			//base it on camera rotation
			if (CameraBoom->GetDesiredRotation().Yaw == 180.f)
			{
				TargetCamTransform.Rotation = FRotator(-15.f, 90.f, 0.f);
				TargetRotation.Yaw = 180.f;
			}
			else
			{
				TargetCamTransform.Rotation = FRotator(-15.f, 180.f, 0.f);
				TargetRotation.Yaw = 90.f;
			}

			//TargetCamTransform.Rotation = CameraBoom->GetNewDesiredRotation();
			//TargetCamTransform.Rotation.Yaw += 90.f;
			//TargetRotation.Yaw = CameraBoom->GetNewDesiredRotation().Yaw + 90.f;
			
			//if (CameraBoom->GetNewDesiredRotation().Yaw != 90.f)
			//{
			//	TargetRotation.Yaw = 180.f;
			//	TargetCamTransform.Rotation = FRotator(-15.f, 90.f, 0.f);
			//}
			//else
			//{
			//	TargetRotation.Yaw = 90.f;
			//	TargetCamTransform.Rotation = FRotator(-15.f, 180.f, 0.f);
			//}

			UKismetSystemLibrary::MoveComponentTo(Character->GetCapsuleComponent(), TargetLocation, TargetRotation, true, true, 0.5, false, EMoveComponentAction::Move, LatentInfo);
			//UPA_BlueprintFunctionLibrary::MoveCameraComponentTo(Character, false, true, false, TargetCamTransform, true, true, 0.5, EAnimateLatentAction::Move, LatentCamInfo);
		}
	}
}

void APA_ISMA_ChangeDirection::OnBeginDirectionSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	UE_LOG(AnyLog, Log, TEXT("HIT THE DIRECTION SWITCH %s %s"), *(OverlappedComponent->GetName()), *(SweepResult.ImpactNormal.ToString()));

	if (Character)
	{
		EntryDirection = SweepResult.ImpactNormal;
		EnableContextInput(Cast<APlayerController>(Character->GetController()));

		//do something
	}
}

void APA_ISMA_ChangeDirection::OnEndDirectionSwitchOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character)
	{
		EntryDirection = FVector::ZeroVector;
		DisableInput(GetWorld()->GetFirstPlayerController());
	}
}

