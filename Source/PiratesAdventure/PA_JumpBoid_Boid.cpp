// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.


#include "PA_JumpBoid_Boid.h"
#include "PA_JumpBoid_MoveComp.h"
#include "PA_JumpBoid_Flock.h"
#include "PiratesAdventureCharacter.h"

#include "Components/SphereComponent.h"

// Sets default values
APA_JumpBoid_Boid::APA_JumpBoid_Boid()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Scene Component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("ScceneComponent"));
	RootComponent = SceneComponent;

	//Movement Component
	BoidMovementComponent = CreateDefaultSubobject<UPA_JumpBoid_MoveComp>(TEXT("BoidMovementComponent"));
	BoidMovementComponent->UpdatedComponent = RootComponent;
	BoidMovementComponent->SetUpdateNavAgentWithOwnersCollisions(false);
	BoidMovementComponent->SetCanEverAffectNavigation(false);
}

void APA_JumpBoid_Boid::OnSpawned()
{
	SpawnLocation = GetActorLocation();
	Rotation = GetActorRotation();

	World = GetWorld();

	BoidMovementComponent->PostBoidBeginPlay();
}

FVector APA_JumpBoid_Boid::SeekTowardsGoal()
{
	return FVector(0.0f);
}

bool APA_JumpBoid_Boid::HasWanderedTooFar()
{
	return FVector::Dist(GetActorLocation(), SpawnLocation) > FlockOwner->GetJumpBoidSettings().WanderDistance;
}

void APA_JumpBoid_Boid::CheckForObstacles()
{

}

void APA_JumpBoid_Boid::ApplyBehaviors()
{

}

void APA_JumpBoid_Boid::ApplyJumpForce()
{

}

FVector APA_JumpBoid_Boid::Align()
{
	return FVector(0.0f);
}

FVector APA_JumpBoid_Boid::Cohere()
{
	return FVector(0.0f);
}


FVector APA_JumpBoid_Boid::Separate()
{
	return FVector(0.0f);
}

void APA_JumpBoid_Boid::AssignStaticMesh(const FJumpBoidMesh& NewBoidMesh) const
{
}

void APA_JumpBoid_Boid::SetShadowEnabled(const bool EnableShadow) const
{
}

void APA_JumpBoid_Boid::SetMeshScale(const FVector& NewScale) const
{
}

void APA_JumpBoid_Boid::SetMeshRotation(const FRotator& NewRotation) const
{
}

void APA_JumpBoid_Boid::SetSpeed(const float NewSpeed) const
{
	BoidMovementComponent->MaxSpeed = NewSpeed;
}

//void APA_JumpBoid_Boid::SetGoal(AGoal* NewGoal)
//{
//	Behaviors.GoalSeeking.Goal = NewGoal;
//}

void APA_JumpBoid_Boid::SetFlock(APA_JumpBoid_Flock* NewFlock)
{
	FlockOwner = NewFlock;
}

void APA_JumpBoid_Boid::CalculateRotation(FVector InSteering)
{
	Rotation = InSteering.Rotation();
}

void APA_JumpBoid_Boid::UpdateRotation(const float DeltaTime)
{
	SetActorRotation(FMath::Lerp(GetActorRotation(), Rotation, FlockOwner->GetJumpBoidSettings().RotationTurnSpeed * DeltaTime));
}

float APA_JumpBoid_Boid::GetSpeed() const
{
	return BoidMovementComponent->Velocity.Size();
}

float APA_JumpBoid_Boid::GetTurnSpeed() const
{
	return FlockOwner->GetJumpBoidSettings().TurnSpeed;
}

float APA_JumpBoid_Boid::GetRotationTurnSpeed() const
{
	return FlockOwner->GetJumpBoidSettings().RotationTurnSpeed;
}

float APA_JumpBoid_Boid::GetPerceptionRadius() const
{
	return PerceptionRadius;
}