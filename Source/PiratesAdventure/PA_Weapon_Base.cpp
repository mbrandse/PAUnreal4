// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_Weapon_Base.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UPA_Weapon_Base::UPA_Weapon_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RamaMeleeWeapon_OnHit.AddDynamic(this, &UPA_Weapon_Base::WeaponOnHit);

	bWantsInitializeComponent = true;
}

//INITIALIZE COMPONENT
void UPA_Weapon_Base::InitializeComponent()
{
	WeaponOwner = Cast<APiratesAdventureCharacter>(GetOwner());
	if (!WeaponOwner)
	{
		UE_LOG(AnyLog, Error, TEXT("Couldn't register %s component with the player"), *(GetClass()->GetName()));
	}
	else
		WeaponOwner->RegisterWeaponComponent(this);

	Super::InitializeComponent();
}

//CHARGE ATTACKS
void UPA_Weapon_Base::ProcessStartChargeAttack()
{
	OnReceiveProcessStartChargeAttack();

	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(ChargeFinishedHandle, this, &UPA_Weapon_Base::ProcessReadyChargeAttack, ChargeTime, false);
}

void UPA_Weapon_Base::ProcessReadyChargeAttack()
{
	OnReceiveProcessReadyChargeAttack();

	bCanStartChargeAttack = true;
}

void UPA_Weapon_Base::ProcessStopChargeAttack()
{
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(ChargeFinishedHandle))
	{
		InterruptChargeAttack();
		return;
	}

	if (bCanStartChargeAttack)
	{
		bIsChargeAttacking = true;
	}

	OnReceiveProcessStopChargeAttack(bCanStartChargeAttack);
}

void UPA_Weapon_Base::InterruptChargeAttack()
{
	OnReceiveProcessInterruptChargeAttack();

	if (GetWorld())
	{
		bCanStartChargeAttack = false;
		bCanContinueChargeAttack = false;
		GetWorld()->GetTimerManager().ClearTimer(ChargeFinishedHandle);
	}
}

//COMBO ATTACKS
void UPA_Weapon_Base::ProcessAttack(UAnimSequenceBase* Attack)
{
	if (!Attack)
		return;

	if (GetWorld())
	{
		float AnimationFinishTime = Attack->GetPlayLength() / Attack->RateScale;
		CurrentAttackAnimation = Attack;
		ProcessAttackData(FName(*Attack->GetName()));

		//UE_LOG(AnyLog, Log, TEXT("CURRENT ATTACK NAME IS %s"), *(CurrentAttackData.Attack.ToString()));

		//if we start a new attack, we can assume that we need to cancel any charge in progress.
		if (GetWorld()->GetTimerManager().IsTimerActive(ChargeFinishedHandle))
		{
			InterruptChargeAttack();
			//bCanStartChargeAttack = false;
		}

		GetWorld()->GetTimerManager().SetTimer(AnimationFinishedHandle,
			this,
			bIsAttacking ? &UPA_Weapon_Base::ProcessNormalAttackFinished : &UPA_Weapon_Base::ProcessChargeAttackFinished, //since Morgan only has two kinds of attacks
			AnimationFinishTime,
			false);
	}
}

void UPA_Weapon_Base::ProcessAttackData(FName AttackName)
{
	UE_LOG(AnyLog, Log, TEXT("ATTACK NAME IS %s"), *(AttackName.ToString()));

	if (!AttackDataList)
	{
		CurrentAttackData.Attack = TEXT("Attack data is empty");
		CurrentAttackData.Type = EAttackType::AT_NULL;
		CurrentAttackData.Damage = 0.f;

		return;
	}

	static const FString Context = FString(TEXT("Context")); //not sure why this is necessary
	TArray<FName> AttackRowNames = AttackDataList->GetRowNames();

	for (int32 DataIndex = 0; DataIndex < AttackRowNames.Num(); DataIndex++)
	{
		FAttackLookupTable * AttackDataRef = AttackDataList->FindRow<FAttackLookupTable>(AttackRowNames[DataIndex], Context, false);
		if (AttackDataRef && AttackDataRef->Attack == AttackName)
		{
			CurrentAttackData.Attack = AttackName;
			CurrentAttackData.Type = AttackDataRef->Type;
			CurrentAttackData.Damage = AttackDataRef->Damage;

			return;
		}
	}
}

void UPA_Weapon_Base::ProcessPressAttackButton()
{
	ComboCount++;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(ComboCount));
}

void UPA_Weapon_Base::ProcessNormalAttackFinished()
{
	//Since combo functionality will be handled entirely by anim blueprints, use this to initiate the charge
	if (bCanContinueChargeAttack)
	{
		bCanContinueChargeAttack = false;
		ProcessStartChargeAttack();
	}
}

void UPA_Weapon_Base::ProcessChargeAttackFinished()
{
	//bCanContinueChargeAttack = false;
	//bCanStartChargeAttack = false;

	ResetChargeAttack();
}

//PROCESS HIT INFORMATION
void UPA_Weapon_Base::WeaponOnHit(class AActor* HitActor, class UPrimitiveComponent* HitComponent, const FVector& ImpactPoint, const FVector& ImpactNormal, FName HitBoneName, int32 DamageMapIndex, int32 ShapeIndex, const struct FHitResult& HitResult)
{
	FPointDamageEvent DamageEvent;

	if (CanHitActor(HitActor)
		&& RamaMeleeWeapon_OnHit.IsBound())
	{
		if (CurrentDamageType && HitComponent->GetAttachmentRootActor() != WeaponOwner)
		{
			DamageEvent.DamageTypeClass = CurrentDamageType;
			DamageEvent.HitInfo = HitResult;
			DamageEvent.HitInfo.BoneName = HitBoneName;
			DamageEvent.Damage = CurrentAttackData.Damage * DamageMultiplier;
			HitActor->TakeDamage(DamageEvent.Damage, DamageEvent, WeaponOwner->GetController(), WeaponOwner);

			switch (CurrentAttackData.Type)
			{
			case EAttackType::AT_NORMAL:
				ReceiveWeaponOnHitCombo(HitActor, HitComponent, ImpactPoint, ImpactNormal, HitBoneName, DamageMapIndex, ShapeIndex, HitResult);
				break;

			case EAttackType::AT_CHARGE:
				ReceiveWeaponOnHitCharge(HitActor, HitComponent, ImpactPoint, ImpactNormal, HitBoneName, DamageMapIndex, ShapeIndex, HitResult);
				break;

			case EAttackType::AT_NULL:
				UE_LOG(AnyLog, Log, TEXT("ATTACK TYPE IS NULL!"));
				break;
			}
		}
	}
}

bool UPA_Weapon_Base::CanHitActor(AActor* HitActor)
{
	int32 HitIndex = HitActors.Find(HitActor);
	if (HitIndex == INDEX_NONE)
	{
		HitActors.Add(HitActor);
		return true;
	}

	return false;
}

bool UPA_Weapon_Base::CanHitComponent(UPrimitiveComponent* HitComponent)
{
	int32 HitIndex = HitComponents.Find(HitComponent);
	if (HitIndex == INDEX_NONE)
	{
		HitComponents.Add(HitComponent);
		return true;
	}

	return false;
}

void UPA_Weapon_Base::StopSwingDamage()
{
	Super::StopSwingDamage();

	HitActors.Empty();
	HitComponents.Empty();
}

//PROCESS ATTACK COOLDOWN

void UPA_Weapon_Base::StartAttackCooldown()
{
	//attack cooldown now only takes care of the cooldown variable. Resets should be done with ResetWeapon.

	GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &UPA_Weapon_Base::StopAttackCooldown, CooldownTime, false);
	bAttackIsCoolingDown = true;
}

void UPA_Weapon_Base::StopAttackCooldown()
{
	bAttackIsCoolingDown = false;
}

void UPA_Weapon_Base::ResetAttackCooldown()
{
	if (WeaponOwner && GetWorld()->GetTimerManager().IsTimerActive(CooldownHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownHandle);
		bAttackIsCoolingDown = false;
	}
}

//REGISTER AND INITIALIZE WEAPON

void UPA_Weapon_Base::RegisterComponent()
{
	Super::RegisterComponent();

	ReceiveWeaponRegister(WeaponOwner);
}

void UPA_Weapon_Base::OnEquipWeapon()
{
	ReceiveOnEquipWeapon();

	//to make sure we only tick the weapon that is currently equipped.
	SetComponentTickEnabled(true);
}

void UPA_Weapon_Base::OnUnequipWeapon()
{
	ReceiveOnUnequipWeapon();

	//to make sure we only tick the weapon that is currently equipped.
	SetComponentTickEnabled(false);
}

//MISC FUNCTIONALITY

void UPA_Weapon_Base::LandFromAttack()
{
	if (WeaponOwner /*&& bJumpAttack && WeaponOwner->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurrentAttackData.AttackAnimation)*/)
	{
		//switch (CurrentAttackType)
		//{
		//case EAttackType::TYPE_COMBO: 
		//	WeaponOwner->GetMesh()->GetAnimInstance()->Montage_JumpToSection(NormalJumpAttackLandSegment);
		//	//we really don't need an attack cooldown for a jump attack 
		//	if (GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
		//		GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
		//	//GetWorld()->GetTimerManager().SetTimer(EndMontageTimerHandle, this, &UPA_Weapon_Base::StartAttackCooldown, CurrentAttackData.AttackAnimation->GetSectionLength(CurrentAttackData.AttackAnimation->GetSectionIndex(NormalJumpAttackLandSegment)), false);
		//	break;
		//}
	}

	//bJumpAttack = false;
}

void UPA_Weapon_Base::ResetNormalAttack()
{
	if (WeaponOwner)
	{
		StopSwingDamage();
		WeaponOwner->EnableMovement();
		ComboCount = 0;
	}
}

void UPA_Weapon_Base::ResetChargeAttack()
{
	if (WeaponOwner)
	{
		StopSwingDamage();
		WeaponOwner->EnableMovement();
		bCanContinueChargeAttack = false;
		bCanStartChargeAttack = false;

		if (GetWorld())
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(AnimationFinishedHandle))
				GetWorld()->GetTimerManager().ClearTimer(AnimationFinishedHandle);

			if (GetWorld()->GetTimerManager().IsTimerActive(ChargeFinishedHandle))
				GetWorld()->GetTimerManager().ClearTimer(ChargeFinishedHandle);
		}
	}
}

//Element switch functions
void UPA_Weapon_Base::SetWeaponElement(EPlayerWeaponType NewElement)
{
	if (CurrentWeaponType != NewElement)
	{
		RemoveWeaponElement(CurrentWeaponType);

		switch (NewElement)
		{
		case EPlayerWeaponType::WEAPON_DEFAULT:
			CurrentDamageType = DamageTypeDefault;
			ReceiveSetElementDefault();
			break;

		case EPlayerWeaponType::WEAPON_FIRE:
			CurrentDamageType = DamageTypeFire;
			ReceiveSetElementFire();
			break;

		case EPlayerWeaponType::WEAPON_ICE:
			CurrentDamageType = DamageTypeIce;
			ReceiveSetElementIce();
			break;

		case EPlayerWeaponType::WEAPON_WIND:
			CurrentDamageType = DamageTypeWind;
			ReceiveSetElementWind();
			break;

		case EPlayerWeaponType::WEAPON_WOOD:
			CurrentDamageType = DamageTypeWood;
			ReceiveSetElementWood();
			break;
		}

		CurrentWeaponType = NewElement;
	}
}

void UPA_Weapon_Base::RemoveWeaponElement(EPlayerWeaponType CurrentElement)
{
	switch (CurrentElement)
	{
	case EPlayerWeaponType::WEAPON_DEFAULT:
		ReceiveRemoveElementDefault();
		break;

	case EPlayerWeaponType::WEAPON_FIRE:
		ReceiveRemoveElementFire();
		break;

	case EPlayerWeaponType::WEAPON_ICE:
		ReceiveRemoveElementIce();
		break;

	case EPlayerWeaponType::WEAPON_WIND:
		ReceiveRemoveElementWind();
		break;

	case EPlayerWeaponType::WEAPON_WOOD:
		ReceiveRemoveElementWood();
		break;
	}
}