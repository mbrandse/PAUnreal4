// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PiratesAdventureCharacter.h"
#include "PA_PlayerSpringArmComponent.h"
#include "PA_Weapon_Base.h"
//#include "PA_DamageType_Base.h"
#include "PA_BlueprintFunctionLibrary.generated.h"

UENUM()
namespace EAnimateLatentAction
{
	enum Type
	{
		/** Move to target over specified time. */
		Move,
		/** If currently moving, stop. */
		Stop,
		/** If currently moving, return to where you started, over the time elapsed so far. */
		Return
	};
}

UENUM()
enum class EDamagePenetration : uint8
{
	/** The entire explosion will ignore any objects that can stop damage and just deal damage to anything within range */
	DP_PenetrateAll UMETA(DisplayName = "Penetrate All"), 
	/** The core of the explosion will damage everything within its range, but everything else will be subject to the damage prevention channel */
	DP_PenetrateCoreOnly UMETA(DisplayName = "Penetrate Core Only"),
	/** The outer ring of the explosion will damage everything within its range, but the core will be subject to the damage prevention channel */
	DP_PenetrateOuterOnly UMETA(DisplayName = "Penetrate Outer Only"),
	/** The entire explosion will be subject to the damage prevention channel */
	DP_PenetrateNone UMETA(DisplayName = "Penetrate None")
};

USTRUCT(Blueprintable)
struct FCameraComponentTransform
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location = FVector::ZeroVector;
};

USTRUCT(Blueprintable)
struct FNavPolyData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TileID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector TileCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PolyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector PolyCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> PolyVerts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> ConnectedPolys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> NearPolys;

	NavNodeRef PolyIDRef;
};


/**
*
*/
UCLASS()
class PIRATESADVENTURE_API UPA_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", ExpandEnumAsExecs = "MoveAction", OverTime = "1.0"), Category = "Latent Actions")
	//	static void AnimateVelocityTo(class APiratesAdventureCharacter* Character, FVector TargetVelocity, bool bIgnoreX, bool bIgnoreY, bool bIgnoreZ, FRotator TargetRotation, bool bRotate, bool bEaseOut, bool bEaseIn, float OverTime, TEnumAsByte<EAnimateLatentAction::Type> MoveAction, FLatentActionInfo LatentInfo);

	//UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", ExpandEnumAsExecs = "MoveAction", FacingTargetDuration = "0.2", TargetTolerance = 1.f), Category = "Latent Actions")
	//	static void AnimateMovementTo(class ACharacter* Character, FVector TargetLocation, float TargetTolerance, float TargetSpeed, bool bShouldUpdateFacing, FRotator FacingTarget, float FacingTargetDuration, TEnumAsByte<EAnimateLatentAction::Type> MoveAction, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", Tolerance = "1.f", Speed = "5.f"), Category = "Latent Actions")
		static void AnimateSpringArmTo(class AActor* WorldContextObject, class UPA_PlayerSpringArmComponent* SpringArmComponent, bool bInterpolateToPlayer, float Tolerance, float Speed, FVector TargetLocation, bool bInterpLocation, bool bLocationIsComparisonTarget, FRotator TargetRotation, bool bInterpRotation, bool bRotationIsComparisonTarget, float TargetArmLength, bool bInterpArmLength, bool bArmLengthIsComparisonTarget, FLatentActionInfo LatentInfo, float Aperture = 0.05f);

	/*
	//Loads a level by name. Primarily meant for use in C++ classes, but can be used in blueprint as well.
	//
	//Character = Character to get the level name from.
	//LevelToStream = Level to stream in.
	//LastStreamedLevels = An array of levels that are currently active, in case we want to deactivate them but remember them for later use.
	//Callback = If we want to activate a callback function at the moment the levels are loaded, fill in the name of the function here.
	//CallbackOwner = the object which hosts the callback function.
	//UnloadOtherStreamedLevels = If we wish to unload all the other levels, set this to true
	*/
	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
		static bool StreamLevelByName(class ACharacter* Character, FName LevelToStream, TArray<FName> &LastStreamedLevels, FName Callback = NAME_None, class UObject* CallbackOwner = NULL, bool UnloadOtherStreamedLevels = false);

	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
		static bool GetLevelIsStreamed(UObject* WorldContextObject, FName LevelToCheck);

	//-------------------------------------------------------------------------------------------------------
	// INPUT RELATED FUNCTIONS
	//-------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = "Deadzone Correction")
		static float GetModifiedFloat(float Input, float DeadzoneLow, float DeadzoneHigh);

	UFUNCTION(BlueprintPure, Category = "Deadzone Correction")
		static float GetInputStrength(float InputX, float InputY, float DeadzoneLow, float DeadzoneHigh);

	UFUNCTION(BlueprintPure, Category = "Deadzone Correction")
		static void GetCorrectedInput(float InputX, float InputY, float DeadzoneLow, float DeadzoneHigh, float& OutputX, float& OutputY);

	//-------------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------------
	// PLAYER COIN RELATED FUNCTIONS
	//-------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = "Coin")
		static int32 GetCoinCount(class APiratesAdventureCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Coin")
		static void AddCoin(class APiratesAdventureCharacter* Character, int32 CoinNum);

	//-------------------------------------------------------------------------------------------------------

	//Animation related functions
	UFUNCTION(BlueprintCallable, Category = "Animation")
		static bool GetAnimSequenceData(ACharacter* Character, FName StateMachineName, FName StateName, float& AnimLength, float& AnimRemainingTime, float& AnimWeight);

	UFUNCTION(BlueprintPure, Category = "Animation")
		static float GetAnimationLength(UAnimSequenceBase* Animation);

	UFUNCTION(BlueprintPure, Category = "Animation")
		static float GetAttackRelevantTimeRemaining(UAnimInstance* AnimInstance);

	UFUNCTION(BlueprintPure, Category = "Animation")
		static float GetAttackRelevantTimeRemainingRatio(UAnimInstance* AnimInstance);

	//DEPRECATED. USE GETCOMBOCOUNT INSTEAD. Checks whether the combo can continue.
	UFUNCTION(BlueprintPure, Category = "Animation")
		static bool CanContinueCombo(UAnimInstance* AnimInstance);

	//Returns the current combo number. Can be used for the number of attack button presses or to determine how far combos should advance for AI.
	UFUNCTION(BlueprintPure, Category = "Animation")
		static int32 GetComboCount(UAnimInstance* AnimInstance);

	//Checks whether the character can start a charge attack.
	UFUNCTION(BlueprintPure, Category = "Animation")
		static bool CanContinueCharge(UAnimInstance* AnimInstance);

	//Checks whether an attack is being played. Uses a timer within the character, rather than the actual animation, so can be prone to error. Only use when you don't have access to remaining time on actual animation. 
	UFUNCTION(BlueprintPure, Category = "Animation")
		static bool IsAttackPlaying(UAnimInstance* AnimInstance);

	//Takes care of resetting any lingering attack. Is used to make sure that anything missed by animnotifies can be reset, and handles any post attack related variables.
	UFUNCTION(BlueprintPure, Category = "Animation")
		static bool ClearNormalAttack(UAnimInstance* AnimInstance, bool bCanReset = false);

	//Takes care of resetting any lingering attack. Is used to make sure that anything missed by animnotifies can be reset, and handles any post attack related variables.
	UFUNCTION(BlueprintPure, Category = "Animation")
		static bool ClearChargeAttack(UAnimInstance* AnimInstance, bool bCanReset = false);

	//Environmental Calculations
	UFUNCTION(BlueprintCallable, Category = "Environment")
		static float GetLightingAmountAtLocation(AActor* Actor, FVector Location, bool bDrawDebugLines = false);

	//Post Process related functions
	//Sets the new bloom for the PP volumes. Set bForceOverride if you want to force the PP volume to be overwritten, in case the value is not yet made editable. 
	UFUNCTION(BlueprintCallable, Category = "Post Processing")
		static void SetBloomIntensity(ACharacter* Character, float NewBloom, bool bForceOverride = false);

	//get focal data on current pp volumes
	UFUNCTION(BlueprintPure, Category = "Post Processing")
		static void GetDOPFocal(ACharacter* Character, float& FocalDistance, float& FocalRegion);

	//Sets the new focal distance and region for the PP volumes. Set bForceOverride if you want to force the PP volume to be overwritten, in case the value is not yet made editable
	UFUNCTION(BlueprintCallable, Category = "Post Processing")
		static void SetDOPFocal(ACharacter* Character, bool bSetFocalDistance, float NewFocalDistance, bool bSetFocalRegion, float NewFocalRegion, bool bForceOverride = false);

	UFUNCTION(BlueprintCallable, Category = "Post Processing")
		static void SetBlendable(ACharacter* Character, UMaterialInterface* Blendable, float BlendAmount, bool bForceOtherBlendsToZero = false);

	//Makes a screenshot and returns the screenshot path (for use in conjunction with the ImageLoader)
	UFUNCTION(BlueprintCallable, Category = "Post Processing")
		static FString GetScreenshot(UObject* WorldContextObject, FString ScreenshotName);

	//Sets the chromatic abberation for the PP volumes. If Target Priority Volume is not -1.f, it will search for a specific priority volume instead.
	UFUNCTION(BlueprintCallable, Category = "Post Processing")
		static void SetChromaticAbberation(ACharacter* Character, float NewChromaticAbberation, float NewChromaticAbberationStartOffset, bool bForceOverride = false, float TargetPriorityVolume = -1.f);

	//Damage related functions
	//Modified Radial Damage with Falloff that checks for all actors, rather than just dynamic ones. Is heavier, but should be usable on non-Physics objects too. 
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game|Damage", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "IgnoreActors"))
		static bool ApplyRadialDamageWithFalloffAllActors(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser = NULL, AController* InstigatedByController = NULL, EDamagePenetration PenetrationType = EDamagePenetration::DP_PenetrateNone,  ECollisionChannel DamagePreventionChannel = ECC_Visibility);

	static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult);

	//Dialogue related functions
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		static void SpawnDialogue(APlayerController* Controller, TArray<AActor*> NPCs, UDialogue* Dialogue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		static void RemoveDialogue(APlayerController* Controller, UDialogue* Dialogue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		static void SpawnItemGetPopup(ACharacter* Player, class UPA_Asset_Item* ItemData, TSubclassOf<class UPA_HudElements_ItemGet> ItemPopupClass);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		static void RemoveItemGetPopup(ACharacter* Player);

	//Temporary function only. Try to work this into the preprosessor of the widget you are gonna buy
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		static FString GetTextWithEnters(FText InputString);

	//Camera Functions
	UFUNCTION(BlueprintCallable, Category = "Camera")
		static void GetOrthographicCameraFrustumEdges(class UCineCameraComponent* CameraComponent, FVector& TopCenter, FVector& LeftCenter, FVector& BottomCenter, FVector& RightCenter);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		static void GetPerspectiveCameraFrustumEdges(FVector OwnerWorldLocation, class UCineCameraComponent* CameraComponent, class USpringArmComponent* SpringArmComponent, FVector& TopCenter, FVector& LeftCenter, FVector& BottomCenter, FVector& RightCenter);

	//UI functions
	UFUNCTION(BlueprintCallable, Category = "AI")
		static bool GetPolygonsFromNavMesh(const class ACharacter* Character, TArray<FNavPolyData> &PolyRefs);
};