// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PA_JumpBoid_Flock.generated.h"

USTRUCT()
struct FJumpBoidMesh
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		class UStaticMesh* StaticMesh;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "StaticMesh != nullptr", ClampMin = 0.01f))
		FVector Scale = FVector(1.0f);

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "StaticMesh != nullptr"))
		FRotator Rotation = FRotator(90.0f, 180.0f, 0.0f);
};

USTRUCT()
struct FJumpBoidSettings
{
	GENERATED_USTRUCT_BODY()

	//TSubclassOf<class APA_JumpBoid_BoidStatic>

	//The static mesh assigned
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Boid Mesh")
		FJumpBoidMesh BoidMesh;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		uint8 bRandomSpeed : 1;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomSpeed", ClampMin = 1.0f))
		float MovementSpeed = 1200.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 1.0f))
		float BrakeForce = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 1.0f))
		float MovementAdaptationMaxSpeed = 2400.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomSpeed", ClampMin = 1.0f))
		float MovementSpeedMin = 500.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomSpeed", ClampMin = 1.0f))
		float MovementSpeedMax = 2400.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		uint8 bRandomPerceptionRadius : 1;

	// How far can each boid see?
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomPerceptionRadius", ClampMin = 1.0f))
		float PerceptionRadius = 300.0f;

	// The minimum range of how far each boid see?
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bRandomPerceptionRadius", ClampMin = 1.0f))
		float MinPerceptionRadius = 100.0f;

	// The maximum range of how far each boid see?
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bRandomPerceptionRadius", ClampMin = 1.0f))
		float MaxPerceptionRadius = 300.0f;

	// The maximum number of boids allowed in a boid's perception radius
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 2, UIMin = 2))
		uint16 MaxBoidsInPerceptionRadius = 200;

	// Should each boid reduce its perception radius when its maximum allowed neighboring boids have reached?
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		uint8 bShrinkPerceptionRadiusOnMaxBoids : 1;

	// The radius amount to reduce by when a boid has reached its maximum allowed neighboring boids
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Shrink Perception Radius by", meta = (EditCondition = "bShrinkPerceptionRadiusOnMaxBoids", ClampMin = 0.0f, UIMin = 0.0f))
		float PerceptionRadiusReduction = 50.0f;

	// The distance from the boid's location
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 1.0f))
		float AvoidanceDistance = 2000.0f;

	// How fast can each boid turn? Lower values = wider and slower turns, Higher values = narrow and faster turns
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 1.0f, ClampMax = 150.0f))
		float TurnSpeed = 10.0f;

	// Should the boids update thier rotation? If false, no rotation will be applied 
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		uint8 bUpdateBoidRotation : 1;

	// The rate of rotation of the boid mesh (Experiment between two extremes to find your desired result)
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bUpdateBoidRotation", ClampMin = 1.0f, ClampMax = 150.0f))
		float RotationTurnSpeed = 10.0f;

	// The distance from the spawn location
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 1.0f))
		float WanderDistance = 2000.0f;

	// Should the boids cast shadow?
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		uint8 bEnableShadows : 1;
};

UCLASS()
class PIRATESADVENTURE_API APA_JumpBoid_Flock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APA_JumpBoid_Flock();

	const FJumpBoidSettings& GetJumpBoidSettings() const { return JumpBoidSettings; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Enable this flock to spawn boids?
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties")
		uint8 bEnabled : 1;

	// Customize boid attributes
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bEnabled"))
		FJumpBoidSettings JumpBoidSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Flock")
		UWorld* World;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
