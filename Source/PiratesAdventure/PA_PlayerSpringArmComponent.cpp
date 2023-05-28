// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerSpringArmComponent.h"
#include "PiratesAdventure.h"
#include "GameFramework/SpringArmComponent.h"
#include "PiratesAdventureCharacter.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "CineCameraComponent.h"

UPA_PlayerSpringArmComponent::UPA_PlayerSpringArmComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bRecordedCameraFrustum = false;
	bStartDelayFollowCam = false;
	FollowZOffset = 0;
	DefaultAperture = 0.1f;
	DefaultFocusDistance = 1700.f;

	bLockXAxis = false;
	bLockYAxis = false;
	bLockZAxis = false;

	bCamIsInitialized = false;
	bForceRerecordCameraFrustum = true;
	bWantsInitializeComponent = true;
}

void UPA_PlayerSpringArmComponent::OnRegister()
{
	DefaultTargetArmLength = TargetArmLength;
	DesiredTargetArmLength = TargetArmLength;

	DefaultDesiredRotation = DesiredRotation;
	DesiredTargetRotation = DesiredRotation;

	DesiredAperture = DefaultAperture; //TODO: UNCOMMENT THIS. COMMENTED FOR TESTING SO WE CAN FIND THE BEST APERTURE IN GAME
	DesiredFocusDistance = DefaultFocusDistance;

	Super::OnRegister();
}

// Initializes the component, allowing us to register it with the player
void UPA_PlayerSpringArmComponent::InitializeComponent()
{
	Super::InitializeComponent();

	BoomOwner = Cast<APiratesAdventureCharacter>(GetOwner());
	if(BoomOwner)
		UE_LOG(AnyLog, Warning, TEXT("Registered %s component with %s and saved the BoomOwner"), *(GetClass()->GetName()), *(BoomOwner->GetName()));
}

//We will implement our own location here
void UPA_PlayerSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	//APiratesAdventureCharacter* BoomOwner = Cast<APiratesAdventureCharacter>(GetOwner());

	if (BoomOwner && !bDebug)
	{
		const float InverseCameraLagMaxTimeStep = (1.f / CameraLagMaxTimeStep);

		// Get the spring arm 'origin', the target we want to look at
		FVector ArmOrigin;
		if (FollowTarget != NULL)
		{
			ArmOrigin = FollowTarget->GetActorLocation() + TargetOffset;
		}
		else
		{
			ArmOrigin = GetComponentLocation() + TargetOffset;
		}
			
		FVector DesiredLoc = FVector::ZeroVector;
		float OriginalZ;

		if (bCamIsInitialized)
		{
			if (!bManualCameraTranslation)
			{
				//Interp the arm length if necessary
				if (TargetArmLength != DesiredTargetArmLength)
				{
					TargetArmLength = FMath::FInterpTo(TargetArmLength, DesiredTargetArmLength, DeltaTime, CameraArmLagSpeed);
					bRecordedCameraFrustum = false; //since target arm distance directly affects camera frustum, we need to update it if this is updating.
				}

				//Camera Jump Lock. if we are jumping, the camera should only follow when falling (after a certain offset) or when the character lands on a spot with a different Z value than before.
				if (BoomOwner->GetCharacterMovement()->MovementMode == MOVE_Falling)
				{
					//This is still kinda weird. Find a good number and keep in mind that negative numbers behave differently
					FVector OwnerDistance = FVector::ZeroVector;
					OwnerDistance.Z = BoomOwner->GetActorLocation().Z;
					FVector BottomFrustumDistance = FVector::ZeroVector;
					BottomFrustumDistance.Z = BottomFrustum.Z;

					if (FVector::Dist(OwnerDistance, BottomFrustumDistance) < FMath::Abs(FollowZOffset))
					{
						bStartDelayFollowCam = true;
					}

					if (bStartDelayFollowCam)
					{
						OriginalZ = BoomOwner->GetActorLocation().Z + FollowZOffset;
					}
					else
					{
						OriginalZ = PreviousDesiredLoc.Z;
					}

					bRecordedCameraFrustum = false;
				}
				else
				{
					//(Re)record camera frustum, so we can use it to determine falling distance.
					if (!bRecordedCameraFrustum
						|| bForceRerecordCameraFrustum)
					{
						FVector TempA, TempB;
						FVector ActorLoc = BoomOwner->GetActorLocation();
						ActorLoc.Z -= BoomOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
						UPA_BlueprintFunctionLibrary::GetPerspectiveCameraFrustumEdges(ActorLoc, BoomOwner->GetSideViewCameraComponent(), this, TopFrustum, TempA, BottomFrustum, TempB);
						FollowZOffset = FMath::Abs(ActorLoc.Z - BottomFrustum.Z) * FollowZRatio;
						bRecordedCameraFrustum = true;

						if (bForceRerecordCameraFrustum) bForceRerecordCameraFrustum = false;

						//UE_LOG(AnyLog, Log, TEXT("CURRENT FOLLOW OFFSET Z IS %f FROM %f"), FollowZOffset, BottomFrustum.Z);
					}

					bStartDelayFollowCam = false;
					OriginalZ = ArmOrigin.Z;
				}

				// We lag the target, not the actual camera position, so rotating the camera around does not have lag
				//For this game, we remove the conditional bLagCamera, and instead assume we always want to lag it. 
				DesiredLoc = ArmOrigin;

				//Update our camera lag speed to our owner's speed. If falling for too long, the character becomes too fast making it hard for the camera to catch up. 
				//Which is why we increase the lag speed so the camera catches up faster. 
				float BoomOwnerZVelocity = FMath::Abs(BoomOwner->GetCharacterMovement()->Velocity.Z);
				float FinalCameraLagSpeed = CameraLagSpeed;
				if (BoomOwnerZVelocity > 2048.f)
				{
					//First zero it out.
					BoomOwnerZVelocity -= 2048.f;
					//then clamp it.
					BoomOwnerZVelocity = FMath::Clamp(BoomOwnerZVelocity, 0.f, 2048.f);
					//Then normalize it to a range of 0 - 1;
					BoomOwnerZVelocity /= 2048.f;

					FinalCameraLagSpeed = CameraLagSpeed + ((MaxCameraLagSpeed - CameraLagSpeed) * BoomOwnerZVelocity);
				}

				if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && FinalCameraLagSpeed > 0.f)
				{
					const FVector ArmMovementStep = (ArmOrigin - PreviousArmOrigin) * (CameraLagMaxTimeStep / DeltaTime);
					FVector LerpTarget = PreviousArmOrigin;
					float RemainingTime = DeltaTime;
					while (RemainingTime > KINDA_SMALL_NUMBER)
					{
						const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
						LerpTarget += ArmMovementStep * (LerpAmount * InverseCameraLagMaxTimeStep);
						LerpTarget.Z = OriginalZ;
						RemainingTime -= LerpAmount;

						DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, LerpTarget, LerpAmount, FinalCameraLagSpeed);
						//Reset any axes that are locked down.
						DesiredLoc = GetLocationBasedOnLocks(PreviousDesiredLoc, DesiredLoc);
						PreviousDesiredLoc = DesiredLoc;
					}
				}
				else
				{
					DesiredLoc.Z = OriginalZ;
					DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, FinalCameraLagSpeed);
					//Reset any axes that are locked down
					DesiredLoc = GetLocationBasedOnLocks(PreviousDesiredLoc, DesiredLoc);
				}

				// Clamp distance if requested
				bool bClampedDist = false;
				if (CameraLagMaxDistance > 0.f)
				{
					const FVector FromOrigin = DesiredLoc - ArmOrigin;
					if (FromOrigin.SizeSquared() > FMath::Square(CameraLagMaxDistance))
					{
						DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
						//Reset any axes that are locked down
						DesiredLoc = GetLocationBasedOnLocks(PreviousDesiredLoc, DesiredLoc);
						bClampedDist = true;
					}
				}

				//Interp the rotation if necessary
				if (DesiredRotation != DesiredTargetRotation)
				{
					DesiredRotation = FMath::RInterpTo(DesiredRotation, DesiredTargetRotation, DeltaTime, CameraRotationLagSpeed);
				}
			}
			else //From here on is bManualCameraTranslation
			{
				if (!bManualCameraTranslationInstant)
				{
					DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, ManualCameraTranslationTarget, DeltaTime, ManualCameraTranslationLagSpeed);
					DesiredRotation = FMath::RInterpTo(DesiredRotation, ManualCameraRotation, DeltaTime, ManualCameraTranslationLagSpeed);
					if (ManualArmLengthTranslation != 0)
						TargetArmLength = FMath::FInterpTo(TargetArmLength, ManualArmLengthTranslation, DeltaTime, ManualCameraTranslationLagSpeed);
				}
				else
				{
					DesiredLoc = ManualCameraTranslationTarget;
					DesiredRotation = ManualCameraRotation;
					if (ManualArmLengthTranslation != 0)
						TargetArmLength = ManualArmLengthTranslation;
				}
			}

			//Calculate focus distance, for DOP reasons
			//Set focus distance outside of main calculation loop, since we do that regardless of manual translation or not.
			if (TrackingTarget != NULL)
			{
				FVector Distance = BoomOwner->GetSideViewCameraComponent()->GetComponentLocation() - TrackingTarget->GetActorLocation();
				BoomOwner->GetSideViewCameraComponent()->FocusSettings.ManualFocusDistance = FMath::FInterpTo(Distance.Size(), DesiredFocusDistance, DeltaTime, FocusLagSpeed);
			}
			else
			{
				BoomOwner->GetSideViewCameraComponent()->FocusSettings.ManualFocusDistance = FMath::FInterpTo(BoomOwner->GetSideViewCameraComponent()->FocusSettings.ManualFocusDistance, DesiredFocusDistance, DeltaTime, FocusLagSpeed);

				//BoomOwner->GetSideViewCameraComponent()->FocusSettings.ManualFocusDistance = TargetArmLength;
			}

			//interp the cinecam aperture (controls the distance of the DOF)
			if (BoomOwner->GetSideViewCameraComponent()->CurrentAperture != DesiredAperture)
			{
				BoomOwner->GetSideViewCameraComponent()->CurrentAperture = FMath::FInterpTo(BoomOwner->GetSideViewCameraComponent()->CurrentAperture, DesiredAperture, DeltaTime, FocusLagSpeed);
			}
		}
		else
		{
			//first make sure we teleport the camera to the main character location. 
			DesiredLoc = ArmOrigin;
			DesiredRotation = DesiredTargetRotation;
		}

		PreviousDesiredRot = DesiredRotation;

		PreviousArmOrigin = ArmOrigin;
		PreviousDesiredLoc = DesiredLoc;

		DesiredLoc -= DesiredRotation.Vector() * TargetArmLength;
		DesiredLoc += FRotationMatrix(DesiredRotation).TransformVector(SocketOffset);

		FTransform WorldCamTM(DesiredRotation, DesiredLoc);
		FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentToWorld());

		// Update socket location/rotation
		RelativeSocketLocation = RelCamTM.GetLocation();
		RelativeSocketRotation = RelCamTM.GetRotation();

		UpdateChildTransforms();

		if (!bCamIsInitialized)
		{
			//Then, when we have updated our first transforms, update our camera frustum and disable camera initialization.
			FVector TempA, TempB;
			FVector ActorLoc = BoomOwner->GetActorLocation();
			ActorLoc.Z -= BoomOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			UPA_BlueprintFunctionLibrary::GetPerspectiveCameraFrustumEdges(ActorLoc, BoomOwner->GetSideViewCameraComponent(), this, TopFrustum, TempA, BottomFrustum, TempB);
			FollowZOffset = FMath::Abs(ActorLoc.Z - BottomFrustum.Z) * FollowZRatio;
			bRecordedCameraFrustum = true;

			bCamIsInitialized = true;
		}
	}
	else
	{
		//Super it if we don't have a BoomOwner, so we can just get default behavior instead.
		//Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	}
}

FVector UPA_PlayerSpringArmComponent::GetLocationBasedOnLocks(FVector OriginalLocation, FVector NewLocation)
{
	FVector ResultLocation = FVector::ZeroVector;

	ResultLocation.X = bLockXAxis ? OriginalLocation.X : NewLocation.X;
	ResultLocation.Y = bLockYAxis ? OriginalLocation.Y : NewLocation.Y;
	ResultLocation.Z = bLockZAxis ? OriginalLocation.Z : NewLocation.Z;

	return ResultLocation;
}

void UPA_PlayerSpringArmComponent::SetCameraLock(bool bLocked)
{
	bLockXAxis = bLocked;
	bLockYAxis = bLocked;
	bLockZAxis = bLocked;
}

void UPA_PlayerSpringArmComponent::StartManualCameraTranslation(FVector ManualLocation, FRotator ManualRotation, float ManualArmLength, bool bNoTransition, float TransitionSpeed, bool bShouldRotate, bool bIgnoreXAxis)
{
	ManualCameraTranslationTarget = ManualLocation;

	if (bIgnoreXAxis)
	{
		ManualCameraTranslationTarget.X = PreviousDesiredLoc.X;
	}

	ManualCameraTranslationLagSpeed = TransitionSpeed;
	ManualArmLengthTranslation = ManualArmLength;

	if (bShouldRotate)
	{
		ManualCameraRotation = ManualRotation;
	}
	else
	{
		ManualCameraRotation = DesiredRotation;
	}

	bManualCameraTranslationInstant = bNoTransition;
	bManualCameraTranslation = true;
}

void UPA_PlayerSpringArmComponent::ResetCameraToDefault()
{
	DesiredTargetArmLength = DefaultTargetArmLength;
	DesiredTargetRotation = DefaultDesiredRotation;
	DesiredAperture = DefaultAperture;
	DesiredFocusDistance = DefaultFocusDistance;
}

void UPA_PlayerSpringArmComponent::RerecordCameraFrustum()
{
	bForceRerecordCameraFrustum = true;
}