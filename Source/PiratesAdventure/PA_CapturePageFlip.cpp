// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#include "PA_CapturePageFlip.h"
#include "Camera/PlayerCameraManager.h"
#include "PiratesAdventureCharacter.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PA_SkeletalMeshComponentNoPause.h"

APA_CapturePageFlip::APA_CapturePageFlip()
{
	PageFlipMesh = CreateDefaultSubobject<UPA_SkeletalMeshComponentNoPause>(TEXT("PageFlipMesh"));
	PageFlipMesh->SetupAttachment(GetCaptureComponent2D());
	PageFlipMesh->SetHiddenInGame(true);

	GetCaptureComponent2D()->bCaptureEveryFrame = false;
	GetCaptureComponent2D()->bCaptureOnMovement = false;
	GetCaptureComponent2D()->PostProcessBlendWeight = 0.f;

	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;

	PageFlipAnimationLength = 2.f;
}

void APA_CapturePageFlip::StartPageCapture()
{
	GetCaptureComponent2D()->CaptureScene();
	PageFlipMesh->SetHiddenInGame(false);

	OnReceiveStartPageCapture();
	SetActorTickEnabled(true);
}

void APA_CapturePageFlip::EndPageCapture()
{
	UWorld* World = GetWorld();

	if (PageFlipAnimationLength != 0.f && World)
	{
		World->GetTimerManager().SetTimer(FinishAnimationHandle, this, &APA_CapturePageFlip::OnFinishAnimation, PageFlipAnimationLength, false);
		bCanAnimatePageFlip = true;
	}
	else
	{
		SetActorTickEnabled(false);
		OnReceiveFinishAnimation();
		PageFlipMesh->SetHiddenInGame(true);
	}

	if (OnPageFlipEnd.IsBound())
		OnPageFlipEnd.Broadcast();
}

void APA_CapturePageFlip::OnFinishAnimation()
{
	SetActorTickEnabled(false);
	OnReceiveFinishAnimation();
	PageFlipMesh->SetHiddenInGame(true);
	bCanAnimatePageFlip = false;
}