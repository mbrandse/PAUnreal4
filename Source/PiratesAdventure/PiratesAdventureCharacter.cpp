// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PiratesAdventureCharacter.h"
#include "PiratesAdventure.h"
#include "Components/CapsuleComponent.h"
#include "PA_PlayerSpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PA_CharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "PA_Weapon_Base.h"
#include "PA_Tool_Base.h"
#include "PA_PlayerState_Normal.h"
#include "PA_PlayerState_LedgeGrab.h"
#include "PA_PlayerHUD.h"
#include "PA_Volume_Water.h"
//#include "PA_HudElements_Status.h"
//#include "PA_HudElements_IngameMenu.h"
//#include "PA_HudElements_RingMenu.h"
//#include "PA_HudElements_ContextPopup.h"
#include "PA_Interface_ContextAction.h"
#include "PA_PlayerAnimInstance.h"
#include "PA_Asset_Item.h"
#include "PA_ISMA_Base.h"
#include "GameFramework/Controller.h"
#include "PA_CapturePageFlip.h"
#include "CineCameraComponent.h"

//---------------------------------------------------------------------------------------------------------------//
// INIT FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

APiratesAdventureCharacter::APiratesAdventureCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPA_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetHiddenInGame(true);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	CapsuleComponentDuckRadius = 42.f;
	CapsuleComponentDuckHeight = 48.f;

	FrontalCollisionCheckCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FrontalCollisionCheckCapsule"));
	FrontalCollisionCheckCapsule->SetupAttachment(RootComponent);
	FrontalCollisionCheckCapsule->InitCapsuleSize(30.f, 90.f);// (30.f, 95.f); //make sure it's always slightly smaller than the main collision capsule.
	FrontalCollisionCheckCapsule->ShapeColor = FColor::Green;
	FrontalCollisionCheckCapsule->RelativeLocation = FVector(100, 0, 0);
	FrontalCollisionCheckCapsule->SetHiddenInGame(true);
	//FrontalCollisionCheckCapsule->SetGenerateOverlapEvents(true);
	//FrontalCollisionCheckCapsule->OnComponentBeginOverlap.AddDynamic(this, &APiratesAdventureCharacter::OnBeginOverlapForAnimation);

	FrontalHitCheckCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FrontalHitCheckCapsule"));
	FrontalHitCheckCapsule->SetupAttachment(RootComponent);
	FrontalHitCheckCapsule->InitCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), 1.f);
	FrontalHitCheckCapsule->ShapeColor = FColor::White;
	FrontalHitCheckCapsule->RelativeLocation = FVector(0.f, 0.f, -20.f);
	FrontalHitCheckCapsule->SetHiddenInGame(true);
	FrontalHitCheckCapsule->SetGenerateOverlapEvents(true);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<UPA_PlayerSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 1400;
	//CameraBoom->BoomOwner = this;

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera
	SideViewCameraComponent->LensSettings.MinFStop = 0.01f;
	SideViewCameraComponent->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
	SideViewCameraComponent->FocusSettings.ManualFocusDistance = 1400.f;
	SideViewCameraComponent->CurrentFocalLength = 18.f; //18.f corresponds to a 90 degrees FOV, which is what we have used so far for the game camera.
	SideViewCameraComponent->CurrentAperture = 0.05f; //0.05f gives us a somewhat decent DOP. It's an unrealistic Aperture, but who cares. 

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->GravityScale = 2.8f;
	GetCharacterMovement()->AirControl = 2.0f;
	GetCharacterMovement()->JumpZVelocity = JumpSingleHeight;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(250);

	//setup clothing
	EquippedBandana = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquippedBandana"));
	EquippedBandana->SetupAttachment(GetMesh());
	EquippedBandana->SetMasterPoseComponent(GetMesh());
	EquippedBandana->SetRenderCustomDepth(true);
	EquippedBandana->SetCustomDepthStencilValue(250);

	EquippedJacket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquippedJacket"));
	EquippedJacket->SetupAttachment(GetMesh());
	EquippedJacket->SetMasterPoseComponent(GetMesh());
	EquippedJacket->SetRenderCustomDepth(true);
	EquippedJacket->SetCustomDepthStencilValue(250);

	EquippedGloves = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquippedGloves"));
	EquippedGloves->SetupAttachment(GetMesh());
	EquippedGloves->SetMasterPoseComponent(GetMesh());
	EquippedGloves->SetRenderCustomDepth(true);
	EquippedGloves->SetCustomDepthStencilValue(250);

	EquippedBoots = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquippedBoots"));
	EquippedBoots->SetupAttachment(GetMesh());
	EquippedBoots->SetMasterPoseComponent(GetMesh());
	EquippedBoots->SetRenderCustomDepth(true);
	EquippedBoots->SetCustomDepthStencilValue(250);

	bStartedTool = false;
	bCanUseContextActions = true;
	CurrentElement = EPlayerWeaponType::WEAPON_DEFAULT;

	AutoPossessAI = EAutoPossessAI::PlacedInWorld;
}

//---------------------------------------------------------------------------------------------------------------//
// ACTOR FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//
void APiratesAdventureCharacter::BeginPlay()
{
	//TODO: Make sure we initialize this before we actually start beginplay. Otherwise the initial values are not picked up in-game
	RemainingHealth = CurrentMaxHealth;
	RemainingHealthForUI = RemainingHealth;
	OldHealth = RemainingHealth;

	Super::BeginPlay();

	APlayerController* TargetPC = Cast<APlayerController>(GetController());
	if (TargetPC)
	{
		//TODO: TURN THIS ON FOR PACKAGED GAME
		//TargetPC->ConsoleCommand("DISABLEALLSCREENMESSAGES", true);

		//setup the HUD variable
		PlayerHUD = Cast<APA_PlayerHUD>(TargetPC->GetHUD());
	}

	//Create a screenrecorder
	UWorld* World = GetWorld();
	if (World && ScreenRecorderTemplate)
	{
		ScreenRecorder = World->SpawnActor<APA_CapturePageFlip>(ScreenRecorderTemplate);
		//ScreenRecorder->AttachToComponent(GetCameraBoom(), FAttachmentTransformRules::KeepRelativeTransform);
		ScreenRecorder->AttachToComponent(GetSideViewCameraComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FTransform Transform = FTransform::Identity;
		ScreenRecorder->SetActorRelativeTransform(Transform);
		ScreenRecorder->CaptureOwner = this;
	}
}

void APiratesAdventureCharacter::SetupPlayerInputComponent(class UInputComponent* NewInputComponent)
{
	// Action Bindings
	NewInputComponent->BindAction("Jump", IE_Pressed, this, &APiratesAdventureCharacter::PerformJump);
	NewInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	NewInputComponent->BindAction("Attack", IE_Pressed, this, &APiratesAdventureCharacter::StartAttack);
	NewInputComponent->BindAction("Attack", IE_Released, this, &APiratesAdventureCharacter::StopAttack);
	NewInputComponent->BindAction("Context", IE_Pressed, this, &APiratesAdventureCharacter::StartContext);
	NewInputComponent->BindAction("Context", IE_Released, this, &APiratesAdventureCharacter::StopContext);
	NewInputComponent->BindAction("StartMenu", IE_Pressed, this, &APiratesAdventureCharacter::ToggleStart).bExecuteWhenPaused = true;
	NewInputComponent->BindAction("ToggleTool", IE_Pressed, this, &APiratesAdventureCharacter::StartTool);
	NewInputComponent->BindAction("Defense", IE_Pressed, this, &APiratesAdventureCharacter::StartDefend);
	NewInputComponent->BindAction("Defense", IE_Released, this, &APiratesAdventureCharacter::StopDefend);
	NewInputComponent->BindAction("Dodge", IE_Pressed, this, &APiratesAdventureCharacter::StartDodge);

	NewInputComponent->BindAction("SwapEquipMisc", IE_Pressed, this, &APiratesAdventureCharacter::StartMiscMenu);
	NewInputComponent->BindAction("SwapEquipMisc", IE_Released, this, &APiratesAdventureCharacter::StopMiscMenu);
	NewInputComponent->BindAction("SwapEquipElement", IE_Pressed, this, &APiratesAdventureCharacter::StartElementMenu);
	NewInputComponent->BindAction("SwapEquipElement", IE_Released, this, &APiratesAdventureCharacter::StopElementMenu);
	NewInputComponent->BindAction("EquipOption1", IE_Pressed, this, &APiratesAdventureCharacter::EquipOption1);
	NewInputComponent->BindAction("EquipOption2", IE_Pressed, this, &APiratesAdventureCharacter::EquipOption2);
	NewInputComponent->BindAction("EquipOption3", IE_Pressed, this, &APiratesAdventureCharacter::EquipOption3);
	NewInputComponent->BindAction("EquipOption4", IE_Pressed, this, &APiratesAdventureCharacter::EquipOption4);
	NewInputComponent->BindAction("EquipOption5", IE_Pressed, this, &APiratesAdventureCharacter::EquipOption5);

	// Axis bBindings
	NewInputComponent->BindAxis("MoveRight", this, &APiratesAdventureCharacter::MoveRight).bExecuteWhenPaused = true;
	NewInputComponent->BindAxis("MoveUp", this, &APiratesAdventureCharacter::MoveUp).bExecuteWhenPaused = true;
	NewInputComponent->BindAxis("RightAnalog_MoveRight", this, &APiratesAdventureCharacter::AimToolRight);
	NewInputComponent->BindAxis("RightAnalog_MoveUp", this, &APiratesAdventureCharacter::AimToolUp);
}

void APiratesAdventureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentStateType == EPlayerStateType::GPS_GameNormal)
	{
		if (GetCharacterMovement()->Velocity.Z == 0.f)
		{
			PlayerZ = GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}

		if (GetCharacterMovement()->Velocity.Z < 0.f)
		{
			//do something; this is for ledge trace implementation
		}

		////If our recorded checkpoint timer is for some reason not active activate it
		//if (!GetWorld()->GetTimerManager().IsTimerActive(RecordedCheckpointHandle))
		//{
		//	GetWorld()->GetTimerManager().SetTimer(RecordedCheckpointHandle, this, &APiratesAdventureCharacter::SetRecordedCheckpoint, RecordedCheckpointInterval, true);
		//}

		if (bIsContextAI)
		{
			//if we are currently using AI, on the normal state, force the player to uncrouch/unduck
			if (bIsCrouched)
			{
				UPA_CharacterMovementComponent* PACharacterMovement = Cast<UPA_CharacterMovementComponent>(GetCharacterMovement());
				if (PACharacterMovement)
					PACharacterMovement->bWantsToCrouch = false;
			}

			if (bIsDucked)
			{
				UPA_CharacterMovementComponent* PACharacterMovement = Cast<UPA_CharacterMovementComponent>(GetCharacterMovement());
				if (PACharacterMovement)
					PACharacterMovement->bWantsToDuck = false;
			}
		}
	}

	if (ContextActorArray.Num() != 0)
	{
		TArray<AActor*> OverlappingActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, APA_ISMA_Base::StaticClass());
		if (OverlappingActors.Num() == 0)
		{
			//If the requested array is empty, we can just clear out the entire context actor array
			RemoveActorFromContextArray(NULL, true);
		}
		else
		{
			//do a check to see whether the current actor is still inside
			for (FContextActionData ContextData : ContextActorArray)
			{
				bool bHasActor = false;

				for (AActor* OverlapActor : OverlappingActors)
				{
					if (ContextData.ContextActor == OverlapActor)
						bHasActor = true;
				}

				if (!bHasActor)
					RemoveActorFromContextArray(ContextData.ContextActor);
			}
		}
	}

	//First update our character if we have a water volume
	if (CurrentWaterVolume != NULL)
	{
		if (CurrentWaterVolume->IsPlayerWithinVolume())
		{
			AirDecrease = UnderwaterAirDecrease;
		}
		else
		{
			AirDecrease = 0.f;
			if (GetWorld()->GetTimerManager().IsTimerActive(WaterDamageHandle))
				GetWorld()->GetTimerManager().ClearTimer(WaterDamageHandle);
		}
	}

	//If we have air decrease, we can safely say the character cannot breathe. 
	if (AirDecrease != 0.f)
	{
		bCanBreathe = false;

		if (GetEquippedTool()
			&& GetEquippedTool()->ToolType == EPlayerToolType::TOOL_LANTARN
			&& GetEquippedTool()->bToolIsActive
			&& GetEquippedTool()->GetToolElement() == EPlayerWeaponType::WEAPON_WIND
			|| bRefillAir)
		{
			if (RemainingAir < 1.f)
			{
				RemainingAir += 0.5f * DeltaTime;

				if (GetWorld()->GetTimerManager().IsTimerActive(WaterDamageHandle))
					GetWorld()->GetTimerManager().ClearTimer(WaterDamageHandle);
			}
		}
		else
		{
			RemainingAir -= AirDecrease * DeltaTime;
			if (RemainingAir < 0.f)
			{
				RemainingAir = 0.f;
				if (CurrentWaterVolume
					&& !GetWorld()->GetTimerManager().IsTimerActive(WaterDamageHandle))
				{
					GetWorld()->GetTimerManager().SetTimer(WaterDamageHandle, this, &APiratesAdventureCharacter::ApplyWaterDamage, UnderwaterDamageInterval, true);
					ApplyWaterDamage();
				}
			}
		}
	}
	else
	{
		bCanBreathe = true;

		if (RemainingAir < 1.f)
		{
			RemainingAir += 1.f * DeltaTime;
		}
	}

	//if (DisableMovementRequesters.Num() != 0)
	//{
	//	for (int32 Index; Index < DisableMovementRequesters.Num(); Index++)
	//	{
	//		if (DisableMovementRequesters[Index] == NULL)
	//			DisableMovementRequesters.RemoveAt(Index);
	//	}
	//}

	////Check whether we can latch unto ledges, but only when we are falling.
	//if (CurrentStateType == EPlayerStateType::GPS_GameNormal
	//	&& GetCharacterMovement()->Velocity.Z < 0.f)
	//{
	//	//UPA_PlayerState_LedgeGrab* LedgeState = Cast<UPA_PlayerState_LedgeGrab>(GetRegisteredState(EPlayerStateType::GPS_GameLedgegrab));

	//	//TODO: fix ledge state. Currently it crashes on everything.

	//	//if (LedgeState
	//	//	&& LedgeState->bStateIsEnabled)
	//	//{
	//	//	FrontalCollisionCheckCapsule->GetOverlappingComponents(LedgeState->OverlappedComponents);
	//	//	if (LedgeState->OverlappedComponents.Num() > 0)
	//	//	{
	//	//		LedgeState->FindLedge(&LedgeState->OverlappedComponents);
	//	//	}
	//	//}
	//}
}

void APiratesAdventureCharacter::ApplyWaterDamage()
{
	if (UnderwaterDamageType)
	{
		UDamageType* DamageTypeObject = UnderwaterDamageType->GetDefaultObject<UDamageType>();

		IPA_Interface_GlobalCharacter* Interface = Cast<IPA_Interface_GlobalCharacter>(this);
		if (Interface)
		{
			if (Interface->Execute_CanTakeDamage(this))
			{
				Interface->Execute_OnProcessTimedDamage(this, DamageTypeObject, UnderwaterDamage, EPlayerWeaponType::NONE);
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------//
// COMPONENT FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

void APiratesAdventureCharacter::OnBeginFrontalHitOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherComp->GetAttachmentRoot() != GetRootComponent()
		&& OtherComp->GetCollisionProfileName() == TEXT("BlockAll"))
	{
		
	}
}

void APiratesAdventureCharacter::OnBeginOverlapForAnimation(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UPA_PlayerAnimInstance* PlayerAnimInstance = Cast<UPA_PlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (PlayerAnimInstance)
		PlayerAnimInstance->OnHitSomething(OtherActor);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//  CHECKPOINT RELATED FUNCTIONS
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

void APiratesAdventureCharacter::SetRecordedCheckpoint()
{
	//Add additional conditions here for updating the manual checkpoint.
	if (GetVelocity().Z == 0.f
		&& CurrentWaterVolume == NULL
		&& CurrentState->StateID == EPlayerStateType::GPS_GameNormal
		&& !IsUsingAI())
	{
		RecordedCheckpointLocation = GetActorLocation();
	}
}

void APiratesAdventureCharacter::SetManualCheckpoint(FVector NewLocation, FName AssociatedTag)
{
	ManualCheckpointLocation = NewLocation;
	CurrentCheckpointTag = AssociatedTag;
}

void APiratesAdventureCharacter::LoadManualCheckpoint(float Damage, TSubclassOf<UDamageType> DamageTypeClass)
{
	LoadCheckPoint(ManualCheckpointLocation, Damage, DamageTypeClass, true);
}

void APiratesAdventureCharacter::LoadRecordedCheckpoint()
{	
	LoadCheckPoint(RecordedCheckpointLocation);
}

void APiratesAdventureCharacter::LoadCheckPoint(FVector Location, float Damage /* = 0.f */, TSubclassOf<class UDamageType> DamageTypeClass /* = NULL */, bool bLoadBoundChecks /* = false */)
{
	if (CurrentState
		&& CurrentState->StateID != EPlayerStateType::GPS_GameNormal
		&& GetRegisteredState(EPlayerStateType::GPS_GameNormal))
		SetNewPlayerState(EPlayerStateType::GPS_GameNormal);

	if (EquippedTool)
	{
		//Reset any tools that are active at the moment.
		if (EquippedTool->bToolIsActive)
			EquippedTool->StopTool();

		if (EquippedTool->bToolIsAiming)
			EquippedTool->StopAimTool();
	}

	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics); //make sure to teleport physics so our ragdoll parts don't go weird. 
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	EnableMovement(true);

	bIsJumping = false;
	bIsDoubleJumping = false;

	if (Damage != 0.f
		&& DamageTypeClass)
	{
		OnReceiveDamageByLoadingCheckpoint(Damage, DamageTypeClass);
	}

	if (OnLoadCheckpoint.IsBound()
		&& bLoadBoundChecks)
		OnLoadCheckpoint.Broadcast(CurrentCheckpointTag);
}

//---------------------------------------------------------------------------------------------------------------//
// STATE RELATED FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

void APiratesAdventureCharacter::RegisterStateComponent(UPA_PlayerState_Base* NewState)
{
	if (NewState)
	{
		if (NewState->StateID != EPlayerStateType::GPS_None)
		{
			FPlayerStateController NewStateController;
			NewStateController.StateID = NewState->StateID;
			NewStateController.StateClass = NewState;

			RegisteredStates.Add(NewStateController);

			if (NewState->bIsDefaultState)
			{
				CurrentState = NewState;
				CurrentStateType = NewState->StateID;
			}

			//UE_LOG(AnyLog, Warning, TEXT("Has registered %s's state component"), *(RegisteredStates[RegisteredStates.Num() - 1].StateClass->GetName()));
		}
	}
}

UPA_PlayerState_Base* APiratesAdventureCharacter::GetRegisteredState(EPlayerStateType State)
{
	if (RegisteredStates.Num() != 0)
	{
		for (int i = 0; i < RegisteredStates.Num(); i++)
		{
			if (State == RegisteredStates[i].StateID)
			{
				return RegisteredStates[i].StateClass;
			}
		}
	}

	return NULL;
}

void APiratesAdventureCharacter::SwitchPlayerState(EPlayerStateType NewState)
{
	if (GetRegisteredState(NewState))
	{
		for (int i = 0; i < RegisteredStates.Num(); i++)
		{
			if (NewState == RegisteredStates[i].StateID)
			{
				if (CurrentState == RegisteredStates[i].StateClass)
					return;
				else
				{
					CurrentState = RegisteredStates[i].StateClass;
					CurrentStateType = RegisteredStates[i].StateID;
					CurrentState->BeginState();
				}
			}
		}
	}
}

void APiratesAdventureCharacter::SetNewPlayerState(EPlayerStateType NewState, bool bForceStateReset)
{
	if (CurrentState->StateID != NewState)
	{
		CurrentState->EndState(NewState);
	}
	else if (bForceStateReset)
	{
		CurrentState->BeginState();
	}
}

//---------------------------------------------------------------------------------------------------------------//
// INPUT FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//


void APiratesAdventureCharacter::MoveUp(float Value)
{
	VerticalMovementSpeed = Value;

	if (CurrentState && !UGameplayStatics::IsGamePaused(GetWorld()) && !bDisableMovement)
	{
		CurrentState->MoveUp(Value);
	}
}

void APiratesAdventureCharacter::MoveRight(float Value)
{
	HorizontalMovementSpeed = Value;

	if (Value != 0.f || LastKnownDirection == 0)
	{
		if ((int32)Value != 0 
			&& (int32)Value != LastKnownDirection
			&& OnChangedDirections.IsBound())
		{
			OnChangedDirections.Broadcast(LastKnownDirection * -1);
		}
		LastKnownDirection = Value < 0.f ? -1 : 1; //right now this implementation will cause LastKnownDirection to initialize at 1.
	}

	if (CurrentState && !UGameplayStatics::IsGamePaused(GetWorld()) && !bDisableMovement)
	{
		CurrentState->MoveRight(Value);
	}
}

void APiratesAdventureCharacter::AimToolRight(float Val)
{
	HorizontalAimSpeed = Val;

	if (CurrentState)
	{
		CurrentState->AimToolRight(Val);
	}
}

void APiratesAdventureCharacter::AimToolUp(float Val)
{
	VerticalAimSpeed = Val;

	if (CurrentState)
	{
		CurrentState->AimToolUp(Val);
	}
}

void APiratesAdventureCharacter::StartTool()
{
	if (CurrentState)
	{
		CurrentState->StartTool();
	}
}

void APiratesAdventureCharacter::FinalizeSlipTurn()
{

}

bool APiratesAdventureCharacter::IsSlipturning()
{
	return false;
}

void APiratesAdventureCharacter::PerformJump()
{
	if (CurrentState)
	{
		CurrentState->PerformJump();
	}
}

void APiratesAdventureCharacter::Jump()
{
	//Only jump when we are not already falling. Otherwise skip to DoubleJump right away. 
	if (GetVelocity().Z == 0.f)
	{
		Super::Jump();
	}
	else
	{
		bIsJumping = true;
		DoubleJump();
	}
}

void APiratesAdventureCharacter::DoubleJump()
{
	if (JumpDoubleHeight > 0)
	{
		bIsDoubleJumping = true;

		LaunchCharacter(FVector(0.f, 0.f, JumpDoubleHeight), false, true);
	}
}

void APiratesAdventureCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bIsJumping = false;
	bIsDoubleJumping = false;

	if (CurrentState)
	{
		CurrentState->Landed(Hit);
	}

	if (OnLand.IsBound())
		OnLand.Broadcast(Hit);
}

//---------------------------------------------------------------------------------------------------------------//
// ATTACK FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

void APiratesAdventureCharacter::StartAttack()
{
	//UE_LOG(AnyLog, Log, TEXT("Pressing Attack Button"));

	if (CurrentState)
	{
		CurrentState->StartAttack();
	}
}

void APiratesAdventureCharacter::StopAttack()
{
	if (CurrentState)
	{
		CurrentState->StopAttack();
	}
}

void APiratesAdventureCharacter::StartDefend()
{
	if (CurrentState)
	{
		CurrentState->StartDefend();
	}
}

void APiratesAdventureCharacter::StopDefend()
{
	if (CurrentState)
	{
		CurrentState->StopDefend();
	}
}

void APiratesAdventureCharacter::ShatterDefense()
{
	bIsDefending = false;
}

void APiratesAdventureCharacter::StartDodge()
{
	if (CurrentState)
	{
		CurrentState->StartDodge();
	}
}

//keeping this function inside of the character is okay.
void APiratesAdventureCharacter::InitializeAttackSequence(UAnimSequenceBase* Attack)
{

	if (EquippedWeapon)
	{
		EquippedWeapon->ProcessAttack(Attack);
	}
}

bool APiratesAdventureCharacter::IsAttackPlaying()
{
	if (GetWorld() && EquippedWeapon)
	{
		return GetWorld()->GetTimerManager().IsTimerActive(EquippedWeapon->AnimationFinishedHandle);
	}

	return false;
}

float APiratesAdventureCharacter::GetAttackRelevantTimeRemaining()
{
	if (EquippedWeapon
		&& GetWorld()
		&& GetWorld()->GetTimerManager().IsTimerActive(EquippedWeapon->AnimationFinishedHandle))
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(EquippedWeapon->AnimationFinishedHandle);
	}

	return 0.f;
}

float APiratesAdventureCharacter::GetAttackRelevantTimeRemainingRatio()
{
	if (EquippedWeapon
		&& EquippedWeapon->CurrentAttackAnimation
		&& GetWorld()
		&& GetWorld()->GetTimerManager().IsTimerActive(EquippedWeapon->AnimationFinishedHandle))
	{
		float TotalTime = EquippedWeapon->CurrentAttackAnimation->GetPlayLength() / EquippedWeapon->CurrentAttackAnimation->RateScale;
		float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(EquippedWeapon->AnimationFinishedHandle);

		return RemainingTime / TotalTime;
	}

	return 0.f;
}

int32 APiratesAdventureCharacter::GetComboCount()
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->ComboCount;
	}

	return INDEX_NONE;
}

bool APiratesAdventureCharacter::CanContinueChargeAttack()
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->bCanContinueChargeAttack;
	}

	return false;
}

void APiratesAdventureCharacter::ResetNormalAttack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->bIsAttacking = false;
		EquippedWeapon->StartAttackCooldown();
		EquippedWeapon->ResetNormalAttack();
	}
}

void APiratesAdventureCharacter::ResetChargeAttack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->bIsChargeAttacking = false;
		EquippedWeapon->StartAttackCooldown();
		EquippedWeapon->ResetChargeAttack();
	}
}

int32 APiratesAdventureCharacter::GetActorDirection()
{
	return LastKnownDirection;
}

void APiratesAdventureCharacter::StartContext()
{
	if (bCanUseContextActions)
	{
		if (ContextActorArray.Num() != 0
			&& ContextActorArray[0].ContextActor != NULL
			&& bCanUseContextActions)
		{
			IPA_Interface_ContextAction* ContextAction = Cast<IPA_Interface_ContextAction>(ContextActorArray[0].ContextActor);
			if (ContextAction)
			{
				ContextAction->StartContextAction(this);
			}
		}
	}
	else
	{
		if (CurrentState)
		{
			CurrentState->StartContext();
		}
	}
}

void APiratesAdventureCharacter::StopContext()
{
	if (bCanUseContextActions)
	{
		//TODO: MAKE SURE WE FIRST CHECK WHETHER THERE ARE NOT OTHER CONTEXT ACTIONS BELOW THE STACK, FOR FEEDBACK PURPOSES.
		if (ContextActorArray.Num() != 0 && ContextActorArray[0].ContextActor != NULL)
		{
			IPA_Interface_ContextAction* ContextAction = Cast<IPA_Interface_ContextAction>(ContextActorArray[0].ContextActor);
			if (ContextAction)
			{
				ContextAction->StopContextAction(this);
			}
		}
	}
	else
	{
		if (CurrentState)
		{
			CurrentState->StopContext();
		}
	}
}

//TODO: MOVE THIS TO CONTEXT ACTION AND LET THE CONTEXT ACTORS CALL THE HUD INSTEAD. ITS AN HUD ACTION, NO REASON TO KEEP IT HERE.
void APiratesAdventureCharacter::AddActorToContextArray(AActor* NewActor, FText HUDText, FLinearColor Color, bool bShowButton)
{
	//TODO: double check functionality here; sometimes context actions don't register correctly and pressing the button won't do anything then (even though the HUD is okay).
	UE_LOG(AnyLog, Log, TEXT("TRYING TO ADD ACTOR TO CONTEXT ARRAY"));

	if (NewActor
		&& bCanUseContextActions)
	{
		for (FContextActionData ActionData : ContextActorArray)
		{
			if (ActionData.ContextActor && ActionData.ContextActor == NewActor) return;
		}

		FContextActionData NewContext;
		NewContext.ContextActor = NewActor;
		NewContext.ContextText = HUDText;
		NewContext.ContextColor = Color;
		NewContext.bShowButton = bShowButton;

		//if (Index == INDEX_NONE ||
		//	ContextActorArray.Num() == 0)
		//{
			//bool bHUDAlreadyPopulated = ContextActorArray.IsValidIndex(0);
			ContextActorArray.Insert(NewContext, 0);

			if (GetPlayerHUD() != NULL
				&& !HUDText.IsEmpty())
			{
				GetPlayerHUD()->SetContextText(HUDText, Color);
			}
		//}
	}
}

void APiratesAdventureCharacter::RemoveActorFromContextArray(AActor* ActorToRemove, bool bFlushAllActors)
{
	if (GetPlayerHUD() != NULL)
	{
		if (bFlushAllActors)
		{
			ContextActorArray.Empty();
			GetPlayerHUD()->ClearContextText();

			return;
		}

		UWorld* World = GetWorld();

		if (ActorToRemove
			&& World)
		{
			for (int32 Index = 0; Index < ContextActorArray.Num(); Index++)
			{
				if (ContextActorArray[Index].ContextActor == ActorToRemove)
				{
					ContextActorArray.RemoveAt(Index);

					//make sure to only clear text when the actor is actually the current active actor. Also refresh text to next action, if there are any.
					if (Index == 0)
					{
						GetPlayerHUD()->ClearContextText();
						if(ContextActorArray.Num() != 0)
							GetPlayerHUD()->SetContextText(ContextActorArray[0].ContextText, ContextActorArray[0].ContextColor);
					}
				}
			}
		}
	}
}

void APiratesAdventureCharacter::DisableContextActions()
{
	RemoveActorFromContextArray(NULL, true); //flush all context actors just in case. Its not like we can update it anyway.

	bCanUseContextActions = false;
}

void APiratesAdventureCharacter::EnableContextActions()
{
	bCanUseContextActions = true;

	TArray<UPrimitiveComponent*> OverlappingComponents;
	GetCapsuleComponent()->GetOverlappingComponents(OverlappingComponents);

	if (OverlappingComponents.Num() != 0)
	{
		for (UPrimitiveComponent* Comp : OverlappingComponents)
		{
			if (Comp && Comp->OnComponentBeginOverlap.IsBound())
			{
				FHitResult HitResult;;

				HitResult = FHitResult(this, GetCapsuleComponent(), GetActorLocation(), FVector::ZeroVector);
				FOverlapInfo NewInfo = FOverlapInfo(HitResult);

				Comp->BeginComponentOverlap(NewInfo, true);
			}
		}
	}
}

//Switch to AI controller for context actions
AController* APiratesAdventureCharacter::StartContextController()
{
	if (UWorld* World = GetWorld())
	{
		if (AIControllerClass != NULL
			&& ContextController == NULL)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = Instigator;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.OverrideLevel = GetLevel();
			SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save AI controllers into a map
			ContextController = GetWorld()->SpawnActor<AController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		}

		if (ContextController != NULL)
		{
			//AxisConstraint->ReleaseConstraint();

			APlayerController* PC = World->GetFirstPlayerController();
			if (PC)
			{
				PC->bAutoManageActiveCameraTarget = false;
				PC->UnPossess();
				ContextController->Possess(this);
				PC->SetViewTargetWithBlend(this);

				DisableAxisLock();
				DisableAutoRotation();
				bIsContextAI = true;
			}
		}
	}

	return ContextController;
}

void APiratesAdventureCharacter::StopContextController()
{
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC && ContextController)
		{
			ContextController->UnPossess();
			PC->Possess(this);
			PC->SetViewTargetWithBlend(this);
			PC->bAutoManageActiveCameraTarget = true;

			EnableAxisLock();
			EnableAutoRotation();
			bIsContextAI = false;

			//Force a rerecord of the camera frustum, since it's not unlikely we have completely changed our Z position
			CameraBoom->RerecordCameraFrustum();

			//AxisConstraint->ReinstateConstraint(true);
		}
	}
}

//Check if character is on a floor or not.
bool APiratesAdventureCharacter::PlayerIsOnFloor()
{
	if (GetCharacterMovement())
	{
		return GetCharacterMovement()->CurrentFloor.HitResult.GetActor() != NULL;
	}

	return true;
}

//HUD FUNCTIONS
void APiratesAdventureCharacter::ToggleStart()
{
	//UWorld* World = GetWorld();

	//if (World)
	//{
	//	APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(World, 0)->GetHUD());
	//	if (!PlayerHUD)
	//		return;

	//	if (!UGameplayStatics::IsGamePaused(World))
	//	{
	//		UGameplayStatics::SetGamePaused(World, true);
	//		//PlayerHUD->RingMenuElementInstance->RefreshRing();
	//		PlayerHUD->RingMenuElementInstance->OpenRingMenu(ERingType::RING_ELEMENT);
	//	}
	//	else
	//	{
	//		UGameplayStatics::SetGamePaused(World, false);
	//		PlayerHUD->RingMenuElementInstance->CloseRingMenu();
	//	}
	//}
}

APA_PlayerHUD* APiratesAdventureCharacter::GetPlayerHUD()
{
	if (PlayerHUD)
	{
		return PlayerHUD;
	}
	else
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PC = World->GetFirstPlayerController();
			PlayerHUD = Cast<APA_PlayerHUD>(PC->GetHUD());
			if (PlayerHUD) return PlayerHUD;
		}
	}

	return NULL;
}

void APiratesAdventureCharacter::StartMiscMenu()
{
	UWorld* World = GetWorld();

	if (World 
		&& !bElementMenuIsActive 
		&& GetPlayerHUD() != NULL)
	{
		bMiscMenuIsActive = true;
		GetPlayerHUD()->SwitchEquipVisualization(ERingMenuType::RMT_Misc);
	}
}

void APiratesAdventureCharacter::StopMiscMenu()
{
	UWorld* World = GetWorld();

	if (World 
		&& !bElementMenuIsActive
		&& GetPlayerHUD() != NULL)
	{
		bMiscMenuIsActive = false;
		GetPlayerHUD()->ResetEquipVisualization();
	}
}

void APiratesAdventureCharacter::StartElementMenu()
{
	UWorld* World = GetWorld();

	if (World 
		&& !bMiscMenuIsActive
		&& GetPlayerHUD() != NULL)
	{
		bElementMenuIsActive = true;
		GetPlayerHUD()->SwitchEquipVisualization(ERingMenuType::RMT_Element);
	}
}

void APiratesAdventureCharacter::StopElementMenu()
{
	UWorld* World = GetWorld();

	if (World 
		&& !bMiscMenuIsActive
		&& GetPlayerHUD() != NULL)
	{
		bElementMenuIsActive = false;
		GetPlayerHUD()->ResetEquipVisualization();
	}
}

void APiratesAdventureCharacter::ProcessEquipOption(EPlayerWeaponType Weapon, EPlayerToolType Tool, FName Keybinding)
{
	bool bButtonWasUsed = false;

	if (bElementMenuIsActive
		&& CanElementBeUsed(Weapon)
		&& EquippedWeapon)
	{
		CurrentElement = Weapon;
		EquippedWeapon->SetWeaponElement(CurrentElement);
		bButtonWasUsed = true;

		if (EquippedTool)
		{
			EquippedTool->SetToolElement(CurrentElement);
			if (EquippedTool->bToolIsActive)
			{
				EquippedTool->OnSwitchElement();
			}
		}
	}
	else if (bMiscMenuIsActive)
	{
		if (GetPlayerHUD() != NULL)
		{
			//for now, Equip2 and Equip3 are still open. Equip 1/4/5 are closest to the upperleft edge of the screen, so get priority for the misc menu

			if (Keybinding == TEXT("Equip1"))
			{
				//this will become investigate
			}

			if (Keybinding == TEXT("Equip4"))
			{
				//this will become open items
				GetPlayerHUD()->OpenIngameItems();
			}

			if (Keybinding == TEXT("Equip5"))
			{
				//this will become open map
				GetPlayerHUD()->OpenIngameMap();
			}
		}
	}
	else if (CanToolBeUsed(Tool))
	{
		SetEquippedTool(Tool);
		bButtonWasUsed = true;
	}

	if (GetPlayerHUD() != NULL
		&& bButtonWasUsed)
		GetPlayerHUD()->ProcessKeyBinding(Keybinding);
}

//---------------------------------------------------------------------------------------------------------------//
// MOVEMENT FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

void APiratesAdventureCharacter::UpdatePlayerMutators()
{
	FPlayerMovementMutator BaseMutator;
	BaseMutator.DefaultValues();

	if (MovementMutators.Num() != 0)
	{
		for (int32 Index = 0; Index < MovementMutators.Num(); Index++)
		{
			BaseMutator.GravityScale *= MovementMutators[Index].GravityScale;
			BaseMutator.MaxAcceleration *= MovementMutators[Index].MaxAcceleration;
			BaseMutator.BrakingFrictionFactor *= MovementMutators[Index].BrakingFrictionFactor;

			BaseMutator.RotationRate.Yaw *= MovementMutators[Index].RotationRate.Yaw;
			BaseMutator.RotationRate.Pitch *= MovementMutators[Index].RotationRate.Pitch;
			BaseMutator.RotationRate.Roll *= MovementMutators[Index].RotationRate.Roll;

			BaseMutator.MaxWalkSpeed *= MovementMutators[Index].MaxWalkSpeed;
			BaseMutator.MaxCrouchSpeed *= MovementMutators[Index].MaxCrouchSpeed;
			BaseMutator.JumpSingleHeight *= MovementMutators[Index].JumpSingleHeight;
			BaseMutator.JumpDoubleHeight *= MovementMutators[Index].JumpDoubleHeight;
			BaseMutator.CustomTimeDilation *= MovementMutators[Index].CustomTimeDilation;
		}
	}

	GetCharacterMovement()->GravityScale = BaseMutator.GravityScale;
	GetCharacterMovement()->MaxAcceleration = BaseMutator.MaxAcceleration;
	GetCharacterMovement()->BrakingFrictionFactor = BaseMutator.BrakingFrictionFactor;
	GetCharacterMovement()->RotationRate = BaseMutator.RotationRate;
	GetCharacterMovement()->MaxWalkSpeed = BaseMutator.MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = BaseMutator.MaxCrouchSpeed;
	GetCharacterMovement()->JumpZVelocity = BaseMutator.JumpSingleHeight;
	JumpDoubleHeight = BaseMutator.JumpDoubleHeight;
	CustomTimeDilation = BaseMutator.CustomTimeDilation;
}

void APiratesAdventureCharacter::SetPlayerMutator(FPlayerMovementMutator NewMutator)
{
	//first check if the mutator already exists
	if (MovementMutators.Num() != 0)
	{
		for (int32 Index = 0; Index < MovementMutators.Num(); Index++)
		{
			if (MovementMutators[Index].MutatorName == NewMutator.MutatorName)
				return; 
		}
	}

	MovementMutators.Add(NewMutator);
	UpdatePlayerMutators();
}

void APiratesAdventureCharacter::RemovePlayerMutator(FString MutatorName)
{
	if (MovementMutators.Num() != 0)
	{
		for (int32 Index = 0; Index < MovementMutators.Num(); Index++)
		{
			if (MovementMutators[Index].MutatorName == MutatorName)
			{
				MovementMutators.RemoveAt(Index);
				UpdatePlayerMutators();
				return;
			}
		}
	}
}

TArray<FString> APiratesAdventureCharacter::GetActiveMutatorNames()
{
	TArray<FString> CurrentMutators;

	for (FPlayerMovementMutator Mutator : MovementMutators)
	{
		CurrentMutators.Add(Mutator.MutatorName);
	}

	return CurrentMutators;
}

void APiratesAdventureCharacter::ForceUnCrouch()
{
	UPA_CharacterMovementComponent* PACharacterMovement = Cast<UPA_CharacterMovementComponent>(GetCharacterMovement());
	if (PACharacterMovement)
	{
		PACharacterMovement->UnDuck();
		PACharacterMovement->UnCrouch();
	}
}

void APiratesAdventureCharacter::DisableMovement()
{
	bDisableMovement = true;
	//DisableMovementCounter++;
}

void APiratesAdventureCharacter::EnableMovement(bool bForceMovement)
{
	bDisableMovement = false;

	//if (bForceMovement)
	//{
	//	bDisableMovement = false;
	//	DisableMovementCounter = 0;
	//}
	//else
	//{
	//	DisableMovementCounter--;
	//	if (DisableMovementCounter <= 0)
	//	{
	//		DisableMovementCounter = 0;
	//		bDisableMovement = false;
	//	}
	//}
}

void APiratesAdventureCharacter::EnableAxisLock()
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
}

void APiratesAdventureCharacter::DisableAxisLock()
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(false);
}

//---------------------------------------------------------------------------------------------------------------//
// INVENTORY FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

void APiratesAdventureCharacter::SetEquippedBandana(int32 Index)
{
	if (Index < 0)
	{
		EquippedBandana->SetVisibility(false);
	}
	else if (Bandanas.IsValidIndex(Index) && Bandanas[Index].EquipmentTemplate != NULL)
	{
		if (!EquippedBandana->IsVisible())
			EquippedBandana->SetVisibility(true);

		EquippedBandana->SetSkeletalMesh(Bandanas[Index].EquipmentTemplate);
	}
}

void APiratesAdventureCharacter::SetEquippedJacket(int32 Index)
{
	if (Index < 0)
	{
		EquippedJacket->SetVisibility(false);
	}
	else if (Jackets.IsValidIndex(Index) && Jackets[Index].EquipmentTemplate != NULL)
	{
		if (!EquippedJacket->IsVisible())
			EquippedJacket->SetVisibility(true);

		EquippedJacket->SetSkeletalMesh(Jackets[Index].EquipmentTemplate);
	}
}

void APiratesAdventureCharacter::SetEquippedGloves(int32 Index)
{
	if (Index < 0)
	{
		EquippedGloves->SetVisibility(false);
	}
	else if (Gloves.IsValidIndex(Index) && Gloves[Index].EquipmentTemplate != NULL)
	{
		if (!EquippedGloves->IsVisible())
			EquippedGloves->SetVisibility(true);

		EquippedGloves->SetSkeletalMesh(Gloves[Index].EquipmentTemplate);
	}
}

void APiratesAdventureCharacter::SetEquippedBoots(int32 Index)
{
	if (Index < 0)
	{
		EquippedBoots->SetVisibility(false);
	}
	else if (Boots.IsValidIndex(Index) && Boots[Index].EquipmentTemplate != NULL)
	{
		if (!EquippedBoots->IsVisible())
			EquippedBoots->SetVisibility(true);

		EquippedBoots->SetSkeletalMesh(Boots[Index].EquipmentTemplate);
	}
}

void APiratesAdventureCharacter::RegisterWeaponComponent(UPA_Weapon_Base* NewWeapon)
{
	if (NewWeapon)
	{
		EquippedWeapon = NewWeapon;
		EquippedWeapon->SetRenderCustomDepth(true);
		EquippedWeapon->SetCustomDepthStencilValue(GetMesh()->CustomDepthStencilValue);
		EquippedWeapon->SetHiddenInGame(false);
		EquippedWeapon->RegisterComponent();
	}
}

bool APiratesAdventureCharacter::CanElementBeUsed(EPlayerWeaponType ElementToCheck)
{
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index]->ItemData.Category == EItemCategory::IC_Weapon
			&& Items[Index]->ItemData.WeaponType == ElementToCheck)
		{
			return Items[Index]->bUnlocked;
		}
	}

	return false;
}

void APiratesAdventureCharacter::SetElementUnlocked(EPlayerWeaponType WeaponType, bool bNewUnlock /* = true */)
{
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index]->ItemData.Category == EItemCategory::IC_Weapon
			&& Items[Index]->ItemData.WeaponType == WeaponType
			&& Items[Index]->bUnlocked != bNewUnlock)
		{
			Items[Index]->bUnlocked = bNewUnlock;
			if (GetPlayerHUD() != NULL)
				GetPlayerHUD()->SetRingUnlocked(Items[Index]);
		}
	}
}

void APiratesAdventureCharacter::SetElementInactive(EPlayerWeaponType WeaponType, bool bNewInactive)
{
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index]->ItemData.Category == EItemCategory::IC_Weapon
			&& Items[Index]->ItemData.WeaponType == WeaponType
			&& Items[Index]->bInactive != bNewInactive)
		{
			Items[Index]->bInactive = bNewInactive;
			if (GetPlayerHUD() != NULL)
				GetPlayerHUD()->SetRingInactive(Items[Index]);
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//	TOOL EQUIPMENT FUNCTIONALITY
//---------------------------------------------------------------------------------------------------------------------------------------------------

void APiratesAdventureCharacter::RegisterToolComponent(class UPA_Tool_Base* NewTool)
{
	if (NewTool)
	{
		int32 ToolIndex = 0;
		Tools.Find(NewTool, ToolIndex);
		if (ToolIndex == -1 && NewTool->ToolType != EPlayerToolType::NONE)
		{
			Tools.Add(NewTool);

			//UE_LOG(AnyLog, Warning, TEXT("Has registered %s's tool component"), *(Tools[Tools.Num() - 1]->GetName()));
		}
	}
}

UPA_Tool_Base* APiratesAdventureCharacter::GetEquippedTool()
{
	return EquippedTool;
}

int32 APiratesAdventureCharacter::GetToolIndex(TSubclassOf<UObject> ToolClass)
{
	for (int32 Index = 0; Index < Tools.Num(); Index++)
	{
		if (Tools[Index]->GetClass() == ToolClass)
			return Index;
	}

	return -1;
}

void APiratesAdventureCharacter::ForceEquippedTool(EPlayerToolType ToolType)
{
	UWorld* World = GetWorld();

	if (World)
	{
		//TODO: Make sure the inventory part gets moved to the HUD, where all the item data is stored. 
		for (int32 ToolIndex = 0; ToolIndex < Tools.Num(); ToolIndex++)
		{
			if (Tools[ToolIndex]->ToolType == ToolType)
			{
				SetEquippedTool(ToolType);
			}
		}
	}
}

void APiratesAdventureCharacter::SetEquippedTool(EPlayerToolType ToolType)
{
	if (ToolType != EPlayerToolType::NONE)
	{
		for (int32 Index = 0; Index < Tools.Num(); Index++)
		{
			if (Tools[Index] != NULL
				&& Tools[Index]->ToolType == ToolType
				&& CanToolBeUsed(ToolType))
			{
				//make sure to detach the previous weapon and set it to invisible, if we have one
				if (GetEquippedTool())
				{
					//maybe destroy the component?
					EquippedTool->DeinitializeTool();
					EquippedTool->SetHiddenInGame(true);
				}

				//Now initialize the new tool
				EquippedTool = Tools[Index];
				EquippedTool->SetHiddenInGame(true);
				EquippedTool->InitializeTool();
				EquippedTool->bToolIsActive = false;

				return;
			}
		}
	}
}

bool APiratesAdventureCharacter::CanToolBeUsed(EPlayerToolType ToolToCheck)
{
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index]->ItemData.Category == EItemCategory::IC_Tool
			&& Items[Index]->ItemData.ToolType == ToolToCheck)
			return Items[Index]->bUnlocked && !Items[Index]->bInactive;
	}

	return false;
}

void APiratesAdventureCharacter::SetToolUnlocked(EPlayerToolType ToolType, bool bNewUnlock)
{
	for (int32 Index = 0; Index < Tools.Num(); Index++)
	{
		if (Items[Index]->ItemData.Category == EItemCategory::IC_Tool
			&& Items[Index]->ItemData.ToolType == ToolType
			&& Items[Index]->bUnlocked != bNewUnlock)
		{
			Items[Index]->bUnlocked = bNewUnlock;
			if (GetPlayerHUD() != NULL)
				GetPlayerHUD()->SetRingUnlocked(Items[Index]);
		}
	}
}

void APiratesAdventureCharacter::SetToolInactive(EPlayerToolType ToolType, bool bNewInactive)
{
	for (int32 Index = 0; Index < Tools.Num(); Index++)
	{
		if (Items[Index]->ItemData.Category == EItemCategory::IC_Tool
			&& Items[Index]->ItemData.ToolType == ToolType
			&& Items[Index]->bInactive != bNewInactive)
		{
			Items[Index]->bInactive = bNewInactive;
			if (GetPlayerHUD() != NULL)
				GetPlayerHUD()->SetRingInactive(Items[Index]);
		}
	}
}

void APiratesAdventureCharacter::AddInventoryItem(class UPA_Asset_Item* NewItem)
{
	if (NewItem)
	{
		int32 Index;
		Items.Find(NewItem, Index);
		if (Index == INDEX_NONE)
		{
			Items.Add(NewItem);
		}
	}
}

bool APiratesAdventureCharacter::HasItemInInventory(class UPA_Asset_Item* ItemToCheck)
{
	if (ItemToCheck)
	{
		for (auto& Item : Items)
		{
			const UPA_Asset_Item* CheckedItem = CastChecked<UPA_Asset_Item>(Item);
			if (CheckedItem && CheckedItem == ItemToCheck)
				return true;
		}
	}

	return false;
}

void APiratesAdventureCharacter::SetInventoryItemActive(class UPA_Asset_Item* ItemToCheck, bool bNewActive)
{
	if (ItemToCheck)
	{
		for (UPA_Asset_Item* Item : Items)
		{
			if (Item == ItemToCheck)
				Item->bActive = bNewActive;
		}
	}
}

void APiratesAdventureCharacter::SetInventoryItemInactive(class UPA_Asset_Item* ItemToCheck, bool bNewInactive)
{
	if (ItemToCheck)
	{
		for (UPA_Asset_Item* Item : Items)
		{
			if (Item == ItemToCheck)
				Item->bInactive = bNewInactive;
		}
	}
}

void APiratesAdventureCharacter::SetInventoryItemRollover(class UPA_Asset_Item* ItemToCheck, bool bNewRollover)
{
	if (ItemToCheck)
	{
		for (UPA_Asset_Item* Item : Items)
		{
			if (Item == ItemToCheck)
				Item->bRollover = bNewRollover;
		}
	}
}

void APiratesAdventureCharacter::SetInventoryItemUnlocked(class UPA_Asset_Item* ItemToCheck, bool bNewUnlock)
{
	if (ItemToCheck)
	{
		for (UPA_Asset_Item* Item : Items)
		{
			if (Item == ItemToCheck)
				Item->bUnlocked = bNewUnlock;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------//
// DAMAGE FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

void APiratesAdventureCharacter::OnStartProcessingDamageAnimation(FDamageAnimation DamageAnimationData)
{
	UPA_PlayerAnimInstance* PlayerAnimInstance = Cast<UPA_PlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (PlayerAnimInstance && GetRegisteredState(EPlayerStateType::GPS_GameDamage) != NULL)
	{
		PlayerAnimInstance->StartReceivingDamage(DamageAnimationData);
		SetNewPlayerState(EPlayerStateType::GPS_GameDamage);

		FVector DamageDirection;
		if (DamageAnimationData.AirImpulse.Size() != 0.f)
		{
			DamageDirection = DamageAnimationData.AirImpulse;
		}
		else
		{
			DamageDirection = DamageAnimationData.GroundImpulse;
		}
		DamageDirection.Normalize();

		if (OnGetInterruptedByDamage.IsBound())
			OnGetInterruptedByDamage.Broadcast(DamageDirection);
	}
}

void APiratesAdventureCharacter::OnStopProcessingDamageAnimation()
{
	SetNewPlayerState(EPlayerStateType::GPS_GameNormal);

	ReceiveOnStopProcessingDamageAnimation();
}

void APiratesAdventureCharacter::ProcessRecovery(float RecoveryAmount, bool bFullRecover /* = false */)
{
	if (!bFullRecover && RecoveryAmount == 0.f) return;

	if (RemainingHealth != CurrentMaxHealth)
	{
		OldHealth = RemainingHealth;

		if (bFullRecover)
		{
			RemainingHealth = CurrentMaxHealth;
			RemainingHealthForUI = CurrentMaxHealth;
		}
		else
		{
			RemainingHealth = FMath::Clamp(RemainingHealth + RecoveryAmount, -1.f, CurrentMaxHealth);
			RemainingHealthForUI = FMath::Clamp(RemainingHealthForUI + RecoveryAmount, -1.f, CurrentMaxHealth);
			//if (RemainingHealth > CurrentMaxHealth)
			//	RemainingHealth = CurrentMaxHealth;
		}

		if (GetPlayerHUD() != NULL)
			GetPlayerHUD()->ProcessRecovery();

		//ReceiveProcessRecovery();
	}
}

void APiratesAdventureCharacter::ProcessDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f) return; //don't allow for negative damage

	OldHealth = RemainingHealth;

	RemainingHealth -= DamageAmount;
	RemainingHealthForUI = FMath::Clamp(RemainingHealthForUI - DamageAmount, -1.f, CurrentMaxHealth);
	if (RemainingHealth <= -1.f)
	{
		RemainingHealth = -1.f;
		OnDead();
	}

	if (GetPlayerHUD() != NULL)
		GetPlayerHUD()->ProcessDamage();

	ReceiveProcessDamage();
}

void APiratesAdventureCharacter::UpgradeHealth()
{
	bool bNeedsRecovery = false;
	//TODO: this doesn't work, because we get weird values in the interface. 
	//We need to find a way to first recover the current health, and then increase the max health with animation, while also making sure the current health is updated. 
	if (RemainingHealth < CurrentMaxHealth)
	{
		ProcessRecovery(0.f, true);
		bNeedsRecovery = true;
	}

	CurrentMaxHealth = FMath::Clamp(CurrentMaxHealth + 1.f, 0.f, MaxHealthCap);

	if (GetPlayerHUD() != NULL)
		GetPlayerHUD()->ProcessHealthUpgrade(bNeedsRecovery);

	//ReceiveUpgradeHealth(bNeedsRecovery);
}

void APiratesAdventureCharacter::MaximizeHealthValuesInternal()
{
	RemainingHealth = CurrentMaxHealth;
	OldHealth = CurrentMaxHealth;
	RemainingHealthForUI = CurrentMaxHealth;
}

void APiratesAdventureCharacter::OnDead()
{
	//TODO: Implement this properly
	ReceiveOnDead();
}


//---------------------------------------------------------------------------------------------------------------//
// MISC FUNCTIONS
//---------------------------------------------------------------------------------------------------------------//

//moved to interface
void APiratesAdventureCharacter::UpdateScalarMaterialParameterSet(UMaterialParameterCollection* Collection, FName ParameterName, float Value)
{
	if (Collection)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, ParameterName, Value);
	}
}

void APiratesAdventureCharacter::SetNewStencilValueForAll(int32 NewStencilValue)
{
	//First set the clothing and the main character
	GetMesh()->SetCustomDepthStencilValue(NewStencilValue);

	if (EquippedBandana)
		EquippedBandana->SetCustomDepthStencilValue(NewStencilValue);

	if (EquippedBoots)
		EquippedBoots->SetCustomDepthStencilValue(NewStencilValue);

	if (EquippedGloves)
		EquippedGloves->SetCustomDepthStencilValue(NewStencilValue);

	if (EquippedJacket)
		EquippedJacket->SetCustomDepthStencilValue(NewStencilValue);

	//Then deal with the sword
	if (EquippedWeapon)
	{
		EquippedWeapon->SetCustomDepthStencilValue(NewStencilValue);
	}

	//and lastly deal with the tools
	for (int32 ToolIndex = 0; ToolIndex < Tools.Num(); ToolIndex++)
	{
		if (Tools[ToolIndex] != NULL)
			Tools[ToolIndex]->SetCustomDepthStencilValue(NewStencilValue);
	}
}

//TEMP Main Menu
void APiratesAdventureCharacter::OpenMainMenu()
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC) return;
}

void APiratesAdventureCharacter::OpenIngameMenu()
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC) return;
}

void APiratesAdventureCharacter::CloseMainMenu()
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC) return;
}

void APiratesAdventureCharacter::CloseIngameMenu()
{
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC) return;
}

////---------------------------------------------------------------------------------------------------------------//
//// BOOK UI FUNCTIONS
////---------------------------------------------------------------------------------------------------------------//
//
////TODO: Something here causes fatal errors. Try checking whether fatal errors happen only on the editor or during (standalone) play as well. If it's just the editor, 
////we may be able to deal with it. Otherwise, let's change the swapping visibility back to just loading the levels with callback.
//
////WE GET CRASHES EVEN DURING GAMEPLAY. LOOK INTO THIS ASAP
//
//void APiratesAdventureCharacter::OpenMainMenu()
//{
//	CurrentLoadType = ELoadBookUIType::BOOK_OpenMainMenu;
//	CurrentStreamIndex = 0;
//
//	LoadBookUI();
//}
//
//void APiratesAdventureCharacter::CloseMainMenu()
//{
//	CurrentLoadType = ELoadBookUIType::BOOK_CloseMainMenu;
//	CurrentStreamIndex = 0;
//
//	if (CloseMainMenuDelegate.IsBound())
//		CloseMainMenuDelegate.Broadcast();
//}
//
//void APiratesAdventureCharacter::OpenIngameMenu()
//{
//	CurrentLoadType = ELoadBookUIType::BOOK_OpenIngameMenu;
//	CurrentStreamIndex = 0;
//
//	LoadBookUI();
//}
//
//void APiratesAdventureCharacter::CloseIngameMenu()
//{
//	CurrentLoadType = ELoadBookUIType::BOOK_CloseIngameMenu;
//	CurrentStreamIndex = 0;
//
//	if (CloseIngameMenuDelegate.IsBound())
//		CloseIngameMenuDelegate.Broadcast();
//}
//
////THIS IS MAIN MENU MAP LOADING WITH JUST TURNING ON/OFF THEIR VISIBILITY. 
////RIGHT NOW THIS IS SET TO STREAMING RATHER THAN TURNING VISIBLE/INVISIBLE
//void APiratesAdventureCharacter::LoadBookUI()
//{
//	if (GetWorld() 
//		&& MainMenuMap != "")
//	{
//		FName MapPackageName;
//		bool bHasMapInLevels = false;
//		TArray<FName> CurrentStreamedMaps;
//
//		//First check whether our main menu is the persistent level. We don't want to do anything if that's the case.
//		MapPackageName = FPackageName::GetShortFName(GetLevel()->GetName());
//		if (MapPackageName == MainMenuMap)
//			return;
//
//		//Then we see whether we even have a main menu in the Levels list
//		const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->StreamingLevels;
//		for (const ULevelStreaming* EachLevelStreaming : StreamedLevels)
//		{
//			if (!EachLevelStreaming)
//				continue;
//
//			FString Prefix = GetWorld()->StreamingLevelsPrefix;
//			FString FinalPackageName = FPackageName::GetShortFName(EachLevelStreaming->GetWorldAssetPackageName()).ToString();
//			FinalPackageName.RemoveFromStart(Prefix);
//			MapPackageName = *FinalPackageName;
//
//			if (EachLevelStreaming->IsLevelVisible())
//				CurrentStreamedMaps.Add(MapPackageName);
//
//			if (MapPackageName == MainMenuMap)
//				bHasMapInLevels = true;
//		}
//
//		LastKnownMaps = CurrentStreamedMaps;
//
//		//THIS WORKS WITH SETTING LEVELS TO VISIBLE AND INVISIBLE. LIKELY CAUSES CRASHES
//		if (bHasMapInLevels) //first test without the check
//		{
//			if (!GetWorld()->GetTimerManager().IsTimerActive(LoadBookUIHandle))
//			{
//				SideViewCaptureComponent->Activate();
//				//UGameplayStatics::GetStreamingLevel(GetWorld(), MainMenuMap)->bShouldBeVisible = true; //if this is called here, the player camera switches too quickly
//				GetWorld()->GetTimerManager().SetTimer(LoadBookUIHandle, this, &APiratesAdventureCharacter::DeactivateSideViewCaptureComponent, 0.1f, false);
//			}
//		}
//
//		//THIS WORKS WITH SETTING LEVELS TO LOADING AND UNLOADING. HAS VARIOUS VISUAL ISSUES
//		//if (bHasMapInLevels)
//		//{
//		//	FLatentActionInfo ActionInfo;
//		//	ActionInfo.CallbackTarget = this;
//		//	ActionInfo.ExecutionFunction = "StreamForOpeningBookUI";
//		//	ActionInfo.UUID = 1;
//		//	ActionInfo.Linkage = 0;
//		//
//		//	SideViewCaptureComponent->Activate();
//		//	bIsFading = false; //just in case, so we can reset it if it ever turns true and has to be set to false afterwards.
//		//	bIsStreaming = true;
//
//		//	APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
//		//	if (HUD && HUD->StatusIndicatorInstance)
//		//	{
//		//		bIsFading = true;
//		//		HUD->StatusIndicatorInstance->StartOpenFadeIn();
//		//	}
//		//	
//		//	UGameplayStatics::LoadStreamLevel(GetWorld(), MainMenuMap, true, false, ActionInfo);
//		//}
//	}
//}
//
//bool APiratesAdventureCharacter::CanProceedWithMainMenuAnimation()
//{
//	return !bIsFading && !bIsStreaming;
//}
//
//void APiratesAdventureCharacter::DeactivateSideViewCaptureComponent()
//{
//	//if we don't add this additional slowdown in the opening of the main menu, we will end up with the capture component recording the invisible screen. 
//	//Deactivate probably works on a delay.
//	//TODO: investigate this on slower computers! This could be problematic!
//
//	SideViewCaptureComponent->Deactivate();
//
//	GetCharacterMovement()->SetMovementMode(MOVE_None);
//
//	APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
//	if (HUD && HUD->StatusIndicatorInstance)
//	{
//		HUD->StatusIndicatorInstance->StartOpenFadeIn();
//	}
//
//	//TODO:For speedrunners, use the timermanager option only when set to instant (if we can even use that that is. We should probably just use a singular scenecapture then);
//	//GetWorld()->GetTimerManager().SetTimer(LoadBookUIHandle, this, &APiratesAdventureCharacter::FinalizeLoadBookUI, 0.01f, false);
//}
//
//void APiratesAdventureCharacter::StreamForOpeningBookUI()
//{
//	FLatentActionInfo ActionInfo;
//	ActionInfo.CallbackTarget = this;
//	ActionInfo.ExecutionFunction = "StreamForOpeningBookUI";
//	ActionInfo.UUID = 1;
//	ActionInfo.Linkage = 0;
//
//	if (SideViewCaptureComponent->IsActive())
//	{
//		SideViewCaptureComponent->Deactivate();
//
//		GetCharacterMovement()->SetMovementMode(MOVE_None);
//	
//		//GetCharacterMovement()->GravityScale = 0.f;
//		//GetCharacterMovement()->Velocity = FVector::ZeroVector;
//		//CustomTimeDilation = 0.f;	
//	}
//	
//	UWorld* World = GetWorld();
//	
//	if (World)
//	{
//		if (LastKnownMaps.IsValidIndex(CurrentStreamIndex))
//		{
//			UGameplayStatics::UnloadStreamLevel(World, LastKnownMaps[CurrentStreamIndex], ActionInfo);
//			CurrentStreamIndex++;
//		}
//		else
//		{
//			bIsStreaming = false;
//			//Only move forward with the animation if both the bIsStreaming and bIsFading bools are false. Otherwise, wait for the other.
//
//			//TODO: since we may actually get problems with the beginplay here, considering using an OpenMainMenu signature in the MM levelscript actor if the 
//			//CanProceedWithMainMenuAnimation() returns false at this point. We don't want the MM map to start when streaming is not done yet. 
//			APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
//			if (HUD
//				&& HUD->StatusIndicatorInstance)
//			{
//				if (CanProceedWithMainMenuAnimation())
//				{
//					HUD->StatusIndicatorInstance->StartOpenFadeOut();
//				}
//			}
//		}
//	}
//}
//
//void APiratesAdventureCharacter::FinalizeLoadBookUI()
//{
//	FLatentActionInfo ActionInfo;
//	ActionInfo.CallbackTarget = this;
//	ActionInfo.ExecutionFunction = "LoadBookUICallback";
//	ActionInfo.UUID = 1;
//	ActionInfo.Linkage = 0;	
//
//	//first see if the main menu has actually been loaded. 
//	if (UGameplayStatics::GetStreamingLevel(GetWorld(), MainMenuMap)->IsLevelLoaded())
//	{
//		UGameplayStatics::GetStreamingLevel(GetWorld(), MainMenuMap)->bShouldBeVisible = true;
//
//		for (const FName MapName : LastKnownMaps)
//		{
//			if (MapName == NAME_None)
//				continue;
//
//			UGameplayStatics::GetStreamingLevel(GetWorld(), MapName)->bShouldBeVisible = false;
//		}
//
//		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//		if (PC)
//		{
//			APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());
//			if (HUD)
//			{
//				HUD->OpenIngameMenu(); //TODO; MAKE THIS CLEANER. PERHAPS MOVE THE ENTIRE SEQUENCE INTO THE HUD WHERE IT BELONGS.
//			}
//		}
//	}
//	else
//	{
//		UGameplayStatics::LoadStreamLevel(GetWorld(), MainMenuMap, true, true, ActionInfo);
//	}
//}
//
//void APiratesAdventureCharacter::LoadBookUICallback()
//{
//	GetCharacterMovement()->GravityScale = 0.f;
//	GetCharacterMovement()->Velocity = FVector::ZeroVector;
//
//	for (const FName MapName : LastKnownMaps)
//	{
//		if (MapName == NAME_None)
//			continue;
//
//		UGameplayStatics::GetStreamingLevel(GetWorld(), MapName)->bShouldBeVisible = false;
//	}
//}
//
//void APiratesAdventureCharacter::UnloadBookUI()
//{
//	//VISIBLE/INVISIBLE METHOD
//	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
//	if (!PC) return;
//
//	APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());
//	if (HUD && HUD->StatusIndicatorInstance)
//	{
//		for (const FName MapName : LastKnownMaps)
//		{
//			if (MapName == NAME_None)
//				continue;
//
//			UGameplayStatics::GetStreamingLevel(GetWorld(), MapName)->bShouldBeVisible = true;
//		}
//
//		if (UGameplayStatics::GetStreamingLevel(GetWorld(), MainMenuMap)->IsLevelVisible())
//			UGameplayStatics::GetStreamingLevel(GetWorld(), MainMenuMap)->bShouldBeVisible = false;
//
//		LastKnownMaps.Empty();
//
//		//HUD->StatusIndicatorInstance->StartCloseFadeIn();
//		PC->SetViewTargetWithBlend(this);
//	}
//
//	//LOAD/UNLOAD METHOD
//	//if (LastKnownMaps.IsValidIndex(CurrentStreamIndex))
//	//{
//	//	FLatentActionInfo ActionInfo;
//	//	ActionInfo.CallbackTarget = this;
//	//	ActionInfo.ExecutionFunction = "StreamForClosingBookUI";
//	//	ActionInfo.UUID = 1;
//	//	ActionInfo.Linkage = 0;		
//
//	//	bIsFading = false; //just in case, so we can reset it if it ever turns true and has to be set to false afterwards.
//	//	bIsStreaming = true;
//
//	//	APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
//	//	if (HUD && HUD->StatusIndicatorInstance)
//	//	{
//	//		bIsFading = true;
//	//		HUD->StatusIndicatorInstance->StartCloseFadeIn();
//	//	}
//
//	//	UGameplayStatics::LoadStreamLevel(GetWorld(), LastKnownMaps[CurrentStreamIndex], true, false, ActionInfo);
//	//	CurrentStreamIndex++; //since this time, we load/unload in reverse instead of starting with the main menu
//	//}
//}
//
//void APiratesAdventureCharacter::StreamForClosingBookUI()
//{
//	FLatentActionInfo ActionInfo;
//
//	UWorld* World = GetWorld();
//
//	if (World)
//	{
//		if (LastKnownMaps.IsValidIndex(CurrentStreamIndex))
//		{
//			ActionInfo.CallbackTarget = this;
//			ActionInfo.ExecutionFunction = "StreamForClosingBookUI";
//			ActionInfo.UUID = 1;
//			ActionInfo.Linkage = 0;
//
//			UGameplayStatics::LoadStreamLevel(World, LastKnownMaps[CurrentStreamIndex], true, true, ActionInfo);
//			CurrentStreamIndex++;
//		}
//		else// if(UGameplayStatics::GetStreamingLevel(GetWorld(), MainMenuMap)->IsLevelLoaded())
//		{
//			UE_LOG(AnyLog, Log, TEXT("TRYING TO UNLOAD MAIN MENU"));
//			UGameplayStatics::UnloadStreamLevel(World, MainMenuMap, ActionInfo);
//
//			bIsStreaming = false;
//			//Only move forward with the animation if both the bIsStreaming and bIsFading bools are false. Otherwise, wait for the other.
//			APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
//			if (HUD
//				&& HUD->StatusIndicatorInstance)
//			{
//				if (CanProceedWithMainMenuAnimation())
//				{
//					HUD->StatusIndicatorInstance->StartOpenFadeOut();
//				}
//			}
//			else
//			{
//				FinalizeUnloadBookUI();
//			}
//		}
//	}
//}
//
//void APiratesAdventureCharacter::FinalizeUnloadBookUI()
//{
//	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
//
//	//APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
//	//if (PC)
//	//	PC->SetViewTargetWithBlend(this);
//}