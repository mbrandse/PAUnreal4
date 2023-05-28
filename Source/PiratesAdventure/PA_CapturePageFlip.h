// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "PA_CapturePageFlip.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPageFlipEnd);

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_CapturePageFlip : public ASceneCapture2D
{
	GENERATED_BODY()

public:

	APA_CapturePageFlip();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		class UPA_SkeletalMeshComponentNoPause* PageFlipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page Flip")
		float PageFlipAnimationLength;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Page Flip")
		bool bCanAnimatePageFlip;

	UFUNCTION(BlueprintCallable, Category = "PageFlip")
		void StartPageCapture();

	UFUNCTION(BlueprintImplementableEvent, Category = "Page Flip", meta = (DisplayName = "Start Page Capture"))
		void OnReceiveStartPageCapture();

	//Ends the capture functionality and starts the animation. Any actions to do after ending the capture functionality should be implemented before using this function
	UFUNCTION(BlueprintCallable, Category = "PageFlip")
		void EndPageCapture();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class APiratesAdventureCharacter* CaptureOwner;

	FTimerHandle FinishAnimationHandle;

	void OnFinishAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Page Flip", meta = (DisplayName = "Finish Animation"))
		void OnReceiveFinishAnimation();

	UPROPERTY(BlueprintAssignable, Category = "PageFlip")
		FOnPageFlipEnd OnPageFlipEnd;
};
