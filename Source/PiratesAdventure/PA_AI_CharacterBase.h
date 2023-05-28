// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PA_AI_CharacterBase.generated.h"

UCLASS()
class PIRATESADVENTURE_API APA_AI_CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APA_AI_CharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	//JUMPING
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement|Jumping")
		bool bIsJumping;

	//PATHFINDING
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		TArray<class APA_PatrolPoint*> PatrolPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		bool bCanJump;

	//Whether the AI should only fall distances it can jump back to later. If not, it will jump the distance that is marked as safe for the AI. 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		bool bFallOnlyJumpZDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		float MaxJumpZDistance;

	//Functions as a minimum distance by which we should start tracing, since AI can walk up certain ledges with just the stepheight.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		float MinJumpZDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		float MaxJumpYDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		float MaxFallZDistance;

	//How much before reaching the end of a navigation poly we need to start calculating the jump. Cannot be 0, because the character won't jump then.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		float JumpCheckTolerance;

	//Same as JumpCheckTolerance, but for walls. Since in order to jump on walls, we need to have a larger distance from the wall. Otherwise our AI just jumps into the wall.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Pathfinding")
		float WallCheckTolerance;

	UPROPERTY(BlueprintAssignable, Category = "Movement|Pathfinding")
		FLandedSignature AILandedDelegate;

	virtual void Landed(const FHitResult& Hit) override;	

	//virtual void OnConstruction(const FTransform& Transform) override;

	//virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	//on packaging the game, this function should be present, even if it ends up doing nothing, since other objects call this function. 
	UFUNCTION(BlueprintCallable, Category = "Debug", meta = (CallInEditor = "true", DisplayName = "Setup Debug Patrol Meshes"))
		void OnReceiveSetupDebugPatrolMeshes();

//#if WITH_EDITOR
//
//	void OnObjectSelected(UObject* Object);
//
//	void SetupDebugPatrolMeshes(bool bSetVisible);
//
//	bool bSelected;
//
//	//UFUNCTION(BlueprintImplementableEvent, Category = "Debug", meta = (DisplayName = "On Object Selected", CallInEditor = "true"))
//	//	void OnReceiveObjectSelected();
//
//	//UFUNCTION(BlueprintImplementableEvent, Category = "Debug", meta = (DisplayName = "On Object Deselected", CallInEditor = "true"))
//	//	void OnReceiveObjectDeselected();
//
//	UPROPERTY()
//		TArray<class UStaticMeshComponent*> PPointMeshes;
//
//	UPROPERTY()
//		TArray<class UStaticMeshComponent*> PConnectorMeshes;
//
//#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Movement|Pathfinding")
		class UStaticMesh* PathPointMeshTemplate;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Movement|Pathfinding")
		class UStaticMesh* PathConnectorMeshTemplate;
};
