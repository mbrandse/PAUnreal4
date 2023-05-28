// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "PA_Interface_GlobalCharacter.h"
#include "PA_Interface_MeleeAttack.h"
#include "PA_PlayerSpringArmComponent.h"
#include "PA_HudElements_RingMenu.h"
#include "PA_StaticLibrary.h"
#include "Components/CapsuleComponent.h"
#include "PiratesAdventureCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageInterruptSignature, FVector, DamageDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadCheckpointSignature, FName, ComponentTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeDirectionSignature, int32, NewDirection);

//TODO: MOVE THIS INTO THE ITEM ASSET CLASS.
USTRUCT(Blueprintable)
struct FEquipmentData
{
	GENERATED_USTRUCT_BODY()

public:

	//the number of the item in the HUD inventory list.  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
		class USkeletalMesh* EquipmentTemplate;

	UPROPERTY()
		USkeletalMeshComponent* Equipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool bUnlocked;
};

//TODO: MOVE THIS INTO THE ITEM ASSET CLASS.
USTRUCT(Blueprintable)
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		TSubclassOf<UPA_Weapon_Base> WeaponTemplate;

	UPROPERTY()
		UPA_Weapon_Base* Weapon;
};

//TODO: MOVE THIS INTO THE ITEM ASSET CLASS.
USTRUCT(Blueprintable)
struct FToolData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		TSubclassOf<UPA_Tool_Base> ToolTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		EPlayerToolType ToolType;

	UPROPERTY()
		UPA_Tool_Base* Tool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool bUnlocked;
};

UENUM(BlueprintType)
enum class EPlayerStateType : uint8
{
	//GPS = Game Player State 
	//UPS = UI Player State

	GPS_None UMETA(DisplayName = "Game | None"), //default value, not used in classes. Doesn't have any functionality.
	GPS_GameNormal UMETA(DisplayName = "Game | Normal"),
	GPS_GameClimb UMETA(DisplayName = "Game | Climb"),
	GPS_GameShimmy UMETA(DisplayName = "Game | Shimmy"),
	GPS_GameLedgegrab UMETA(DisplayName = "Game | Ledge Grab"),
	GPS_GameWallJump UMETA(DisplayName = "Game | Wall Jump"),
	GPS_GameBoat UMETA(DisplayName = "Game | Boat"),
	GPS_GamePush UMETA(DisplayName = "Game | Push"),
	GPS_GameCarryThrow UMETA(DisplayName = "Game | Carry and Throw"),
	GPS_GameDamage UMETA(DisplayName = "Game | Damage"),
};

USTRUCT(BlueprintType)
struct FPlayerStateController
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPlayerStateType StateID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UPA_PlayerState_Base* StateClass;
};

USTRUCT(BlueprintType)
struct FPlayerMovementMutator
{
	GENERATED_USTRUCT_BODY()

public:
	//An unique name for the mutator so we can prevent multiple mutators of the same source
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MutatorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BrakingFrictionFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxCrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float JumpSingleHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float JumpDoubleHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CustomTimeDilation;

	void DefaultValues()
	{
		MutatorName = "Default";
		GravityScale = 1.f;
		MaxAcceleration = 2048.f;
		BrakingFrictionFactor = 2.f;
		RotationRate = FRotator(0.f, 720.f, 0.f);
		MaxWalkSpeed = 600.f;
		MaxCrouchSpeed = 40.f;
		JumpSingleHeight = 1200.f;
		JumpDoubleHeight = 800.f;
		CustomTimeDilation = 1.f;
	}

	void DefaultMutator()
	{
		MutatorName = "None";
		GravityScale = 1.f;
		MaxAcceleration = 1.f;
		BrakingFrictionFactor = 1.f;
		RotationRate = FRotator(1.f, 1.f, 1.f);
		MaxWalkSpeed = 1.f;
		MaxCrouchSpeed = 1.f;
		JumpSingleHeight = 1.f;
		JumpDoubleHeight = 1.f;
		CustomTimeDilation = 1.f;
	}

};

//TODO: MOVE THIS INTO THE ITEM ASSET CLASS.
USTRUCT(Blueprintable)
struct FContextActionData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		AActor* ContextActor;

	UPROPERTY()
		FText ContextText;

	UPROPERTY()
		FLinearColor ContextColor;

	UPROPERTY()
		bool bShowButton;
};

UCLASS(config=Game)
class APiratesAdventureCharacter : public ACharacter, public IPA_Interface_GlobalCharacter, public IPA_Interface_MeleeAttack
{
	GENERATED_BODY()

		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
		//  GENERIC ACTOR FUNCTIONS
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

public:

	APiratesAdventureCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay();

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  PLAYER COMPONENT FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

private:

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCineCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UPA_PlayerSpringArmComponent* CameraBoom;

	///** Screenshot Capture Component */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//	class USceneCaptureComponent2D* SideViewCaptureComponent;

	///** Camera boom positioning the camera beside the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//	class UPA_Movement_AxisRestraint* AxisConstraint;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Rendering")
		TSubclassOf<class APA_CapturePageFlip> ScreenRecorderTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class APA_CapturePageFlip* ScreenRecorder;

	UPROPERTY()
		float CameraDefaultZoom = 1400.f;

	UPROPERTY()
		FRotator CameraDefaultRotation = FRotator(-15.f, 180.f, 0.f);

	UPROPERTY()
		FVector CameraDefaultLocation = FVector::ZeroVector;

	UPROPERTY()
		float CapsuleComponentDefaultRadius;

	UPROPERTY()
		float CapsuleComponentDefaultHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Movement")
		float CapsuleComponentDuckRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Movement")
		float CapsuleComponentDuckHeight;

private:

	//Frontal collision check capsule is used to determine in advance whether the player is actually touching anything. Necessary for things like ledge grabbing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UCapsuleComponent* FrontalCollisionCheckCapsule;

public:

	//Frontal hit check capsule on the other hand is used to determine the moment the player hits something in front of him. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UCapsuleComponent* FrontalHitCheckCapsule;

	//UPROPERTY()
	//	USphereComponent* ActiveSkillSphere;

public:

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCineCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }

	/** Returns CameraBoom subobject **/
	FORCEINLINE class UPA_PlayerSpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns SideViewCameraComponent subobject **/
	//FORCEINLINE class USceneCaptureComponent2D* GetSideViewCaptureComponent() const { return SideViewCaptureComponent; }

	UFUNCTION(BlueprintCallable, Category = Collision)
		void SetFrontalCollisionCapsuleSize(float NewRadius = 30.f, float NewHeight = 95.f) { FrontalCollisionCheckCapsule->SetCapsuleSize(NewRadius, NewHeight); };

	UFUNCTION(BlueprintCallable, Category = Collision)
		void SetFrontalCollisionCapsuleLocation(FVector NewLocation = FVector(100, 0, 0)) { FrontalCollisionCheckCapsule->SetRelativeLocation(NewLocation); };

	UFUNCTION(BlueprintCallable, Category = Collision)
		void SetFrontalCollisionCapsuleRotation(FRotator NewRotation = FRotator::ZeroRotator) { FrontalCollisionCheckCapsule->SetRelativeRotation(NewRotation); };

	UFUNCTION()
		void GetFrontalCollisionCapsuleOverlaps(TArray<UPrimitiveComponent*>& OverlappingComponents) { FrontalCollisionCheckCapsule->GetOverlappingComponents(OverlappingComponents); };

	UFUNCTION()
		UCapsuleComponent* GetFrontalCollisionCapsuleComponent() { return FrontalCollisionCheckCapsule; };

	//UFUNCTION()
	//	void OnFrontalCapsuleHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnBeginFrontalHitOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//For animation, to make sure we can have bounce animations and the likes when a character hits something during animation
	UFUNCTION()
		void OnBeginOverlapForAnimation(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//UFUNCTION()
	//	void OnBeginOverlapForAnim(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  PLAYER INPUT FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	/** Our own wrapper for the jump function, so we can control whether to do the double jump or whether to jump normally **/
	void PerformJump();

	/** Takes care of basic jump functionality **/
	virtual void Jump();

	/** if character can double jump, will take care of doublejump functionality **/
	void DoubleJump();

	/** necessary to reset bools for jump functions **/
	virtual void Landed(const FHitResult& Hit);

	UPROPERTY(BlueprintAssignable, Category = "Jumping")
		FLandedSignature OnLand;

	/** Called for side to side input */
	void MoveRight(float Val);

	/** Called for up and down input */
	void MoveUp(float Val);

	/** Process attack button presses **/
	virtual void StartAttack();

	/** When attach button is released; necessary for thrust attacks **/
	virtual void StopAttack();

	/** start crouching **/
	//virtual void StartCrouch();

	/** stop crouching **/
	//virtual void StopCrouch();
	
	//records PlayerZ if not falling or jumping.
	float PlayerZ = 0.f;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Movement")
		float GetPlayerZ() { return PlayerZ; };

	/** Perform a context sensitive event **/
	virtual void StartContext();

	virtual void StopContext();

	/** Activate and deactive a tool **/
	virtual void StartTool();
	//virtual void StopTool();

	bool bStartedTool;

	/** Process defend button presses **/
	virtual void StartDefend();

	/** Necessary in order to stop defending, since defence lasts until backdamage or button release **/
	virtual void StopDefend();

	/** Process defend button presses **/
	virtual void StartDodge();

	/** Activate / aim tool (currently only flintlock) **/
	virtual void AimToolRight(float Val);

	virtual void AimToolUp(float Val);

	/** Open up the ring menu within the HUD **/
	virtual void ToggleStart();

	/** Ring Menu Functions **/
	virtual void StartMiscMenu ();

	virtual void StopMiscMenu ();

	virtual void StartElementMenu();

	virtual void StopElementMenu();

	//There are 5 possible things the player can quickly equip at any given time. 
	virtual void ProcessEquipOption(EPlayerWeaponType Weapon, EPlayerToolType Tool, FName Keybinding);

	void EquipOption1() { ProcessEquipOption(EPlayerWeaponType::WEAPON_FIRE, EPlayerToolType::TOOL_GUN, TEXT("Equip1")); };

	void EquipOption2() { ProcessEquipOption(EPlayerWeaponType::WEAPON_ICE, EPlayerToolType::TOOL_LANTARN, TEXT("Equip2")); };

	void EquipOption3() { ProcessEquipOption(EPlayerWeaponType::WEAPON_WIND, EPlayerToolType::TOOL_EYE, TEXT("Equip3")); };

	void EquipOption4() { ProcessEquipOption(EPlayerWeaponType::WEAPON_WOOD, EPlayerToolType::TOOL_GRENADE, TEXT("Equip4")); };

	void EquipOption5() { ProcessEquipOption(EPlayerWeaponType::WEAPON_DEFAULT, EPlayerToolType::TOOL_WAND, TEXT("Equip5")); };

	/** Seperate button for pressing accept on the interface, so we can divide the in-game and UI actions **/
	//virtual void InterfaceAccept();

	//APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  CHECKPOINT FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		FVector ManualCheckpointLocation;

	UPROPERTY()
		FVector RecordedCheckpointLocation;

	UPROPERTY()
		FTimerHandle RecordedCheckpointHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Checkpoint")
		float RecordedCheckpointInterval = 1.f;

	//Updates the recorded checkpoint, but only if conditions are met. Is based on timer, which can be paused. Will be unpaused using Tick.
	void SetRecordedCheckpoint();

	UPROPERTY()
		FName CurrentCheckpointTag;

	UFUNCTION(BlueprintCallable, Category = "Morgan Checkpoint")
		void LoadRecordedCheckpoint();

	UFUNCTION(BlueprintCallable, Category = "Morgan Checkpoint")
		void SetManualCheckpoint(FVector NewLocation, FName AssociatedTag);

	UFUNCTION(BlueprintCallable, Category = "Morgan Checkpoint")
		void LoadManualCheckpoint(float Damage = 0.f, TSubclassOf<class UDamageType> DamageTypeClass = NULL);

	UFUNCTION()
		void LoadCheckPoint(FVector Location, float Damage = 0.f, TSubclassOf<class UDamageType> DamageTypeClass = NULL, bool bLoadBoundChecks = false);

	UPROPERTY(BlueprintAssignable, Category = "Morgan Checkpoint")
		FLoadCheckpointSignature OnLoadCheckpoint;

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  CONTEXT ACTION FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool bCanUseContextActions; //to make sure we can disable the use of context actions

	UPROPERTY()
		TArray<FContextActionData> ContextActorArray;

	//We save context actors in here when we cannot use context actions.
	//TArray<FContextActionData> TempContextActorArray;

	void AddActorToContextArray(AActor* NewActor, FText HUDText = FText::GetEmpty(), FLinearColor Color = FLinearColor::White, bool bShowButton = true);

	//bFlushAllActors; if set to true, we will clear the array and all the HUD text associated with it. Used when we activate a context action by button press.
	void RemoveActorFromContextArray(AActor* ActorToRemove, bool bFlushAllActors = false);

	//AI controllers are way more precise with pathfinding, so during pathfinding we will switch to the controller instead. 
	class AController* ContextController;

	//so we can keep track of when the character is AI or not, so we can properly take care of certain variables
	bool bIsContextAI = false;

	UFUNCTION(BlueprintCallable, Category = "Context Actions")
		class AController* StartContextController();

	UFUNCTION(BlueprintCallable, Category = "Context Actions")
		void StopContextController();

	UFUNCTION(BlueprintPure, Category = "Context Actions")
		bool IsUsingAI() { return bIsContextAI; };

	UFUNCTION(BlueprintPure, Category = "Context Actions")
		bool PlayerIsOnFloor();

	UFUNCTION(BlueprintCallable, Category = "Context Actions")
		void DisableContextActions();

	UFUNCTION(BlueprintCallable, Category = "Context Actions")
		void EnableContextActions();

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  PLAYER ATTACK FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	//IPA_Interface_MeleeAttack functions
	void InitializeAttackSequence(UAnimSequenceBase* Attack);

	//bool CanContinueComboAttack();

	int32 GetComboCount();

	bool CanContinueChargeAttack();

	bool IsAttackPlaying();

	void ResetNormalAttack();

	void ResetChargeAttack();

	float GetAttackRelevantTimeRemaining();

	float GetAttackRelevantTimeRemainingRatio();

	int32 GetActorDirection();

	UPROPERTY()
		bool bIsDefending = false;

	//This function is necessary in case we want to stop defence through having it shattered
	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Attack")
		void ShatterDefense();

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Attack")
		bool IsDefending() { return bIsDefending; };

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  PLAYER MOVEMENT FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		bool bCanDoubleJump = true;

	UPROPERTY()
		bool bIsJumping = false;

	UPROPERTY()
		bool bIsDoubleJumping = false;

	UPROPERTY()
		float JumpSingleHeight = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Movement")
		float JumpDoubleHeight = 800.f;

private:

	bool bDisableMovement = false;

public:

	bool bDisableAutoRotation = false;

	UPROPERTY()
		float VerticalMovementSpeed;

	UPROPERTY()
		float CorrectedVerticalMovementSpeed;

	UPROPERTY()
		float HorizontalMovementSpeed;

	UPROPERTY()
		float CorrectedHorizontalMovementSpeed;

	UPROPERTY()
		float VerticalAimSpeed;

	UPROPERTY()
		float HorizontalAimSpeed;

	UPROPERTY()
		int32 LastKnownDirection;

	UPROPERTY()
		bool bIsStoppingRun = false;

	UPROPERTY()
		bool bIsDucked = false;

	UPROPERTY(BlueprintAssignable, Category = "Morgan Functions || Movement")
		FChangeDirectionSignature OnChangedDirections;

	//The vertical movement speed without any deadzones applied
	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		float GetVerticalMovement() { return VerticalMovementSpeed; };

	//The vertical movement speed after deadzones are applied.
	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		float GetCorrectedVerticalMovement() { return CorrectedVerticalMovementSpeed; };

	//The horizontal movement speed without any deadzones applied
	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		float GetHorizontalMovement() { return HorizontalMovementSpeed; };

	//The horizontal movement speed after deadzones are applied.
	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		float GetCorrectedHorizontalMovement() { return CorrectedHorizontalMovementSpeed; };

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		float GetVerticalAim() { return VerticalAimSpeed; };

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		float GetHorizontalAim() { return HorizontalAimSpeed; };

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		int32 GetLastKnownDirection() { return LastKnownDirection; };

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		bool IsSlipturning();

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		bool IsStoppingRun() { return bIsStoppingRun; };

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		bool IsDucking() { return bIsDucked; };

	//Due to the StartCrouch and EndCrouch functions also modifying the collision capsule offset, we don't want to call those in BP anymore. 
	//Instead, we will use the 4 functions below to call in addition to start/endcrouch and use those 4 functions to set things like camera effects.
	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Functions || Movement")
		void OnMorganDuck();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Functions || Movement")
		void OnMorganUnduck();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Functions || Movement")
		void OnMorganCrouch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Functions || Movement")
		void OnMorganUncrouch();

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void ForceUnCrouch();

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		bool GetMovementDisabled() { return bDisableMovement; };

	//A cheap way to basically make sure we only set the disable movement back to true when everyone is done with it. 
	//SUPER risky however
	UPROPERTY()
		int32 DisableMovementCounter = 0;

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void DisableMovement();

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void EnableMovement(bool bForceMovement = false);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void EnableAutoRotation() { bDisableAutoRotation = false; };

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void DisableAutoRotation() { bDisableAutoRotation = true; };

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void EnableAxisLock();

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void DisableAxisLock();

	UPROPERTY()
		TArray<FPlayerMovementMutator> MovementMutators;

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void UpdatePlayerMutators();

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void SetPlayerMutator(FPlayerMovementMutator NewMutator);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void RemovePlayerMutator(FString MutatorName);

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Movement")
		TArray<FString> GetActiveMutatorNames();

public:
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	// STATE FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	/** Filled using the RegisterStateComponent function. Holds all current active player states so we can swap quickly between them **/
	UPROPERTY()
		TArray<FPlayerStateController> RegisteredStates;

	/** The current state the player is in **/
	UPROPERTY()
		class UPA_PlayerState_Base* CurrentState;

	/** The current state type the player is in **/
	UPROPERTY()
		EPlayerStateType CurrentStateType;

	/** RegisterStateComponent is called from within the state components themselves when they become active. **/
	void RegisterStateComponent(class UPA_PlayerState_Base* NewState);

	/** Calls the End State on the current Player State and then switches to new state (or default if nothing can be found). If bForceStateReset is set, it will call Beginstate even if the state is currently the same. **/
	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Player State")
		void SetNewPlayerState(EPlayerStateType NewState, bool bForceStateReset = false);

	/** Called after the End State in the player state is called **/
	void SwitchPlayerState(EPlayerStateType NewState);

	/** Returns the state if it can be found in the registered states. Otherwise returns NULL **/
	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Player State")
		class UPA_PlayerState_Base* GetRegisteredState(EPlayerStateType State);

	/** Returns the current statetype. May be useful for things like animation blueprints **/
	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Player State")
		EPlayerStateType GetCurrentPlayerState() { return CurrentStateType; };

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  INVENTORY FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

public:

	UPROPERTY()
		class APA_PlayerHUD* PlayerHUD;

	UFUNCTION(BlueprintCallable, Category = "Morgan UI")
		class APA_PlayerHUD* GetPlayerHUD();

	UPROPERTY()
		bool bElementMenuIsActive = false;

	UPROPERTY()
		bool bMiscMenuIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Attack")
		FName WeaponSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
		TSubclassOf<class UPA_Weapon_Base> SwordTemplate;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
	//	TArray<class UPA_Asset_Item*> Elements;

	UPROPERTY()
		class UPA_Weapon_Base* EquippedWeapon;

	UPROPERTY()
		TArray<class UPA_Tool_Base*> Tools;

	UFUNCTION(BlueprintPure, Category = "Morgan Equipment")
		TArray<class UPA_Tool_Base*> GetTools() { return Tools; };

	UPROPERTY()
		UPA_Tool_Base* EquippedTool;

	UPROPERTY()
		EPlayerWeaponType CurrentElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
		TArray<FEquipmentData> Bandanas;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* EquippedBandana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
		TArray<FEquipmentData> Jackets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* EquippedJacket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
		TArray<FEquipmentData> Gloves;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* EquippedGloves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
		TArray<FEquipmentData> Boots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* EquippedBoots;

	//Total item inventory (without equipment, tools or weapons). Use sorting in the HUD to separate the items.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morgan Equipment")
		TArray<class UPA_Asset_Item*> Items;

	//WEAPON EQUIP FUNCTIONALITY
	UFUNCTION()
		void RegisterWeaponComponent(class UPA_Weapon_Base* NewWeapon);

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Equipment")
		UPA_Weapon_Base* GetEquippedWeapon() { return EquippedWeapon; };

	//UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
	//	void EquipWeapon();

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Equipment")
		bool CanElementBeUsed(EPlayerWeaponType ElementToCheck);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetElementUnlocked(EPlayerWeaponType WeaponType, bool bNewUnlock = true);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetElementInactive(EPlayerWeaponType WeaponType, bool bNewInactive);

	//TOOL EQUIP FUNCTIONALITY
	/** RegisterToolComponent is called from within the tool components themselves when they become active. **/
	void RegisterToolComponent(class UPA_Tool_Base* NewTool);

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Equipment")
		UPA_Tool_Base* GetEquippedTool();

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Equipment")
		int32 GetToolIndex(TSubclassOf<UObject> ToolClass);

	UFUNCTION(BlueprintPure, Category = "Morgan Functions || Equipment")
		bool CanToolBeUsed(EPlayerToolType ToolToCheck);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetEquippedTool(EPlayerToolType ToolType);

	//Like ForceEquippedWeapon
	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void ForceEquippedTool(EPlayerToolType ToolType);

	//UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
	//	void RemoveEquippedTool();

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetToolUnlocked(EPlayerToolType ToolType, bool bNewUnlock = true);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetToolInactive(EPlayerToolType ToolType, bool bNewInactive);

	UFUNCTION(BlueprintCallable, Category = "Tool")
		void SetToolsAmmoInfinite(bool bInfinite) { bAllAmmoIsInfinite = bInfinite; };

	//we need this bool here, because tools may not exist at the time of calling the function
	bool bAllAmmoIsInfinite = false;

	//EQUIPMENT EQUIP FUNCTIONALITY
	//since equipment is (largely) cosmetic, we only need setters for equipment at the moment. 
	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetEquippedBandana(int32 Index); //replace later with something more easy to understand, like an ENUM, when we actually have different clothing

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetEquippedJacket(int32 Index); //replace later with something more easy to understand, like an ENUM, when we actually have different clothing

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetEquippedGloves(int32 Index); //replace later with something more easy to understand, like an ENUM, when we actually have different clothing

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Equipment")
		void SetEquippedBoots(int32 Index); //replace later with something more easy to understand, like an ENUM, when we actually have different clothing

	//ITEM EQUIP FUNCTIONALITY
	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Items")
		void AddInventoryItem(class UPA_Asset_Item* NewItem);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Items")
		bool HasItemInInventory(class UPA_Asset_Item* ItemToCheck);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Items")
		void SetInventoryItemInactive(class UPA_Asset_Item* ItemToCheck, bool bNewInactive);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Items")
		void SetInventoryItemActive(class UPA_Asset_Item* ItemToCheck, bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Items")
		void SetInventoryItemRollover(class UPA_Asset_Item* ItemToCheck, bool bNewRollover);

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Items")
		void SetInventoryItemUnlocked(class UPA_Asset_Item* ItemToCheck, bool bNewUnlock);

	//Add functions to activate the item effects, update the HUD representation, ammo, used or not used, etcetera

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  CALLBACKS
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Morgan Functions || Movement")
		void FinalizeSlipTurn();

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  DAMAGE FUNCTIONALITY -> TOTALLY REWORK THIS WHILE USING UE4 RELATED FUNCTIONS. WE REALLY DON'T NEED OUR OWN IMPLEMENTATIONS.
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	/*
	
	For damage, we need the following things to be true;

	-When the player receives heavy to medium damage, the player will be launched back or forwards.
	-During fall the player can incur additional damage from enemies. Small damage will not have any animation (only material feedback). Medium to high damage will relaunch the character
	-When landed, the player cannot be damaged. The invulnerability lasts until after the recovery animation and the invulnerability period.
	-In the case of a 3 hit combo, the 3rd hit will automatically become medium damage, despite the amount of damage incurred.
	-When damage starts, we need to set a bool to see if the player was in air or in land before launching the character. After being launched, the Physics become PHYS_FALLING, so we cannot use that.
	-We need an additional override in the damage dealer, so we can force a certain type of damage animation (necessary for bosses with unique attacks). 
	
	
	*/

	UFUNCTION(BlueprintCallable, Category = "Morgan Damage")
		void OnStartProcessingDamageAnimation(FDamageAnimation DamageAnimationData);

	//UFUNCTION(BlueprintCallable, Category = "Morgan Damage")
		void OnStopProcessingDamageAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Damage", meta = (DisplayName = "On Stop Processing Damage Animation"))
		void ReceiveOnStopProcessingDamageAnimation();

	UPROPERTY(BlueprintAssignable, Category = "Morgan Damage")
		FDamageInterruptSignature OnGetInterruptedByDamage;

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Damage")
		void OnReceiveDamageByLoadingCheckpoint(float Damage, TSubclassOf<class UDamageType> DamageTypeClass);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  CASH FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		int32 CoinCount;

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  HEALTH FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		float RemainingHealth;

	UPROPERTY()
		float RemainingHealthForUI;

	UPROPERTY()
		float OldHealth;

	/** Current max health is the current total amount of health the character can hold. Remaining Health cannot be more than this. */
	UPROPERTY()
		float CurrentMaxHealth = 14.f;

	/** The total amount of max health that the character can gain over the course of the game. Any health upgrades gained that go over this value are ignored. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Health")
		float MaxHealthCap = 19.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Morgan Water Damage")
		float RemainingAir = 1.f;

	//Air decrease is set within volumes, not in character. Airdecrease is calculated as unit per second. 1 means air drains entirely within 1 second.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Morgan Water Damage")
		float AirDecrease = 0.f;

	//if the main character cannot breathe, an air meter will start dropping. If zero, periodical damage is received. Only exiting the affected area or activating the wind lantarn can stop the effect
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Morgan Water Damage")
		bool bCanBreathe = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Morgan Water Damage")
		bool bRefillAir = false;

	UPROPERTY()
		class APA_Volume_Water* CurrentWaterVolume;

	UPROPERTY()
		FTimerHandle WaterDamageHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Water Damage")
		const TSubclassOf<class UDamageType> UnderwaterDamageType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Water Damage")
		float UnderwaterDamage = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Water Damage")
		float UnderwaterDamageInterval = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Morgan Water Damage")
		float UnderwaterAirDecrease = 0.3f;

	void ApplyWaterDamage();

	UFUNCTION(BlueprintCallable, Category = "Morgan Water Damage")
		void RefillAir(bool Refill) { bRefillAir = Refill; };

	/** Returns current possible max health based on the current upgrades.**/
	UFUNCTION(BlueprintPure, Category = "Morgan Health")
		float GetMaxHealth() { return CurrentMaxHealth; };

	UFUNCTION(BlueprintPure, Category = "Morgan Health")
		float GetRemainingHealth() { return RemainingHealth; };

	UFUNCTION(BlueprintPure, Category = "Morgan Health")
		float GetRemainingHealthForUI() { return RemainingHealthForUI; };

	UFUNCTION(BlueprintPure, Category = "Morgan Health")
		float GetOldHealth() { return OldHealth; };

	UFUNCTION(BlueprintCallable, Category = "Morgan Health")
		void UpgradeHealth();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Health", meta = (DisplayName = "On Upgrade Health"))
		void ReceiveUpgradeHealth(bool RequiredRecovery);

	//This function maxes out health without calling the associated animation functions. This is necessary especially during health upgrades, since those work differently from normal recovers.
	UFUNCTION(BlueprintCallable, Category = "Morgan Health")
		void MaximizeHealthValuesInternal();

	UFUNCTION(BlueprintCallable, Category = "Morgan Health")
		void ProcessRecovery(float RecoveryAmount, bool bFullRecover = false);

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Health", meta = (DisplayName = "On Process Recovery"))
		void ReceiveProcessRecovery();

	UFUNCTION(BlueprintCallable, Category = "Morgan Health")
		void ProcessDamage(float DamageAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Health", meta = (DisplayName = "On Process Damage"))
		void ReceiveProcessDamage();

	UFUNCTION()
		void OnDead();

	UFUNCTION(BlueprintImplementableEvent, Category = "Morgan Health", meta = (DisplayName = "On Dead"))
		void ReceiveOnDead();

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  MISC FUNCTIONALITY
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	virtual void UpdateScalarMaterialParameterSet(UMaterialParameterCollection* Collection, FName ParameterName, float Value);

	//this function assumes that the RenderCustomDepth has already been set to true.
	UFUNCTION(BlueprintCallable, Category = "Rendering")
		void SetNewStencilValueForAll(int32 NewStencilValue);

	UFUNCTION(BlueprintCallable, Category = "UI testing")
		void OpenMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI testing")
		void CloseMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI testing")
		void OpenIngameMenu();

	UFUNCTION(BlueprintCallable, Category = "UI testing")
		void CloseIngameMenu();
};
