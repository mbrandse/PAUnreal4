// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_LevelObject_TransCamera.h"
#include "PiratesAdventure.h"

// Sets default values
APA_LevelObject_TransCamera::APA_LevelObject_TransCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	//CollisionCapsule->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CollisionCapsule->SetupAttachment(RootComponent);
	CollisionCapsule->SetGenerateOverlapEvents(true);
	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &APA_LevelObject_TransCamera::OnCompStartOverlap);
	CollisionCapsule->OnComponentEndOverlap.AddDynamic(this, &APA_LevelObject_TransCamera::OnCompEndOverlap);

}

// Called when the game starts or when spawned
void APA_LevelObject_TransCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void APA_LevelObject_TransCamera::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	if (CameraOwner != NULL)
	{
		float FullLength = CollisionCapsule->GetScaledCapsuleRadius() - UpperLimit;
		float CurrentLength = FMath::Abs(GetActorLocation().Y - CameraOwner->GetActorLocation().Y);
		float Percentage = CurrentLength / FullLength;
		Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
		
		CameraOwner->GetCameraBoom()->TargetArmLength = FMath::LerpStable(TargetCameraTransform.Distance, DefaultCameraTransform.Distance, Percentage);
		CameraOwner->GetCameraBoom()->DesiredRotation = FMath::LerpStable(TargetCameraTransform.Rotation, DefaultCameraTransform.Rotation, Percentage);
		CameraOwner->GetCameraBoom()->RelativeLocation = FMath::LerpStable(TargetCameraTransform.Location, DefaultCameraTransform.Location, Percentage);
	}

}

void APA_LevelObject_TransCamera::OnCompStartOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	CameraOwner = Cast<APiratesAdventureCharacter>(OtherActor);

	if (CameraOwner != NULL)
	{
		DefaultCameraTransform.Distance = CameraOwner->CameraDefaultZoom;
		DefaultCameraTransform.Rotation = CameraOwner->CameraDefaultRotation;
		DefaultCameraTransform.Location = CameraOwner->CameraDefaultLocation;

		CameraOwnerEntryLocation = CameraOwner->GetActorLocation();

		SetActorTickEnabled(true);
	}
}

void APA_LevelObject_TransCamera::OnCompEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CameraOwner != NULL)
	{
		CameraOwner->GetCameraBoom()->TargetArmLength = CameraOwner->CameraDefaultZoom;
		CameraOwner->GetCameraBoom()->DesiredRotation = CameraOwner->CameraDefaultRotation;
		CameraOwner->GetCameraBoom()->RelativeLocation = CameraOwner->CameraDefaultLocation;

		CameraOwner = NULL;
		SetActorTickEnabled(false);
	}
}