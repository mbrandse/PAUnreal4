// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Volume_CameraZoom.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "PA_PlayerSpringArmComponent.h"
#include "CineCameraComponent.h"

APA_Volume_CameraZoom::APA_Volume_CameraZoom()
{
	bZoomOnVolume = true;
	bUpdateCameraRotation = false;
	bUpdateCameraDistance = false;

	ZoomSpeed = 5.f;
	TargetDistance = 1400.f;
	CameraDistanceMultiplier = 1.15f;
}

void APA_Volume_CameraZoom::ActorEnteredVolume(class AActor* Other)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(Other);
	UWorld* World = GetWorld();

	if (Character && World)
	{
		float DefaultArmlength = Character->GetCameraBoom()->DefaultTargetArmLength;
		float DefaultAperture = Character->GetCameraBoom()->DefaultAperture;

		if (bZoomOnVolume)
		{
			FVector Origin, Extents;
			GetActorBounds(false, Origin, Extents);
			FMinimalViewInfo CameraView;
			Character->GetSideViewCameraComponent()->GetCameraView(0, CameraView);
			FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);

			//Just in case to prevent division by zero. Besides, if the viewportsize is zero, we weren't able to get the data anyway. 
			if (ViewportSize.X == 0 || ViewportSize.Y == 0)
				return;

			float hFOV = FMath::DegreesToRadians(Character->GetSideViewCameraComponent()->GetHorizontalFieldOfView());
			float vFOV = 2 * FMath::Atan(FMath::Tan(hFOV * 0.5) * (ViewportSize.Y / ViewportSize.X));
			float CameraDistance = FMath::Abs(Extents.Y / (FMath::Tan(hFOV / 2)));
			CameraDistance *= CameraDistanceMultiplier;

			FVector NewCameraLoc = Origin;
			FRotator NewCameraRot = bUpdateCameraRotation ? TargetRotation : Character->GetCameraBoom()->GetDesiredRotation();

			Character->GetCameraBoom()->StartManualCameraTranslation(NewCameraLoc, TargetRotation, CameraDistance, false, ZoomSpeed, true, true);
			Character->GetCameraBoom()->SetDesiredAperture((CameraDistance / DefaultArmlength) * DefaultAperture);
			Character->GetCameraBoom()->SetTrackingTarget(Character);
		}
		else if (bUpdateCameraRotation)
		{
			Character->GetCameraBoom()->SetCustomDesiredRotation(TargetRotation);
		}
		else if (bUpdateCameraDistance)
		{
			Character->GetCameraBoom()->SetDesiredArmLength(TargetDistance);
			Character->GetCameraBoom()->SetDesiredAperture((TargetDistance / DefaultArmlength) * DefaultAperture);
		}
	}
}

void APA_Volume_CameraZoom::ActorLeavingVolume(class AActor* Other)
{
	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(Other);

	if (Character)
	{
		if (bZoomOnVolume)
		{
			Character->GetCameraBoom()->StopManualCameraTranslation();
			Character->GetCameraBoom()->SetTrackingTarget(NULL);
			Character->GetCameraBoom()->ResetCameraToDefault();
		}
		else if (bUpdateCameraRotation || bUpdateCameraDistance)
		{
			Character->GetCameraBoom()->ResetCameraToDefault();
		}
	}
}





