// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PA_StaticLibrary.h"
#include "PA_Tool_Base.generated.h"

/**
 * 
 */

UCLASS(ClassGroup = Tools, BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class PIRATESADVENTURE_API UPA_Tool_Base : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:

	UPA_Tool_Base(const FObjectInitializer& ObjectInitializer);

	//FUNCTIONALITY FOR REGISTERING AND INITIALIZING WEAPON
	//void RegisterComponent();

	// Necessary for our own initializations
	virtual void InitializeComponent() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Tool Register"))
		void ReceiveToolRegister(APiratesAdventureCharacter* Owner);

	virtual void InitializeTool();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Tool Equip"))
		void ReceiveInitializeTool();

	virtual void DeinitializeTool();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Tool Unequip"))
		void ReceiveDeinitializeTool();

	virtual void StartToolNoAmmo();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On No Ammo"))
		void ReceiveStartToolNoAmmo();

	UFUNCTION(BlueprintCallable, Category = "Player Tool", meta = (DisplayName = "Toggle Tool"))
		virtual void ToggleTool() { bToolIsActive = !bToolIsActive; };

	UFUNCTION(BlueprintCallable, Category = "Player Tool")
		void SetToolElement(EPlayerWeaponType NewElement);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "Set Elemental Properties"))
		void ReceiveSetToolElement(EPlayerWeaponType NewElement);

	UFUNCTION(BlueprintPure, Category = "Player Tool")
		EPlayerWeaponType GetToolElement();

	//Activate and deactivate tool effect
	virtual void StartTool();
	virtual void StopTool();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Start Tool"))
		void ReceiveStartTool();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Stop Tool"))
		void ReceiveStopTool();

	//starts or stops aiming tool. Doesn't activate it.
	virtual void StartAimTool(float X, float Y) {};
	virtual void StopAimTool() {};

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Start Aim Tool"))
		void ReceiveStartAimTool(float X, float Y);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool", meta = (DisplayName = "On Stop Aim Tool"))
		void ReceiveStopAimTool();

	float AimToolRight;
	float AimToolUp;

	UFUNCTION(BlueprintPure, Category = "Tool")
		void GetAim(float& X, float& Y) { X = AimToolRight; Y = AimToolUp; };

	//TOOL PROPERTY VALUES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		EPlayerToolType ToolType;

	UPROPERTY()
		EPlayerWeaponType CurrentElement;

	//This can differ from tool to tool, so rather than defining it in the player, we should define it in the tools instead.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		FName ToolSocketName;

	//Visualizes feedback for the sake of debugging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		bool bDebug = false;

	UPROPERTY()
		class APiratesAdventureCharacter* ToolOwner;

	UFUNCTION(BlueprintPure, Category = "Tool")
		class APiratesAdventureCharacter* GetToolOwner() { return ToolOwner; };

	//Used for aiming tools; when set to true, the player can start aiming
	UPROPERTY(BlueprintReadOnly, Category = "Tool")
		bool bToolIsActive = false;

	UPROPERTY()
		bool bToolHasAiming = false;

	//Used for tools with ammunition, to make sure we don't reuse it while animations are still playing. 
	UPROPERTY()
		bool bIsCoolingDown;

	//Stopgap measure for making sure the tool cannot aim while the player is turning during movement.
	UPROPERTY()
		bool bLockToolAiming;

	UFUNCTION(BlueprintCallable, Category = "Tool")
		void SetCooldown(bool bNewCooldown) { bIsCoolingDown = bNewCooldown; };

	UPROPERTY()
		TArray<EPlayerWeaponType> ElementsInfiniteAmmo;

	//Non-blueprint function only. Compares only the tool elements versus what has been set to infinite. Does not check whether all elements have been set to infinite in Player.
	bool ElementIsInfinite(EPlayerWeaponType Element);

	UFUNCTION(BlueprintPure, Category = "Tool")
		bool GetAmmoIsInfinite();

	//While it works, it can be exploited easily by switching elements. Since the countdown is not based on element, but on tool usage, we can quickly reset any 
	//magic depletion by switching to the elements not in the array and then switching back. Consider just making all "infinite" points to be infinite on all elements. Requires less overhead.
	UFUNCTION(BlueprintCallable, Category = "Tool")
		void SetAmmoElementIsInfinite(bool bInfinite, EPlayerWeaponType Element, bool bResetAll = false);

	UPROPERTY()
		float AmmoCurrent;

	//Used for determining the usage method. For non-charge based, we decrease the current ammo by one. For charge based, we need to count down. 
	UPROPERTY()
		bool bToolIsChargeBased = false;

	UPROPERTY()
		bool bToolIsAiming = false;

	//We use this for charge based tools, since ToolIsActive is also true while simply aiming.
	UPROPERTY()
		bool bToolChargeIsDecreasing = false;

	UPROPERTY()
		bool bRechargeDelayed = false;

	UFUNCTION(BlueprintCallable, Category = "Tool")
		void ToggleChargeDecrease(bool bDecreaseCharge);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		class UPA_Asset_Item* ItemData;

	//A failsafe for tools that need continueous ticking for their animation. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		bool bShouldAlwaysTick = false;

	//The number we multiply the upgrades with to get the current maximum ammo count. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float AmmoMultiplier;

	//For charge based tools, our ammo has a duration. For non-charge based tools, we simply use one unit instantly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float AmmoDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float AmmoUpgrades;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float AmmoMaxUpgrades;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float AmmoRechargeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float AmmoRechargeDelay;

	UPROPERTY()
		bool bRechargeUpgradeUnlocked;

	FTimerHandle StartRechargeHandle;

	void StartRecharge();

	UFUNCTION(BlueprintPure, Category = "Tool")
		float GetCurrentAmmo() { return AmmoCurrent; };

	UFUNCTION(BlueprintCallable, Category = "Tool")
		void UpgradeAmmoCount();

	UFUNCTION(BlueprintCallable, Category = "Tool")
		void UpgradeRechargeTime() { bRechargeUpgradeUnlocked = true; };
	
	UFUNCTION(BlueprintPure, Category = "Tool")
		bool CanStartTool();

	UFUNCTION(BlueprintPure, Category = "Tool")
		bool ChargeIsDecreasing() { return bToolChargeIsDecreasing; };

	UFUNCTION(BlueprintPure, Category = "Tool")
		bool ToolIsAiming() { return bToolHasAiming && bToolIsAiming; };

	//For instantaneous effects for certain tools
	bool bToolIsBeingUsed = false;

	FTimerHandle ToolUsedHandle;

	//For instantaneous effects, for instance when we fire the gun "from the hip"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
		float ToolBeingUsedTimer = 0.1f;

	void TurnOffToolBeingUsed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Tool")
		void OnSwitchElement();
};
