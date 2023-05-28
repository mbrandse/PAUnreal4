// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerState_Boat.h"
#include "PiratesAdventure.h"

UPA_PlayerState_Boat::UPA_PlayerState_Boat()
{
	StateID = EPlayerStateType::GPS_GameBoat;
	bIsDefaultState = false;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UPA_PlayerState_Boat::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called every frame
//void UPA_PlayerState_Boat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

void UPA_PlayerState_Boat::BeginState()
{
	Super::BeginState();
}

void UPA_PlayerState_Boat::EndState(EPlayerStateType NewState)
{
	Super::EndState(NewState);
}

void UPA_PlayerState_Boat::MoveRight(float Value)
{
	//if (GetBoatInstance() != NULL)
	//{
		//this is code for the APiratesAdventureBoat class

		//GetBoatInstance->GetSkeletalMeshComponent()->AddIm
		//GetBoatInstance()->GetSkeletalMeshComponent()->AddForce(FVector(0.f, -20000000.0 * Value, 0.f), TEXT("Rudder_1"));
		//GetBoatInstance()->GetSkeletalMeshComponent()->AddForce(FVector(0.f, -20000.0 * Value, 0.f), TEXT("Boat_0"), true);
		//FRotator NewRotation = FRotator::ZeroRotator;
		//NewRotation.Pitch = FMath::Clamp(GetBoatInstance()->GetActorRotation().Pitch, -45.f, 45.f);
		//GetBoatInstance()->GetSkeletalMeshComponent()->AddForce(NewRotation.RotateVector(FVector(0.f, -17500000 * Value, 0.f))/*, TEXT("Boat_0"), false*/);
		//GetBoatInstance()->GetSkeletalMeshComponent()->AddRadialForce(GetBoatInstance()->GetSkeletalMeshComponent()->GetBoneLocation(TEXT("Root")), 64.f, -17500000 * Value, RIF_Linear, false);//(NewRotation.RotateVector(FVector(0.f, -17500000 * Value, 0.f))/*, TEXT("Boat_0"), false*/);
		//GetBoatInstance()->GetSkeletalMeshComponent()->AddForceAtLocation(FVector(0.f, -20000000.0 * Value, 0.f), GetBoatInstance()->GetSkeletalMeshComponent()->GetBoneLocation(TEXT("Root")));
	
		//this one uses the PA_Pawn_Boat class

		//FRotator NewRotation = FRotator::ZeroRotator;
		//NewRotation.Pitch = FMath::Clamp(GetBoatInstance()->GetActorRotation().Pitch, -45.f, 45.f);
		//GetBoatInstance()->BoatMesh->AddRadialForce(GetBoatInstance()->BoatMesh->GetBoneLocation(TEXT("Root")), 64.f, -17500000 * Value, RIF_Linear, false);//(NewRotation.RotateVector(FVector(0.f, -17500000 * Value, 0.f))/*, TEXT("Boat_0"), false*/);

		//GetBoatInstance()->ApplyImpulseToBoat(Value);
	//}
}