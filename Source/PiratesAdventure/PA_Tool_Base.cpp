// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Tool_Base.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"

UPA_Tool_Base::UPA_Tool_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bToolIsChargeBased = false;
	bToolIsAiming = false;
	bIsCoolingDown = false;
	bRechargeDelayed = false;

	AmmoMultiplier = 1.f;
	AmmoDuration = 1.f;
	AmmoUpgrades = 1.f;
	AmmoMaxUpgrades = 3.f;
	AmmoRechargeTime = 1.f;
	AmmoRechargeDelay = 0.5f;
	bRechargeUpgradeUnlocked = false;
	AmmoCurrent = AmmoMultiplier * AmmoUpgrades; //TODO: See if this doesn't make the game max out the charge on reload, or whether that's even an issue. 

	if (bShouldAlwaysTick)
	{
		PrimaryComponentTick.bStartWithTickEnabled = true;
	}
	else
	{
		PrimaryComponentTick.bStartWithTickEnabled = false;
	}

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = false;

	bWantsInitializeComponent = true;
}

////REGISTER AND INITIALIZE WEAPON
//void UPA_Tool_Base::RegisterComponent()
//{
//	Super::RegisterComponent();
//
//	AmmoCurrent = AmmoMultiplier * AmmoUpgrades;
//
//	ToolOwner = Cast<APiratesAdventureCharacter>(GetOwner());
//	ReceiveToolRegister(ToolOwner);
//}

// Initializes the component, allowing us to register it with the player. Is different from InitializeTool
void UPA_Tool_Base::InitializeComponent()
{
	ToolOwner = Cast<APiratesAdventureCharacter>(GetOwner());
	if (!ToolOwner)
	{
		UE_LOG(AnyLog, Error, TEXT("Couldn't register %s component with the player"), *(GetClass()->GetName()));
	}
	else
		ToolOwner->RegisterToolComponent(this);

	AmmoCurrent = AmmoMultiplier * AmmoUpgrades;

	ReceiveToolRegister(ToolOwner);

	Super::InitializeComponent();
}

void UPA_Tool_Base::InitializeTool()
{
	if (ToolOwner->EquippedWeapon)
	{
		SetToolElement(ToolOwner->EquippedWeapon->CurrentWeaponType);
	}

	ReceiveInitializeTool();
}

void UPA_Tool_Base::DeinitializeTool()
{
	bLockToolAiming = false;

	StopTool();
	ReceiveDeinitializeTool();
}

void UPA_Tool_Base::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	//UE_LOG(AnyLog, Warning, TEXT("CURRENT CHARGE IS %f"), AmmoCurrent);
	//TODO: Add a boolean for when to restart recharge; NOT CONNECTED TO COOLDOWN. 
	if (ToolOwner
		&& GetAmmoIsInfinite())
	{
		AmmoCurrent = AmmoMultiplier * AmmoUpgrades;
	}
	else if (!bRechargeDelayed)
	{
		if (bToolChargeIsDecreasing)
		{
			if (AmmoCurrent >= 0.f)
			{
				AmmoCurrent -= DeltaTime / AmmoDuration;
			}
			else
			{
				AmmoCurrent = 0.f;
				//UE_LOG(AnyLog, Warning, TEXT("STOP TOOL WAS CALLED ON CHARGE TOOL FROM TICK"));
				StopTool();
			}
		}
		else
		{
			if (AmmoCurrent < AmmoMultiplier * AmmoUpgrades)
			{
				AmmoCurrent += DeltaTime / AmmoRechargeTime;
			}
			else
			{
				AmmoCurrent = AmmoMultiplier * AmmoUpgrades; //Fix it to the max ammo just in case, so we don't get any overshoot.

				if (!bShouldAlwaysTick)
					SetComponentTickEnabled(false);
			}
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPA_Tool_Base::GetAmmoIsInfinite()
{
	if (ToolOwner != NULL)
	{
		return ToolOwner->bAllAmmoIsInfinite || ElementIsInfinite(GetToolElement());
	}
	else
		return false;
}

bool UPA_Tool_Base::ElementIsInfinite(EPlayerWeaponType Element)
{
	if (ElementsInfiniteAmmo.Num() != 0)
	{
		for (int32 i = 0; i < ElementsInfiniteAmmo.Num(); i++)
		{
			if (ElementsInfiniteAmmo[i] == Element)
				return true;
		}

		return false;
	}
	else return false;
}

void UPA_Tool_Base::SetAmmoElementIsInfinite(bool bInfinite, EPlayerWeaponType Element, bool bResetAll /* = false */)
{
	if (bResetAll)
	{
		ElementsInfiniteAmmo.Empty();
		return;
	}
	else
	{
		if (bInfinite)
		{
			ElementsInfiniteAmmo.AddUnique(Element);
		}
		else
		{
			for (int32 i = 0; i < ElementsInfiniteAmmo.Num(); i++)
			{
				if (ElementsInfiniteAmmo[i] == Element)
				{
					ElementsInfiniteAmmo.RemoveAt(i);
					return;
				}
			}
		}
	}
}

bool UPA_Tool_Base::CanStartTool()
{
	if (!bToolIsChargeBased)
	{
		return FMath::FloorToInt(AmmoCurrent) != 0 && !bIsCoolingDown;
	}
	else
	{
		return AmmoCurrent > 0.f && !bIsCoolingDown;
	}
}

void UPA_Tool_Base::StartTool()
{
	UWorld* World = GetWorld();

	if (World)
	{
		if (!CanStartTool())
		{
			StartToolNoAmmo();
		}
		else
		{
			if (bToolIsChargeBased)
			{
				//bToolChargeIsDecreasing = true;
				//AmmoCurrent = (float)FMath::FloorToInt(AmmoCurrent); //Make sure that we only use the fully recharged bits.
				//UE_LOG(AnyLog, Warning, TEXT("AMMOCURRENT AFTER TICK STARTS %f"), AmmoCurrent);

				if (!bShouldAlwaysTick && AmmoCurrent != 0.f)
				{
					SetComponentTickEnabled(true);
				}
			}
			else
			{
				if(!bShouldAlwaysTick)
					SetComponentTickEnabled(false); //don't recharge when using the tool. 

				//AmmoCurrent = (float)FMath::FloorToInt(AmmoCurrent) - 1.f;
				AmmoCurrent -= 1.f;
				//UE_LOG(AnyLog, Warning, TEXT("AMMOCURRENT AFTER REDUCTION OF USED AMMO"), AmmoCurrent);
				World->GetTimerManager().SetTimer(StartRechargeHandle, this, &UPA_Tool_Base::StartRecharge, AmmoRechargeDelay, false);
				bRechargeDelayed = true;
			}

			ReceiveStartTool();

			bToolIsBeingUsed = true;
			World->GetTimerManager().SetTimer(ToolUsedHandle, this, &UPA_Tool_Base::TurnOffToolBeingUsed, ToolBeingUsedTimer, false);
		}
	}
}

void UPA_Tool_Base::TurnOffToolBeingUsed()
{
	bToolIsBeingUsed = false;
}

void UPA_Tool_Base::StartToolNoAmmo()
{
	ReceiveStartToolNoAmmo();
}

void UPA_Tool_Base::ToggleChargeDecrease(bool bDecreaseCharge)
{
	bToolChargeIsDecreasing = bDecreaseCharge;

	if (bToolChargeIsDecreasing && GetWorld()->GetTimerManager().IsTimerActive(StartRechargeHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(StartRechargeHandle);
		StartRecharge();
	}
}

void UPA_Tool_Base::StopTool()
{
	if (GetWorld() && bToolChargeIsDecreasing)
	{
		if (bToolIsChargeBased)
		{
			bToolChargeIsDecreasing = false;

			if (!bShouldAlwaysTick)
				SetComponentTickEnabled(false);
		}

		GetWorld()->GetTimerManager().SetTimer(StartRechargeHandle, this, &UPA_Tool_Base::StartRecharge, AmmoRechargeDelay, false);
		bRechargeDelayed = true;
	}

	ReceiveStopTool();
}

void UPA_Tool_Base::StartRecharge()
{
	if(!bShouldAlwaysTick)
		SetComponentTickEnabled(true);

	bRechargeDelayed = false;
}

void UPA_Tool_Base::SetToolElement(EPlayerWeaponType NewElement)
{
	ReceiveSetToolElement(NewElement);
}

EPlayerWeaponType UPA_Tool_Base::GetToolElement()
{
	if (ToolOwner)
	{
		return ToolOwner->CurrentElement;
	}

	return EPlayerWeaponType::NONE;
}

void UPA_Tool_Base::UpgradeAmmoCount()
{
	AmmoUpgrades = FMath::Clamp(AmmoUpgrades + 1.f, 0.f, AmmoMaxUpgrades);

	AmmoCurrent = AmmoMultiplier * AmmoUpgrades; //Refill ammo completely during an upgrade.
}