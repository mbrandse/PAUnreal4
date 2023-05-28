// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#include "PA_AI_CharacterBase.h"
#include "PiratesAdventure.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "PA_PatrolPoint.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
APA_AI_CharacterBase::APA_AI_CharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	MaxJumpZDistance = 1200.f;
	MaxJumpYDistance = 1000.f;
	WallCheckTolerance = 175.f;
	JumpCheckTolerance = 75.f;
	MaxFallZDistance = 2500.f;

	bCanJump = false;
	bFallOnlyJumpZDistance = false;

//#if WITH_EDITOR
//
//	USelection::SelectObjectEvent.AddUObject(this, &APA_AI_CharacterBase::OnObjectSelected);
//	bSelected = false;
//
//#endif //WITH_EDITORONLY_DATA
}

//void APA_AI_CharacterBase::OnConstruction(const FTransform& Transform)
//{
//	Super::OnConstruction(Transform);
//
//#if WITH_EDITOR
//	SetupDebugPatrolMeshes(false);
//#endif //WITH_EDITOR
//}
//
//void APA_AI_CharacterBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//#if WITH_EDITOR
//	SetupDebugPatrolMeshes(true);
//#endif //WITH_EDITOR
//}
//
void APA_AI_CharacterBase::OnReceiveSetupDebugPatrolMeshes()
{
//#if WITH_EDITOR
//	SetupDebugPatrolMeshes(false);
//#endif //WITH_EDITOR
}

//#if WITH_EDITOR
//void APA_AI_CharacterBase::SetupDebugPatrolMeshes(bool bSetVisible)
//{
//	////First check if all our patrolpoints are valid
//	//for (int32 FirstCheck = 0; FirstCheck < PatrolPoints.Num(); FirstCheck++)
//	//{
//	//	if (!PatrolPoints[FirstCheck])
//	//		return;
//	//}
//
//	//If the length of the patrol point array does not equal that of the debug meshes, we can assume the array changed.
//	if (PPointMeshes.Num() != 0)
//	{
//		for (UStaticMeshComponent* PPoint : PPointMeshes)
//		{
//			if (PPoint)
//			{
//				PPoint->UnregisterComponent();
//				PPoint->DestroyComponent();
//			}
//		}
//	}
//
//	if (PConnectorMeshes.Num() != 0)
//	{
//		for (UStaticMeshComponent* PConnector : PConnectorMeshes)
//		{
//			if (PConnector)
//			{
//				PConnector->UnregisterComponent();
//				PConnector->DestroyComponent();
//			}
//		}
//	}
//
//	//then setup the new array
//	if (PathPointMeshTemplate 
//		&& PathConnectorMeshTemplate
//		&& PatrolPoints.Num() != 0)
//	{
//		for (int32 Idx = 0; Idx < PatrolPoints.Num(); Idx++)
//		{
//			if (PatrolPoints[Idx] != NULL) //since this array can have NULL entries
//			{
//				UStaticMeshComponent* PPoint = NewObject<UStaticMeshComponent>(this);
//				PPoint->CreationMethod = EComponentCreationMethod::UserConstructionScript;
//				PPoint->SetStaticMesh(PathPointMeshTemplate);
//				PPoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
//				PPoint->bIsEditorOnly = true;
//				PPoint->SetWorldLocation(PatrolPoints[Idx]->GetActorLocation());
//				PPoint->SetRelativeScale3D(FVector(0.5));
//				PPoint->SetRelativeRotation(FQuat::Identity);
//				PPoint->RegisterComponent();
//				PPoint->SetVisibility(bSetVisible);
//				PPoint->SetCastShadow(false);
//				PPoint->bCastDynamicShadow = false;
//				PPointMeshes.Add(PPoint);
//
//				if (PatrolPoints.IsValidIndex(Idx + 1)
//					&& PatrolPoints[Idx + 1] != NULL)
//				{
//					FRotator NewRotation = FRotationMatrix::MakeFromX(PatrolPoints[Idx + 1]->GetActorLocation() - PatrolPoints[Idx]->GetActorLocation()).Rotator();
//					float NewScale = FVector::Dist(PatrolPoints[Idx + 1]->GetActorLocation(), PatrolPoints[Idx]->GetActorLocation()) / 100; //since our debugmesh is based on 1m, aka 100cm
//
//					UStaticMeshComponent* PConnector = NewObject<UStaticMeshComponent>(this);
//					PConnector->CreationMethod = EComponentCreationMethod::UserConstructionScript;
//					PConnector->SetStaticMesh(PathConnectorMeshTemplate);
//					PConnector->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
//					PConnector->bIsEditorOnly = true;
//					PConnector->SetWorldLocation(PatrolPoints[Idx]->GetActorLocation());
//					PConnector->SetRelativeRotation(FRotator(NewRotation.Roll, NewRotation.Yaw, NewRotation.Pitch));
//					PConnector->SetWorldScale3D(FVector(0.15, NewScale, 0.15));
//					PConnector->RegisterComponent();
//					PConnector->SetVisibility(bSetVisible);
//					PConnector->SetCastShadow(false);
//					PConnector->bCastDynamicShadow = false;
//					PConnectorMeshes.Add(PConnector);
//				}
//			}
//		}
//	}
//}
//
//void APA_AI_CharacterBase::OnObjectSelected(UObject* Object)
//{
//	if (Object == this)
//	{
//		bSelected = true;
//		//OnReceiveObjectSelected();	
//
//		for (UStaticMeshComponent* PPoint : PPointMeshes)
//		{
//			if (PPoint)
//			{
//				PPoint->SetVisibility(true);
//			}
//		}
//
//		for (UStaticMeshComponent* PConnector : PConnectorMeshes)
//		{
//			if (PConnector)
//			{
//				PConnector->SetVisibility(true);
//			}
//		}
//	}
//	else if (!IsSelected() && bSelected)
//	{
//		bSelected = false;
//		//OnReceiveObjectDeselected();
//
//		for (UStaticMeshComponent* PPoint : PPointMeshes)
//		{
//			if (PPoint)
//			{
//				PPoint->SetVisibility(false);
//			}
//		}
//
//		for (UStaticMeshComponent* PConnector : PConnectorMeshes)
//		{
//			if (PConnector)
//			{
//				PConnector->SetVisibility(false);
//			}
//		}
//	}
//}
//#endif //WITH_EDITOR

// Called when the game starts or when spawned
void APA_AI_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

//#if WITH_EDITOR
//	for (UStaticMeshComponent* PPoint : PPointMeshes)
//	{
//		if (PPoint)
//		{
//			PPoint->SetVisibility(false);
//		}
//	}
//
//	for (UStaticMeshComponent* PConnector : PConnectorMeshes)
//	{
//		if (PConnector)
//		{
//			PConnector->SetVisibility(false);
//		}
//	}
//#endif
}

// Called every frame
void APA_AI_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APA_AI_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APA_AI_CharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	AILandedDelegate.Broadcast(Hit); //Broadcast our own delegate that we can bind in BP.
}