// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
//#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "PA_Interface_ContextAction.h"
#include "Navigation/PathFollowingComponent.h"
#include "PA_StaticLibrary.h"
#include "PA_ISMA_Door.generated.h"

/**
 * 
 */
UCLASS()
//class PIRATESADVENTURE_API APA_ISMA_Door : public AStaticMeshActor, public IPA_Interface_ContextAction
class PIRATESADVENTURE_API APA_ISMA_Door : public AActor, public IPA_Interface_ContextAction
{
	GENERATED_BODY()
	
public:
	APA_ISMA_Door(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void OnBeginDoorOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndDoorOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Start Context Action"))
		void OnReceiveContext();

	//UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Move To Door A"))
	//	void OnReceiveMoveToDoorA(class APiratesAdventureCharacter* PlayerCharacter, FVector MidTarget, FVector FinalTarget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Move To Door"))
		void OnReceiveMoveToDoor(class APiratesAdventureCharacter* PlayerCharacter, FVector MidTarget, FVector FinalTarget, bool bIsMovinginReverse);

	UFUNCTION(BlueprintCallable, Category = "Door")
		void FinishMoveToDoor() { bFinishedMoveToDoor = true; };

	bool bFinishedMoveToDoor;

	bool bMovingToMidTarget;

	//This prevents the end overlap functions to be called when we are already using a context action.
	bool bIsUsingContextAction;

	//This allows us for applying animations for both directions. Reverse refers to moving from B to A.
	bool bPlayerIsMovingInReverse;

	//Since we don't want to stream levels if the player has arrived at a streamed level door.
	bool bSkipLevelStreaming;

	class APiratesAdventureCharacter* OverlappingCharacter;

	//If you need the door to be lockable, add an item here. Otherwise, keep it empty.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		class UPA_Asset_Item* RequiredKey;

	//TODO; ADD FUNCTIONALITY FOR SAVING SO WE DON'T HAVE TO CONSTANTLY KEEP UNLOCKING DOORS. 
	UPROPERTY()
		bool bDoorIsUnlocked;

	//If set to teleport, the camera will move to the target location by itself, rather than use the player location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		bool bShouldTeleport = false;

	//If set to true, the camera will interpolate to the teleport location. If not, the camera will move there instantly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "bShouldTeleport"))
		bool bShouldTransitionTeleport = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "bShouldTransitionTeleport"))
		float TransitionSpeed = 5.f;

	//If we need to stream a new level, fill this in. Otherwise, just leave it.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door", meta = (EditCondition = "bShouldTeleport"))
		FName MapToLoad;

	//In case we have a teleporting door, it will search for a door with a matching UID. This is implemented so that we can even get doors that are not loaded at the moment.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "bShouldTeleport"))
		int32 DoorUID = 0;

	//If set to true, the camera follows the player with the default settings. 
	//NOTE: If bShouldTeleport is set to true, that will take precedence.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		bool bKeepCurrentCameraSettings = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "!bKeepCurrentCameraSettings"))
		float NewTargetArmLength = 500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerDoorA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TriggerDoorB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* OpenDoorASpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* OpenDoorBSpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* DoorASpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		class UBillboardComponent* DoorBSpriteComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Depth Switch", meta = (AllowPrivateAccess = "true"))
	//	USplineComponent* DepthPath; //use this to later create an array of path points.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent* SourceComponent;

	UPROPERTY()
		FVector TargetLocation;

	UPROPERTY()
		FVector MidTargetLocation;

	//Start context action interface
	virtual void StartContextAction(class APiratesAdventureCharacter* OverlappingPlayer);
	//End interface		
};
