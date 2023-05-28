// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "PA_PlayerSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_PlayerSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
protected:

	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime);

public:

	UPA_PlayerSpringArmComponent(const FObjectInitializer& ObjectInitializer);

	void OnRegister() override;

	// Necessary for our own initializations
	virtual void InitializeComponent() override;

	UPROPERTY()
		class UCameraComponent* AttachedCamera;

	//For initializing the camera the moment we have a boom owner; so we don't get any weird jump behavior.
	bool bCamIsInitialized;

	//Camera default values, for resetting purposes
	float DefaultTargetArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lag)
		float DefaultAperture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lag)
		float DefaultFocusDistance;

	FRotator DefaultDesiredRotation;

	//Camera follow
	FVector TopFrustum;
	FVector BottomFrustum;
	bool bRecordedCameraFrustum;
	bool bStartDelayFollowCam;
	float FollowZOffset;

	//Camera locks. We only lock the position axes, as those can be influenced by player movement.
	bool bLockXAxis;
	bool bLockYAxis;
	bool bLockZAxis;

	bool bManualCameraTranslation; //in case we want the camera to translate to a specific location, not dependent on player location.
	bool bManualCameraTranslationInstant; //in case we don't want any interpolation.
	FVector ManualCameraTranslationTarget;
	float ManualCameraTranslationLagSpeed;
	float ManualArmLengthTranslation;
	FRotator ManualCameraRotation;

	FVector GetLocationBasedOnLocks(FVector OriginalLocation, FVector NewLocation);

	//Camera Arm Length and rotation
	float DesiredTargetArmLength;
	float DesiredAperture;
	float DesiredFocusDistance;
	FRotator DesiredTargetRotation;

	//For forcing reset of camera frustum
	bool bForceRerecordCameraFrustum;
	void RerecordCameraFrustum();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SpringArm Settings")
		class APiratesAdventureCharacter* BoomOwner;

	//Rotation based variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringArm Settings")
		FRotator DesiredRotation;

	//Determines when the camera should start following a falling character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringArm Settings")
		float FollowZRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lag)
		float MaxCameraLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lag)
		float CameraArmLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lag)
		float FocusLagSpeed;

	//renamed to "GetCustomDesiredRotation" since "GetDesiredRotation" is now a function in SpringArmComponent that cannot be overridden and is constant.
	UFUNCTION(BlueprintPure, Category = Camera, meta = (DisplayName = "Get Desired Rotation"))
		FRotator GetCustomDesiredRotation() { return DesiredRotation; };

	//Renamed to "SetCustomDesiredRotation" to keep naming sense in line with the previous function.
	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Set Desired Rotation"))
		void SetCustomDesiredRotation(FRotator NewRotation) { DesiredTargetRotation = NewRotation; };

	UFUNCTION(BlueprintCallable, Category = Camera)
		void ResetDesiredRotationToDefault(FRotator NewRotation) { DesiredTargetRotation = DefaultDesiredRotation; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Set Desired Arm Length"))
		void SetDesiredArmLength(float NewArmLength) { DesiredTargetArmLength = NewArmLength; };

	UFUNCTION(BlueprintCallable, Category = Camera)
		void ResetDesiredArmLengthToDefault() { DesiredTargetArmLength = DefaultTargetArmLength; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Set Desired Aperture"))
		void SetDesiredAperture(float NewAperture) { DesiredAperture = NewAperture; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Reset Desired Aperture"))
		void ResetDesiredApertureToDefault() { DesiredAperture = DefaultAperture; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Set Desired Focus Distance"))
		void SetDesiredFocusDistance(float NewFocusDistance) { DesiredFocusDistance = NewFocusDistance; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Reset Desired Focus Distance"))
		void ResetDesiredFocusDistanceToDefault() { DesiredFocusDistance = DefaultFocusDistance; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Lock Camera X Movement"))
		void SetCameraXLock(bool bLocked) { bLockXAxis = bLocked; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Lock Camera Y Movement"))
		void SetCameraYLock(bool bLocked) { bLockYAxis = bLocked; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Lock Camera Z Movement"))
		void SetCameraZLock(bool bLocked) { bLockZAxis = bLocked; };

	UFUNCTION(BlueprintCallable, Category = Camera, meta = (DisplayName = "Lock Camera All Movement"))
		void SetCameraLock(bool bLocked);

	UFUNCTION(BlueprintCallable, Category = Camera)
		void ResetCameraToDefault();

	//Moves the camera manually to a location independent of player location. X Axis can generally be ignored, since that can also be handled with target arm length.
	//If TargetArmLength  is 0, it will ignore it and use the old targetarmlength instead
	UFUNCTION(BlueprintCallable, Category = Camera)
		void StartManualCameraTranslation(FVector ManualLocation, FRotator ManualRotation, float ManualArmLength = 0, bool bNoTransition = false, float TransitionSpeed = 5.f, bool bShouldRotate = true, bool bIgnoreXAxis = true);

	UFUNCTION(BlueprintCallable, Category = Camera)
		void StopManualCameraTranslation() { bManualCameraTranslation = false; };

	UFUNCTION(BlueprintCallable, Category = Camera)
		bool IsManuallyTranslating() { return bManualCameraTranslation; };

	UPROPERTY()
		class AActor* FollowTarget;

	/*Sets a follow target so the camera starts following something else from the player. 
	Leave to NULL to reset back to player. 
	Follow targets will be ignored if camera is manually translating.*/
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetFollowTarget(class AActor* Target) { FollowTarget = Target; };

	UPROPERTY()
		class AActor* TrackingTarget;

	/*Sets a tracking target to set the tracking distance of the camera. 
	Is used to fix the DOP to a target, rather than camera distance. 
	Leave NULL to reset back to camera distance. */
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetTrackingTarget(class AActor* Target) { TrackingTarget = Target; };

	UFUNCTION(BlueprintCallable, Category = Camera)
		void ActivateCameraFunctionality() { bCamIsInitialized = true; };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpringArm Settings")
		bool bDebug;
};

