// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PA_JumpBoid_Boid.generated.h"

struct FJumpBoidMesh;

UCLASS()
class PIRATESADVENTURE_API APA_JumpBoid_Boid : public APawn
{
	GENERATED_BODY()

public:
	APA_JumpBoid_Boid();

	void ApplyBehaviors();
	void CalculateRotation(FVector Steering);
	virtual void UpdateRotation(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Boid | Settings")
		float GetSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Boid | Settings")
		float GetTurnSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Boid | Settings")
		float GetRotationTurnSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Boid | Settings")
		float GetPerceptionRadius() const;

	UFUNCTION(BlueprintPure, Category = "Boid | Settings")
		FORCEINLINE bool ShouldUpdateRotation() const { return bShouldUpdateRotation; };

	//setters
	virtual void AssignStaticMesh(const FJumpBoidMesh& NewBoidMesh) const;
	virtual void SetShadowEnabled(bool bEnableShadow) const;
	virtual void SetMeshRotation(const FRotator& NewRotation) const;
	virtual void SetMeshScale(const FVector& NewScale) const;

	void SetSpeed(float NewSpeed) const;
	void SetShouldUpdateRotation(bool NewValue);
	void SetGoal(class AGoal* NewGoal);
	void SetFlock(class AFlock* NewFlock);

	//custom jump functions
	void SetGoal(class APiratesAdventureCharacter* NewGoal);
	void SetFlock(class APA_JumpBoid_Flock* NewFlock);

	bool HasWanderedTooFar();
	void CheckForObstacles();

	FORCEINLINE FBoxSphereBounds GetBoxSphereBounds() { return FBoxSphereBounds(GetActorLocation(), FVector(PerceptionRadius), PerceptionRadius); };

	FORCEINLINE USceneComponent* GetSceneComponent() const { return SceneComponent; }

	FORCEINLINE class APA_JumpBoid_Flock* GetFlockOwner() const { return FlockOwner; }

	void OnSpawned();

	uint8 ArrayIndex = 0;

protected:
	FVector SeekTowardsGoal();
	FVector Align();
	FVector Cohere();
	FVector Separate();

	void ApplyJumpForce();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UPA_JumpBoid_MoveComp* BoidMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties", DisplayName = "Owned By")
		class APA_JumpBoid_Flock* FlockOwner;

private: 

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	AActor* Obstacle;

	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToDetect;
	TArray<TSubclassOf<AActor>> ActorObstacles;

	//forces
	FVector Flocking;
	FVector Avoidance;

	FVector SpawnLocation;
	FVector AvoidLocation;
	FRotator Rotation;

	float PerceptionRadius = 300.0f;

	uint8 bShouldUpdateRotation : 1;

};
