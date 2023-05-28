// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "Animation/AnimNodeBase.h"
#include "PA_PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetRelativeSpeed();

	/** checks whether current slot is playing any animation or not **/
	UFUNCTION(BlueprintPure, Category = "Animation")
		bool IsSlotActive(FName SlotNodeName = NAME_None);

	UFUNCTION(BlueprintPure, Category = "Attack")
		EPlayerWeaponType GetWeaponType();

	UFUNCTION(BlueprintPure, Category = "Attack")
		void GetToolType(EPlayerToolType& ToolType, bool& ToolIsActive);

	//TEMPORARY FUNCTION UNTIL YOU CAN GET THE DAMN ANIMNODE TO WORK
	UFUNCTION(BlueprintPure, Category = "Animation")
		UAnimSequenceBase* GetWeaponAnimLength(UAnimSequenceBase* Default = NULL, UAnimSequenceBase* Fire = NULL, UAnimSequenceBase* Ice = NULL, UAnimSequenceBase* Wind = NULL, UAnimSequenceBase* Wood = NULL);

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetHorizontalInputDirection();

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetverticalInputDirection();

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetHorizontalAim();

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetverticalAim();

	/** Returns the last known direction on the horizontal axis the player was heading in. Returns 1 or -1**/
	UFUNCTION(BlueprintPure, Category = "Animation")
		int32 GetLastKnownDirection();

	UFUNCTION(BlueprintPure, Category = "Attack")
		bool IsAttacking();

	UFUNCTION(BlueprintPure, Category = "Attack")
		bool IsChargeAttacking();

	UFUNCTION(BlueprintPure, Category = "Attack")
		bool SetAttacking(bool bCanSet, bool bNewValue);

	UFUNCTION(BlueprintPure, Category = "Attack")
		bool SetChargeAttacking(bool bCanSet, bool bNewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = Collision)
		void OnHitSomething(AActor* HitActor);

	UFUNCTION(BlueprintPure, Category = "Attack")
		bool IsUsingTool();

	//DAMAGE
	//StartReceivingDamage is called from within the player. We then use the animinstance to call the stop receiving damage to finalize damage based on animations.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Morgan Damage")
		void StartReceivingDamage(FDamageAnimation DamageAnimationData);

	UFUNCTION(BlueprintCallable, Category = "Morgan Damage")
		void StopReceivingDamage();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Damage", meta = (DisplayName = "Stop Receiving Damage"))
		void ReceiveStopReceivingDamage();

	//DODGE
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Morgan Dodge")
		void StartDodge(FVector DodgeDirection);

	UFUNCTION(BlueprintCallable, Category = "Morgan Dodge")
		void StopDodge();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Dodge", meta = (DisplayName = "Stop Dodge"))
		void ReceiveStopDodge();

	//SHIMMYING
	//TODO: Are we still using any of these?
	UFUNCTION(BlueprintPure, Category = "State Functions | Shimmying")
		void CanShimmy(bool& bCanShimmy);

	UFUNCTION(BlueprintPure, Category = "State Functions | Shimmying")
		void ShimmyDistanceTraceToA(bool& bCanFinishShimmy, float Range = 0.f);

	UFUNCTION(BlueprintPure, Category = "State Functions | Shimmying")
		void ShimmyDistanceTraceToB(bool& bCanFinishShimmy, float Range = 0.f);

	//LEDGE GRABBING
	//TODO: Will these still be necessary if we ever re-implement the whole ledge thing?
	UFUNCTION(BlueprintPure, Category = "State Functions | Ledge Grabbing")
		void HitWallDuringSwing(bool& bHitWall);

	UFUNCTION(BlueprintPure, Category = "State Functions | Ledge Grabbing")
		void CanClimbLedge(bool& bCanClimbLedge);

	UFUNCTION(BlueprintPure, Category = "State Functions | Boat")
		void GetBoatSocketLocationAndRotation(FName SocketName, FVector& SocketLocation, FRotator& SocketRotation);

	//TOOL RELATED FUNCTIONS	
	UFUNCTION(BlueprintPure, Category = "Tool Functions")
		void ForceToolDeactivation(bool Input, bool& Output);

	//ANIM SEQUENCE BASE VARIABLES
	//TODO: remove these. There's seriously no need for them anymore and right now they just clutter everything up. Check if we still have dependencies

	//Normal Combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_ComboA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_ComboB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_ComboC;

	//Run Combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_RunA;

	//Upward Combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_UpwardA;

	//Duck Combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_DuckA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_DuckB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_DuckC;

	//Jump Combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Combo Attack")
		UAnimSequenceBase* Attack_Default_JumpA;

	//Normal Charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_NormalA;

	//Run Charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_RunA;

	//Upward Charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_UpwardA;

	//Duck Charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_DuckA;

	//Jump Charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_JumpStartA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_JumpLoopA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations - Charge Attack")
		UAnimSequenceBase* Charge_Default_JumpEndA;
};