// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_Pawn_Boat.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerState_Boat.h"
#include "PA_BlueprintFunctionLibrary.h"

// Sets default values
APA_Pawn_Boat::APA_Pawn_Boat()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BoatMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BoatMesh"));
	//BoatMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//BoatMesh->bGenerateOverlapEvents = true;
	BoatMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoatMesh->SetSimulatePhysics(true);
	BoatMesh->WakeAllRigidBodies();
	//BoatMesh->OnComponentBeginOverlap.AddDynamic(this, &APA_Pawn_Boat::OnBeginOverlapWithBoat);
	SetRootComponent(BoatMesh);

	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetupAttachment(BoatMesh);
	//FlagMesh->AttachToComponent(BoatMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);

	SailMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SailMesh"));
	SailMesh->SetupAttachment(BoatMesh);
	//SailMesh->AttachToComponent(BoatMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);

	EntryCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryCollisionBox"));
	//EntryCollisionBox->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	EntryCollisionBox->SetupAttachment(RootComponent);
	EntryCollisionBox->SetBoxExtent(FVector(100.f));
	EntryCollisionBox->SetGenerateOverlapEvents(true);
	EntryCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APA_Pawn_Boat::OnBeginOverlapWithBoat);

	//CharacterMovementComponent = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("CharacterMovementComponent"));

}

// Called when the game starts or when spawned
void APA_Pawn_Boat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APA_Pawn_Boat::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//if (BoatMesh->GetPhysicsLinearVelocity().Y != 0)
	//{
	//	if (GetActorRotation().Yaw != BoatMesh->GetPhysicsLinearVelocity().Rotation().Yaw)
	//	{
	//		FRotator NewRot = GetActorRotation();
	//		NewRot.Yaw = BoatMesh->GetPhysicsLinearVelocity().Rotation().Yaw;

	//		AddActorLocalRotation(FMath::RInterpTo(GetActorRotation(), NewRot, GetWorld()->GetDeltaSeconds(), 5.f));
	//	}
	//}
}

// Called to bind functionality to input
//void APA_Pawn_Boat::SetupPlayerInputComponent(class UInputComponent* InputComponent)
//{
//	Super::SetupPlayerInputComponent(InputComponent);
//
//}

void APA_Pawn_Boat::OnBeginOverlapWithBoat(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OtherActor);
	if (Character && Character->GetCurrentPlayerState() != EPlayerStateType::GPS_GameBoat)
	{
		//UE_LOG(AnyLog, Log, TEXT("STARTED BOAT OVERLAP %s"), *(OtherActor->GetName()));

		Character->SetNewPlayerState(EPlayerStateType::GPS_GameBoat);
		UPA_PlayerState_Boat* PlayerState = Cast<UPA_PlayerState_Boat>(Character->GetRegisteredState(EPlayerStateType::GPS_GameBoat));
		if (PlayerState)
		{
			PlayerState->SetBoatInstance(this);
			Character->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));

			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			if(PlayerController)
				EnableInput(PlayerController);

			//Set up camera transition
			FLatentActionInfo LatentInfo;
			LatentInfo.UUID = 1;
			LatentInfo.Linkage = 0;
			LatentInfo.ExecutionFunction = FName(TEXT("Temp"));
			LatentInfo.CallbackTarget = this;

			FCameraComponentTransform NewCameraTransform;
			NewCameraTransform.Distance = 2250.f;
			NewCameraTransform.Rotation = FRotator(-5.f, 180.f, 0.f); //make 5.f to 0.f later, when you have made a working sideview of the ocean. 

			//UPA_BlueprintFunctionLibrary::MoveCameraComponentTo(Character, true, true, false, NewCameraTransform, true, true, 1.f, EAnimateLatentAction::Move, LatentInfo);
		}
	}
}

void APA_Pawn_Boat::ApplyImpulseToBoat(float Value)
{
	FRotator NewRotation = FRotator::ZeroRotator;
	NewRotation.Pitch = FMath::Clamp(GetActorRotation().Pitch, -45.f, 45.f);
	//NewRotation.Roll = FMath::Clamp(GetActorRotation().Roll, -45.f, 45.f);
	BoatMesh->AddForce(NewRotation.RotateVector(FVector(0.f, -17500000 * Value, 0.f)));
}