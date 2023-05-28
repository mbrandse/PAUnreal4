// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
//#include "PiratesAdventureCharacter.h"
#include "Dialogue.h"
#include "PA_PlayerHUD.generated.h"

/**
 * 
 */

//TODO: Delete this, once you have gotten rid of the other hud element classes.
UENUM() //for animating hud elements
enum class EHudElementState : uint8
{
	HES_StartLoop UMETA(DisplayName = "Start Loop"),
	HES_MainLoop UMETA(DisplayName = "Main Loop"),
	HES_StopLoop UMETA(DisplayName = "Stop Loop"),
	HES_ChangeLoop UMETA(DisplayName = "Change Loop"),
	HES_NoLoop UMETA(DisplayName = "Inactive")
};

UENUM(BlueprintType) //RingType
enum class ERingMenuType : uint8
{
	RMT_None UMETA(DisplayName = "NULL"),
	RMT_Element UMETA(DisplayName = "Element Ring"),
	RMT_Tool UMETA(DisplayName = "Tool Ring"),
	RMT_Misc UMETA(DisplayName = "Misc Ring") //The misc ring will be used for any ring that is not tool or element.
};

UCLASS()
class PIRATESADVENTURE_API APA_PlayerHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay();

	//We need this variable due to the ability to switch to an AI controller (at which point we lose the normal way of referencing the character).
	UPROPERTY()
		class APiratesAdventureCharacter* OwnerCharacter;

	//All the various elements and what is required to get them up and running.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Hud Elements")
		TSubclassOf<class UPA_HudElements_RingMenu> ElementRingTemplate;

	UPROPERTY(BlueprintReadOnly)
		class UPA_HudElements_RingMenu* ElementRingInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Hud Elements")
		TSubclassOf<class UPA_HudElements_RingMenu> ToolRingTemplate;

	UPROPERTY(BlueprintReadOnly)
		class UPA_HudElements_RingMenu* ToolRingInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Hud Elements")
		TSubclassOf<class UPA_HudElements_Status> StatusIndicatorTemplate;

	//UPROPERTY()
	//	class UPA_HudElements_Status* StatusIndicatorInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Hud Elements")
		TSubclassOf<class UPA_HudElements_ContextPopup> ContextPopupTemplate;

	UPROPERTY()
		class UPA_HudElements_ContextPopup* ContextPopupInstance;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Hud Elements")
	//	TSubclassOf<class UPA_HudElements_IngameMenu> IngameMenuTemplate;

	//UPROPERTY()
	//	class UPA_HudElements_IngameMenu* IngameMenuInstance;

	//TODO: Move this mostly to blueprint.

	//Spawn Dialogue is responsible for spawning the raw data of one Dialogue class. 
	void SpawnDialogue(TArray<AActor*> NPCs, class UDialogue* Dialogue);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "Spawn Dialogue"))
		void OnReceiveSpawnDialogue(class UDialogue* Dialogue);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialogue")
		void RemoveDialogue(class UDialogue* Dialogue);

	UPROPERTY()
		class UPA_HudElements_ItemGet* ItemPopupInstance;

	UFUNCTION()
		void SpawnItemGetWindow(class UPA_Asset_Item* Item, TSubclassOf<class UPA_HudElements_ItemGet> Template);

	UFUNCTION()
		bool ShouldFocusDialogue(class UDialogue* Dialogue);

	//so we can wrestle control away from the pawn, without having to do a state change. Since UI can be called anywhere, it's risky to make a state out of it.
	UFUNCTION(BlueprintCallable, Category = "Input")
		void EnableHUDInput(class APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Input")
		void RebindHUDInput();

	////Fullscreen transition; to transition away the Hud Elements
	//UFUNCTION(BlueprintCallable, Category = "HUD Transition")
	//	void SetFade(float NewValue);

	////testing
	//UFUNCTION(BlueprintCallable, Category = "HUD Transition")
	//	void StartFade();

	//Hud related input functions
	virtual void InterfaceAcceptPress();
	virtual void InterfaceAcceptRelease();
	virtual void InterfaceCancelPress();
	virtual void InterfaceCancelRelease();
	virtual void InterfaceUpPress();
	virtual void InterfaceUpRelease();
	virtual void InterfaceLeftPress();
	virtual void InterfaceLeftRelease();
	virtual void InterfaceDownPress();
	virtual void InterfaceDownRelease();
	virtual void InterfaceRightPress();
	virtual void InterfaceRightRelease();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Accept Press"))
		void ReceiveInterfaceAcceptPress();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Accept Release"))
		void ReceiveInterfaceAcceptRelease();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Cancel Press"))
		void ReceiveInterfaceCancelPress();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Cancel Release"))
		void ReceiveInterfaceCancelRelease();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Up Press"))
		void ReceiveInterfaceUpPress();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Up Release"))
		void ReceiveInterfaceUpRelease();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Left Press"))
		void ReceiveInterfaceLeftPress();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Left Release"))
		void ReceiveInterfaceLeftRelease();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Down Press"))
		void ReceiveInterfaceDownPress();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Down Release"))
		void ReceiveInterfaceDownRelease();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Right Press"))
		void ReceiveInterfaceRightPress();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input", meta = (DisplayName = "Right Release"))
		void ReceiveInterfaceRightRelease();

	//Damage and recovery
	UFUNCTION(BlueprintImplementableEvent, Category = "Damage and Recovery")
		void ProcessDamage();

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage and Recovery")
		void ProcessRecovery();

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage and Recovery")
		void ProcessHealthUpgrade(bool bNeedsRecovery);

	//Tools and Magic
	UFUNCTION(BlueprintImplementableEvent, Category = "Tools and Magic")
		void SetElementUnlocked(EPlayerWeaponType Element, bool bUnlocked);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tools and Magic")
		void SetToolUnlocked(EPlayerToolType Tool, bool bUnlocked);

	//Context
	UFUNCTION(BlueprintImplementableEvent, Category = "Context Action")
		void SetContextText(const FText &ContextText, FLinearColor ContextColor = FColor::White);

	UFUNCTION(BlueprintImplementableEvent, Category = "Context Action")
		void ClearContextText();

	//Ring Menu
	UFUNCTION(BlueprintImplementableEvent, Category = "Ring Menu")
		void SwitchEquipVisualization(ERingMenuType RingMenu);

	UFUNCTION(BlueprintImplementableEvent, Category = "Context Action")
		void ResetEquipVisualization();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ring Menu")
		void ProcessKeyBinding(FName KeyBinding);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ring Menu")
		void SetRingUnlocked(class UPA_Asset_Item* Item);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ring Menu")
		void SetRingInactive(class UPA_Asset_Item* Item);

	//Misc ingame options
	UFUNCTION(BlueprintImplementableEvent, Category = "Misc Menu")
		void OpenIngameMap();

	UFUNCTION(BlueprintImplementableEvent, Category = "Misc Menu")
		void OpenIngameItems();
};
