// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "PA_Volume_CameraZoom.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_Volume_CameraZoom : public APhysicsVolume
{
	GENERATED_BODY()

public:
	
	APA_Volume_CameraZoom();

	// Called when actor enters a volume
	virtual void ActorEnteredVolume(class AActor* Other);

	// Called when actor leaves a volume, Other can be NULL
	virtual void ActorLeavingVolume(class AActor* Other);

	//Uses the bounds of the volume as the indicator of the zoom and locks the camera in the center of that. 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default")
		bool bZoomOnVolume;

	//If set to true, will update the camera rotation.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default")
		bool bUpdateCameraRotation;

	//If set to true, will update the camera distance. Only used when ZoomOnVolume is false.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default", meta = (EditCondition = "!ZoomOnVolume"))
		bool bUpdateCameraDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default")
		float ZoomSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default")
		float CameraDistanceMultiplier;

	//rotates the camera upon entering the volume. Is used when bUpdateCameraRotation is set to true
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default")
		FRotator TargetRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Zoom Default", meta = (EditCondition = "!ZoomOnVolume"))
		float TargetDistance;
	
	
};
