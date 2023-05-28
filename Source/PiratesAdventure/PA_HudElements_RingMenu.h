// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_RingMenu.generated.h"

/**
 * 
 */
UENUM()
enum class EButtonAssociation : uint8
{
	BUTTON_DPADUP UMETA(DisplayName = "D-pad Up"),
	BUTTON_DPADLEFT UMETA(DisplayName = "D-pad Left"),
	BUTTON_DPADDOWN UMETA(DisplayName = "D-pad Down"),
	BUTTON_DPADRIGHT UMETA(DisplayName = "D-pad Right"),
	BUTTON_SELECT UMETA(DisplayName = "Select")
};

UENUM()
enum class ERingType : uint8
{
	RING_NONE UMETA(DisplayName = "No Ring"),
	RING_ELEMENT UMETA(DisplayName = "Ring Elements"),
	RING_TOOL UMETA(DisplayName = "Ring Tools")
};

USTRUCT(Blueprintable)
struct FRingMenuIcon
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		class UImage* Image;

	UPROPERTY()
		class UCanvasPanelSlot* Slot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Angle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInterface* MaterialTemplate;

	UPROPERTY()
		class UMaterialInstanceDynamic* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EButtonAssociation ButtonAssociation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDefault = false;
};

UCLASS()
class PIRATESADVENTURE_API UPA_HudElements_RingMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	
	//UPA_HudElements_RingMenu(const FObjectInitializer & ObjectInitializer);

	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	//MISC
	UPROPERTY()
		class APA_PlayerHUD* OwnerHUD;

	//For properly initializing the ring.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring Icons")
		ERingType RingType;

	//RING DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring Icons")
		TArray<FRingMenuIcon> ElementRing;

	UFUNCTION()
		void ProcessAccept();

	UFUNCTION()
		void ProcessActiveIcons(int32 Index = INDEX_NONE);

	UFUNCTION()
		void TurnOnInactiveIcons(int32 Index = INDEX_NONE);

	UFUNCTION()
		void TurnOffInactiveIcons(int32 Index = INDEX_NONE);

	UFUNCTION()
		int32 GetRingIconIndex(FName ID);

	/** THE ACTIONS ASSOCIATED WITH OPENING/CLOSING THE RING. PURELY FOR ANIMATION **/
	UPROPERTY()
		EHudElementState HudElementState = EHudElementState::HES_NoLoop;

	//The radius at which the elements will be spawned. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		float IconSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		float IconDistance = 250.f;

	//How much the icons rotate around the player during opening and closing the ring
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		float IconRotation = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		float IntroDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		float OuttroDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		float MinTimeDilation = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		FName IconActiveParameter = TEXT("IconSelected");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RingMenu")
		FName IconInactiveParameter = TEXT("IconInactive");

	UPROPERTY()
		float TimeElapsed;

	//void ProcessSelector();
	void SetActiveIcon(EButtonAssociation NewButton);
	void SetInactiveIcon(EButtonAssociation NewButton, bool bNewActive);
	void SetIconUnlocked(EButtonAssociation NewButton, bool bNewLockedState);

	virtual void OpenRingMenu();
	virtual void CloseRingMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "RingMenu Actions", meta = (DisplayName = "Process Open Ring Menu"))
		void OnProcessOpenRingMenu(float CurrentTimeElapsed); //the increment doesn't work correctly; you should look into it. 

	UFUNCTION(BlueprintImplementableEvent, Category = "RingMenu Actions", meta = (DisplayName = "Process Close Ring Menu"))
		void OnProcessCloseRingMenu(float CurrentTimeElapsed);
};