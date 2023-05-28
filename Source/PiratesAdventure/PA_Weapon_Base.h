// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RamaMeleeWeapon.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Weapon_Base.generated.h"

/** Attack types have subtypes that are the same for all attacks. **/
UENUM()
enum class EAttackType : uint8
{
	AT_NULL UMETA(DisplayName = "None"), //NULL is used when attack data cannot be found in the AttackLookupTable
	AT_NORMAL UMETA(DisplayName = "Normal Attack"),
	AT_CHARGE UMETA(DisplayName = "Charge Attack")
};

USTRUCT(BlueprintType)
struct FAttackLookupTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	//Animation name for the attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		FName Attack;

	//Type of attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		EAttackType Type;

	//Attack Damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float Damage;
};

USTRUCT(Blueprintable)
struct FSwordSaveData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
		class USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
		float DamageModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
		FRamaMeleeDamageMap DamageMap;
};

UCLASS(ClassGroup = Weapons, meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class PIRATESADVENTURE_API UPA_Weapon_Base : public URamaMeleeWeapon
{
	GENERATED_BODY()
	
public:

	UPA_Weapon_Base(const FObjectInitializer& ObjectInitializer);

	//FUNCTIONALITY FOR REGISTERING AND INITIALIZING WEAPON
	virtual void InitializeComponent() override;

	void RegisterComponent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Weapon", meta = (DisplayName = "On Weapon Register"))
		void ReceiveWeaponRegister(APiratesAdventureCharacter* Owner);

	UFUNCTION()
		virtual void OnEquipWeapon();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Weapon", meta = (DisplayName = "On Weapon Equip"))
		void ReceiveOnEquipWeapon();

	UFUNCTION()
		virtual void OnUnequipWeapon();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Weapon", meta = (DisplayName = "On Weapon Unequip"))
		void ReceiveOnUnequipWeapon();


	//ATTACK PROPERTY VALUES
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		class UDataTable* AttackDataList;

	////we need this for several IPA_Interface_MeleeAttack functions
	UPROPERTY()
		UAnimSequenceBase* CurrentAttackAnimation;

	UPROPERTY()
		FAttackLookupTable CurrentAttackData;

	UPROPERTY(BlueprintReadOnly, Category = "Attack Properties")
		EPlayerWeaponType CurrentWeaponType;

	UPROPERTY()
		TSubclassOf<class UDamageType> CurrentDamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		TSubclassOf<class UDamageType> DamageTypeDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		TSubclassOf<class UDamageType> DamageTypeFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		TSubclassOf<class UDamageType> DamageTypeIce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		TSubclassOf<class UDamageType> DamageTypeWind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		TSubclassOf<class UDamageType> DamageTypeWood;

	//ELEMENT SWITCHING VARIABLES
	UFUNCTION(BlueprintCallable, Category = "Attacks")
		void SetWeaponElement(EPlayerWeaponType NewElement);

	void RemoveWeaponElement(EPlayerWeaponType CurrentElement);

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Set Element Default"))
		void ReceiveSetElementDefault();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Set Element Fire"))
		void ReceiveSetElementFire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Set Element Ice"))
		void ReceiveSetElementIce();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Set Element Wind"))
		void ReceiveSetElementWind();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Set Element Wood"))
		void ReceiveSetElementWood();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Remove Element Default"))
		void ReceiveRemoveElementDefault();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Remove Element Fire"))
		void ReceiveRemoveElementFire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Remove Element Ice"))
		void ReceiveRemoveElementIce();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Remove Element Wind"))
		void ReceiveRemoveElementWind();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Remove Element Wood"))
		void ReceiveRemoveElementWood();

	//BASIC ATTACK VARIABLES
	/** process the attack and determine when which attack needs to be played; for combos **/
	virtual void ProcessAttack(UAnimSequenceBase* Attack);

	/** Get and set the attack data for the current attack **/
	virtual void ProcessAttackData(FName AttackName);

	/** After an attack has finished, process the information to determine whether to continue a combo or start a charge attack **/
	virtual void ProcessNormalAttackFinished();

	/** After an attack has finished, process the information to determine whether to continue a combo or start a charge attack **/
	virtual void ProcessChargeAttackFinished();

	/** This is used to determine when a button is pressed while an attack is underway. Useful for creating combo chains **/
	virtual void ProcessPressAttackButton();

	/** process the start of the charge attack, so we can enable special effects for the charging **/
	virtual void ProcessStartChargeAttack();

	/** process the start of the charge attack, so we can enable special effects for the charging **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Charge Attack Start"))
		void OnReceiveProcessStartChargeAttack();

	/** When the charge is completed, but the player hasn't let go of the button yet **/
	virtual void ProcessReadyChargeAttack();

	/** When the charge is completed, but the player hasn't let go of the button yet **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Charge Attack Ready"))
		void OnReceiveProcessReadyChargeAttack();

	/** process when charge input has ended; determines whether the player should release the charge or end it **/
	virtual void ProcessStopChargeAttack();

	/** process when charge input has ended; determines whether the player should release the charge or end it **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Charge Attack Release"))
		void OnReceiveProcessStopChargeAttack(bool bChargeCompleted);

	/** for when to interrupt the charge attack even if it is completed (for instance, when the player releases the button during slipturning, or when the player is damaged) **/
	virtual void InterruptChargeAttack();

	/** for when to interrupt the charge attack even if it is completed (for instance, when the player releases the button during slipturning, or when the player is damaged) **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks", meta = (DisplayName = "Charge Attack Interrupt"))
		void OnReceiveProcessInterruptChargeAttack();

	UPROPERTY()
		FTimerHandle ChargeFinishedHandle;

	UPROPERTY()
		FTimerHandle AnimationFinishedHandle;

	//ANIMATION BLUEPRINT VARIABLES
	//These booleans are purely to transition to attack states. They don't actually deal with attacking.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack Animation Variables")
		bool bIsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack Animation Variables")
		bool bIsChargeAttacking;

	//So we can determine whether to continue a combo attack or start/unleash a charge attack.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack Animation Variables")
		int32 ComboCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack Animation Variables")
		bool bCanContinueChargeAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack Animation Variables")
		bool bCanStartChargeAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack Variables")
		float ChargeTime = 1.f;


	//FUNCTIONALITY FOR PROCESSING HIT INFORMATION
	UPROPERTY()
		TArray<AActor*> HitActors;

	UPROPERTY()
		TArray<UPrimitiveComponent*> HitComponents;

	UFUNCTION()
		bool CanHitActor(AActor* HitActor);

	UFUNCTION()
		bool CanHitComponent(UPrimitiveComponent* HitComponent);

	UFUNCTION()
		virtual void WeaponOnHit(
			class AActor* HitActor,
			class UPrimitiveComponent* HitComponent,
			const FVector& ImpactPoint,
			const FVector& ImpactNormal,
			FName HitBoneName,
			int32 DamageMapIndex,
			int32 ShapeIndex,
			const struct FHitResult& HitResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Weapon", meta = (DisplayName = "On Hit With Combo Attack"))
		void ReceiveWeaponOnHitCombo(
			class AActor* HitActor,
			class UPrimitiveComponent* HitComponent,
			const FVector& ImpactPoint,
			const FVector& ImpactNormal,
			FName HitBoneName,
			int32 DamageMapIndex,
			int32 ShapeIndex,
			const struct FHitResult& HitResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Weapon", meta = (DisplayName = "On Hit With Charge Attack"))
		void ReceiveWeaponOnHitCharge(
			class AActor* HitActor,
			class UPrimitiveComponent* HitComponent,
			const FVector& ImpactPoint,
			const FVector& ImpactNormal,
			FName HitBoneName,
			int32 DamageMapIndex,
			int32 ShapeIndex,
			const struct FHitResult& HitResult);


	//FUNCTIONALITY FOR FINALIZING ATTACKING SEQUENCES
	/** After the allotted time has passed, check whether the player has pressed the attack again to initiate the next step in the combo **/
	virtual void StartAttackCooldown();

	virtual void StopAttackCooldown();

	virtual void ResetAttackCooldown();

	UPROPERTY()
		FTimerHandle CooldownHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Properties")
		float CooldownTime = 1.f;

	UPROPERTY()
		bool bAttackIsCoolingDown = false;

	//MISC FUNCTIONALITY
	UPROPERTY()
		bool bDebug = false;

	UPROPERTY()
		APiratesAdventureCharacter* WeaponOwner;

	void StopSwingDamage();

	/** this function makes sure that jump attacks are properly closed upon landing **/
	virtual	void LandFromAttack();

	/** Reset Attack variables**/
	virtual void ResetNormalAttack();

	virtual void ResetChargeAttack();

	UFUNCTION(BlueprintPure, Category = "Player Weapon")
		APiratesAdventureCharacter* GetWeaponOwner() { return WeaponOwner; };
	
	
};
