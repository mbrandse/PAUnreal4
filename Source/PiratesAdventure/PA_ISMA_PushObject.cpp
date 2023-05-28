// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_ISMA_PushObject.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "Engine/InputDelegateBinding.h"
#include "PA_PlayerState_Push.h"

APA_ISMA_PushObject::APA_ISMA_PushObject()
{
	GetStaticMeshComponent()->SetNotifyRigidBodyCollision(false);
	GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetStaticMeshComponent()->SetSimulatePhysics(false);
	GetStaticMeshComponent()->SetConstraintMode(EDOFMode::YZPlane);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &APA_ISMA_PushObject::OnBeginPushOverlap);
	GetStaticMeshComponent()->OnComponentEndOverlap.AddDynamic(this, &APA_ISMA_PushObject::OnEndPushOverlap);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APA_ISMA_PushObject::BeginPlay()
{
	Super::BeginPlay();
	//try to find a way to set the mass scale without ending up with a function that calls GetSimplePhysicalMaterial eventually.
	GetStaticMeshComponent()->SetAllMassScale(20.f); //putting this in the constructor causes it to call GetSimplePhysicalMaterial somewhere, which causes errors somewhere in the engine that hinder cooking/localization. 
}

void APA_ISMA_PushObject::Tick(float DeltaTime)
{
	//UE_LOG(AnyLog, Log, TEXT("CURRENT PUSH OBJECT PHYSICS VELOCITY IS %s"), *(GetStaticMeshComponent()->GetPhysicsLinearVelocity().ToString()));
}

void APA_ISMA_PushObject::EnableContextInput(class APlayerController* PlayerController)
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
			InputComponent->BindAction("Context", IE_Pressed, this, &APA_ISMA_PushObject::OnStartContext);
			InputComponent->BindAction("Context", IE_Released, this, &APA_ISMA_PushObject::OnStopContext);

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

void APA_ISMA_PushObject::OnStartContext()
{
	UE_LOG(AnyLog, Log, TEXT("BINDING TO PUSH OBJECT CONTEXT"));

	if (PushOwner
		&& PushOwner->GetCharacterMovement()->Velocity.Z == 0.f
		&& PushOwner->CurrentStateType == EPlayerStateType::GPS_GameNormal)
	{
		PushOwner->SetNewPlayerState(EPlayerStateType::GPS_GamePush);
		GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
		GetStaticMeshComponent()->SetSimulatePhysics(true);
		AttachToActor(PushOwner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));

		SetActorTickEnabled(true);
	}
}

void APA_ISMA_PushObject::OnStopContext()
{
	if (PushOwner
		&& PushOwner->CurrentStateType == EPlayerStateType::GPS_GamePush)
	{
		PushOwner->SetNewPlayerState(EPlayerStateType::GPS_GameNormal);
		//GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
		//GetStaticMeshComponent()->SetMassScale(NAME_None, 20.f);
		//GetStaticMeshComponent()->SetAllMassScale(20.f);
		//GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//DetachRootComponentFromParent(true);
	}
}

void APA_ISMA_PushObject::OnBeginPushOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(AnyLog, Log, TEXT("GOT AN OVERLAP FROM %s"), *(OtherActor->GetName()));

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character && !bCanOnlyBePushedByPressure && Character->GetCurrentPlayerState() != EPlayerStateType::GPS_GamePush)
	{
		PushOwner = Character;

		EnableContextInput(GetWorld()->GetFirstPlayerController());
	}
}

void APA_ISMA_PushObject::OnEndPushOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(AnyLog, Log, TEXT("STOPPED AN OVERLAP FROM %s"), *(OtherActor->GetName()));

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);

	if (Character && !bCanOnlyBePushedByPressure && Character->GetCurrentPlayerState() != EPlayerStateType::GPS_GamePush)
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
	}
}