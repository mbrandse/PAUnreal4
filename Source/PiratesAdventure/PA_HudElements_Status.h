// Fill out your copyright notice in the Description page of Project Settings.

//The HUdElements_Status is going to take care of all elements on screen during gameplay that the player cannot directly control on its own.
/*

1. Eventually, these elements should be alterable through the options (their placement and visibility)
2. Only one element can never be removed; the 

*/

#pragma once

#include "Blueprint/UserWidget.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_Status.generated.h"

//USTRUCT()
//struct FVesselbarSizeProfile
//{
//	GENERATED_USTRUCT_BODY()
//
//public:
//	UPROPERTY(BlueprintReadWrite, EditAnywhere)
//		FVector2D VesselSize;
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere)
//		FVector2D VesselMargin;
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere)
//		FVector2D VesselBarMargin;
//};


/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudElements_Status : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
public:	
	UPROPERTY()
		class APA_PlayerHUD* OwnerHUD;

	//--------------------------------------------------------------------------------------------------------------------------------
	//  HEALTH TRANSITION
	//--------------------------------------------------------------------------------------------------------------------------------


	//NEW VERSION!
	UPROPERTY()
		TArray<class UImage*> HealthVessels;

	UPROPERTY()
		TArray<class UMaterialInstanceDynamic*> HealthVesselMaterials;

	UPROPERTY()
		class UCanvasPanel* HealthbarPanel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HUD Status Indicator - Health Vessels", meta = (DisplayName = "Health Vessel Material"))
		class UMaterialInterface* HealthVesselMaterialTemplate;

	////TODO; make it so you can resize the vessel bar any way you want
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
	//	FVesselbarSizeProfile VesselBarSmallSize;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
	//	FVesselbarSizeProfile VesselBarMediumSize;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
	//	FVesselbarSizeProfile VesselBarLargeSize;

	//determines the tilesize for each vessel. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FVector2D VesselSize;

	//determines how far removed the vessels are from each other.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FVector2D VesselMargin;

	//determines how far removed the vesselbar is from the bottom and the side.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FVector2D VesselBarMargin;

	//determines how long it takes before the health bar starts animating after damage or recovery. For feedback purposes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		float DelayBeforeVesselBarAnimationStart = 1.f;

	//the speed of the vessel animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		float VesselAnimationSpeed = 4.f;

	//The parameter name in the VesselBarMaterialTemplate for target health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FName ParameterNameTargetAlpha;

	//The parameter name in the VesselBarMaterialTemplate for health before damage - used for determining starting point for damage bar animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FName ParameterNameDamageAlpha;

	//The parameter name in the VesselBarMaterialTemplate for health before recovery - used for determining starting point for recovery bar animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FName ParameterNameRecoveryAlpha;

	//The parameter name in the VesselBarMaterialTemplate for health before recovery - used for determining starting point for recovery bar animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Status Indicator - Health Vessels")
		FName ParameterNameFinalHeartAlpha;

	UPROPERTY()
		FTimerHandle UpdateHealthVesselsTimerRecovery;

	UPROPERTY()
		FTimerHandle UpdateHealthVesselsTimerDamage;

	UPROPERTY()
		float OldHealth = 9.f;

	UPROPERTY()
		float CurrentHealth = 9.f;

	UPROPERTY()
		int32 MinIndex;

	UPROPERTY()
		int32 MaxIndex;

	UPROPERTY()
		int32 HealthVesselsToUpdate;

	UPROPERTY()
		bool bCanAnimateHealthBarRecovery = false;

	UPROPERTY()
		bool bCanAnimateHealthBarDamage = false;

	//processes damage for UI animation and returns the new health amount. 
	UFUNCTION(BlueprintCallable, Category = "Health")
		float ProcessDamage(float NewHeal);

	//processes recovery for UI animation and returns the new health amount. 
	UFUNCTION(BlueprintCallable, Category = "Health")
		float ProcessRecovery(float Recovery);

	UFUNCTION()
		void UpdateHealth(float DeltaTime);

	//forces the current health amount based on the currenthealth variable, and turns off any animation current going on.
	UFUNCTION()
		void ForceHealthAmount();

	UFUNCTION()
		void StartHealthBarRecoveryAnimation() { bCanAnimateHealthBarRecovery = true; };

	UFUNCTION()
		void StartHealthBarDamageAnimation() { bCanAnimateHealthBarDamage = true; };

	//--------------------------------------------------------------------------------------------------------------------------------
	//  FULLSCREEN TRANSITION
	//--------------------------------------------------------------------------------------------------------------------------------

	////Fullscreen transition
	//UPROPERTY()
	//	UImage* FullScreenFadeImage;

	//UPROPERTY()
	//	UCanvasPanelSlot* FullScreenFadeSlot;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fullscreen Fade")
	//	UMaterialInterface* FullScreenFadeMaterial;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fullscreen Fade")
	//	float FadeDuration;

	//UFUNCTION()
	//	void StartOpenFadeIn();

	//UFUNCTION()
	//	void StartCloseFadeIn();

	//UFUNCTION()
	//	void StartOpenFadeOut();

	//UFUNCTION()
	//	void StartCloseFadeOut();

	////Fullscreen transition; to transition away the Hud Elements
	//UFUNCTION()
	//	void SetFade(float NewValue);

	//UFUNCTION()
	//	void SetFadeTest(float NewValue);

	////Callback Functions for when the FullScreen Fade reaches its apex
	//UFUNCTION(BlueprintCallable, Category = "HUD Transition")
	//	void FinishFadeInCallback();

	//UFUNCTION(BlueprintCallable, Category = "HUD Transition")
	//	void FinishFadeOutCallback();

	////Callback so we can properly turn off the Fullscreen Fade image
	//UFUNCTION(BlueprintCallable, Category = "HUD Transition")
	//	void FinalizeOpenFadeCallback();

	//UFUNCTION(BlueprintCallable, Category = "HUD Transition")
	//	void FinalizeCloseFadeCallback();

	//--------------------------------------------------------------------------------------------------------------------------------
	//  MISC
	//--------------------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "HUD Transition")
		void SetElementsToInvisible();

	UFUNCTION(BlueprintCallable, Category = "HUD Transition")
		void SetElementsToVisible();
};