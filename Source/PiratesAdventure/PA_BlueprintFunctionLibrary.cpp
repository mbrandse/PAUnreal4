// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_BlueprintFunctionLibrary.h"
#include "PiratesAdventure.h"
#include "Blueprint/WidgetLayoutLibrary.h"
//#include "PA_LevelScriptActor_MainMenu.h"
//#include "InterpolateVelocityToAction.h"
//#include "InterpolateMovementToLocation.h"
#include "InterpolatePlayerCamera.h"
#include "PA_Weapon_Base.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Animation/AnimClassInterface.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimNode_AssetPlayerBase.h"
#include "Animation/AnimMontage.h"
#include "EngineUtils.h"
#include "PhysicsEngine/BodySetup.h"
#include "Dialogue.h"
#include "DDSLoader.h"
#include "PA_HudElements_DialogueBase.h"
#include "PA_HudElements_ItemGet.h"
#include "Engine/Texture.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "CineCameraComponent.h"
//AI related includes
#include "NavMesh/RecastNavMesh.h"
#include "NavigationPath.h"
#include "AITypes.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

bool UPA_BlueprintFunctionLibrary::ApplyRadialDamageWithFalloffAllActors(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AController* InstigatedByController, EDamagePenetration PenetrationType, ECollisionChannel DamagePreventionChannel)
{
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(ApplyRadialDamage), false, DamageCauser);

	SphereParams.AddIgnoredActors(IgnoreActors);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects), FCollisionShape::MakeSphere(DamageOuterRadius), SphereParams);
	}

	// collate into per-actor list of hit components
	TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
	for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
	{
		FOverlapResult const& Overlap = Overlaps[Idx];
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor &&
			OverlapActor->bCanBeDamaged &&
			OverlapActor != DamageCauser &&
			Overlap.Component.IsValid())
		{
			FHitResult Hit;

			FVector CompLoc = Overlap.Component.Get()->GetComponentLocation();
			FVector ObjectDistance = Origin - CompLoc;

			//First process our penetration type. If the damage penetrates, make a cheap hitresult with approximate impact data to save on raycasting
			if (PenetrationType == EDamagePenetration::DP_PenetrateAll
				|| (PenetrationType == EDamagePenetration::DP_PenetrateCoreOnly && ObjectDistance.Size() < DamageInnerRadius)
				|| (PenetrationType == EDamagePenetration::DP_PenetrateOuterOnly && ObjectDistance.Size() > DamageInnerRadius && ObjectDistance.Size() < DamageOuterRadius))
			{
				FVector const FakeHitNorm = (Origin - CompLoc).GetSafeNormal();		// normal points back toward the epicenter
				Hit = FHitResult(OverlapActor, Overlap.Component.Get(), CompLoc, FakeHitNorm);

				TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
			else if (DamagePreventionChannel == ECC_MAX || ComponentIsDamageableFrom(Overlap.Component.Get(), Origin, DamageCauser, IgnoreActors, DamagePreventionChannel, Hit))
			{
				TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
		}
	}

	bool bAppliedDamage = false;

	UE_LOG(AnyLog, Warning, TEXT("OVERLAPPING COMPONENTS IN OUR MAP IS %d"), OverlapComponentMap.Num());

	if (OverlapComponentMap.Num() > 0)
	{
		// make sure we have a good damage type
		TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());

		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = ValidDamageTypeClass;
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);

		// call damage function on each affected actors
		for (TMap<AActor*, TArray<FHitResult> >::TIterator It(OverlapComponentMap); It; ++It)
		{
			AActor* const Victim = It.Key();
			TArray<FHitResult> const& ComponentHits = It.Value();
			DmgEvent.ComponentHits = ComponentHits;

			Victim->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);

			bAppliedDamage = true;
		}
	}

	return bAppliedDamage;
}

/** @RETURN True if weapon trace from Origin hits component VictimComp.  OutHitResult will contain properties of the hit. */
bool UPA_BlueprintFunctionLibrary::ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	FCollisionQueryParams LineParams(SCENE_QUERY_STAT(ComponentIsVisibleFrom), true, IgnoredActor);
	LineParams.AddIgnoredActors(IgnoreActors);

	// Do a trace from origin to middle of box
	UWorld* const World = VictimComp->GetWorld();
	check(World);

	FVector const TraceEnd = VictimComp->Bounds.Origin;
	FVector TraceStart = Origin;
	if (Origin == TraceEnd)
	{
		// tiny nudge so LineTraceSingle doesn't early out with no hits
		TraceStart.Z += 0.01f;
	}
	bool const bHadBlockingHit = World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams);
	//::DrawDebugLine(World, TraceStart, TraceEnd, FLinearColor::Red, true);

	// If there was a blocking hit, it will be the last one
	if (bHadBlockingHit)
	{
		if (OutHitResult.Component == VictimComp)
		{
			// if blocking hit was the victim component, it is visible
			return true;
		}
		else
		{
			// if we hit something else blocking, it's not
			UE_LOG(LogDamage, Log, TEXT("Radial Damage to %s blocked by %s (%s)"), *GetNameSafe(VictimComp), *GetNameSafe(OutHitResult.GetActor()), *GetNameSafe(OutHitResult.Component.Get()));
			return false;
		}
	}

	// didn't hit anything, assume nothing blocking the damage and victim is consequently visible
	// but since we don't have a hit result to pass back, construct a simple one, modeling the damage as having hit a point at the component's center.
	FVector const FakeHitLoc = VictimComp->GetComponentLocation();
	FVector const FakeHitNorm = (Origin - FakeHitLoc).GetSafeNormal();		// normal points back toward the epicenter
	OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, FakeHitLoc, FakeHitNorm);
	return true;
}

bool UPA_BlueprintFunctionLibrary::StreamLevelByName(ACharacter* Character, FName LevelToStream, TArray<FName> &LastStreamedLevels, FName Callback, class UObject* CallbackOwner, bool UnloadOtherStreamedLevels)
{
	LastStreamedLevels.Empty();

	if (!Character->IsValidLowLevel())
		return false;


	UWorld* World = Character->GetWorld();

	if(World
		&& LevelToStream != NAME_None)
	{ 
		FName MapPackageName;
		bool bHasMapInLevels = false;
		TArray<FName> CurrentStreamedMaps;

		//First check whether the map to load is the persistent level. We don't want to do anything if that's the case.
		MapPackageName = FPackageName::GetShortFName(Character->GetLevel()->GetName());
		if (MapPackageName == LevelToStream)
			return false;

		//Then we see whether we even have the map in the Levels list
		const TArray<ULevelStreaming*>& StreamedLevels = World->GetStreamingLevels();
		for (const ULevelStreaming* EachLevelStreaming : StreamedLevels)
		{
			if (!EachLevelStreaming)
				continue;

			FString Prefix = World->StreamingLevelsPrefix;
			FString FinalPackageName = FPackageName::GetShortFName(EachLevelStreaming->GetWorldAssetPackageName()).ToString();
			FinalPackageName.RemoveFromStart(Prefix);
			MapPackageName = *FinalPackageName;

			if (EachLevelStreaming->IsLevelVisible())
				CurrentStreamedMaps.Add(MapPackageName);

			if (MapPackageName == LevelToStream)
				bHasMapInLevels = true;
		}

		LastStreamedLevels = CurrentStreamedMaps;

		FLatentActionInfo ActionInfo;

		if (Callback != NAME_None
			&& CallbackOwner->IsValidLowLevel())
		{
			ActionInfo.CallbackTarget = CallbackOwner;
			ActionInfo.ExecutionFunction = Callback;
			ActionInfo.UUID = 1;
			ActionInfo.Linkage = 0;
		}

		//first see if the requested map has already actually been loaded
		if (UGameplayStatics::GetStreamingLevel(World, LevelToStream)->IsLevelLoaded())
		{
			//UGameplayStatics::GetStreamingLevel(World, LevelToStream)->bShouldBeVisible = true;
			UGameplayStatics::GetStreamingLevel(World, LevelToStream)->SetShouldBeVisible(true);

			if (UnloadOtherStreamedLevels)
			{
				for (const FName MapName : LastStreamedLevels)
				{
					if (MapName == NAME_None)
						continue;

					UGameplayStatics::GetStreamingLevel(World, MapName)->SetShouldBeVisible(false);
				}
			}
		}
		else
		{
			UGameplayStatics::LoadStreamLevel(World, LevelToStream, true, true, ActionInfo);
		}

		return bHasMapInLevels;
	}

	return false;
}

bool UPA_BlueprintFunctionLibrary::GetLevelIsStreamed(UObject* WorldContextObject, FName LevelToCheck)
{
	if (!WorldContextObject) return false;
	UWorld* World = WorldContextObject->GetWorld();

	if (World)
		return UGameplayStatics::GetStreamingLevel(World, LevelToCheck)->IsLevelLoaded();

	return false;
}

//void UPA_BlueprintFunctionLibrary::AnimateVelocityTo(APiratesAdventureCharacter* Character, FVector TargetVelocity, bool bIgnoreX, bool bIgnoreY, bool bIgnoreZ, FRotator TargetRotation, bool bRotate, bool bEaseOut, bool bEaseIn, float OverTime, TEnumAsByte<EAnimateLatentAction::Type> MoveAction, FLatentActionInfo LatentInfo)
//{
//	if (UWorld* World = ((Character != NULL) ? Character->GetWorld() : NULL))
//	{
//		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
//		FInterpolateVelocityToAction* Action = LatentActionManager.FindExistingAction<FInterpolateVelocityToAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
//
//		const FVector InitialVelocity = (Character != NULL) ? Character->GetVelocity() : FVector::ZeroVector;
//		const FRotator InitialRotation = (Character != NULL) ? Character->GetActorRotation() : FRotator::ZeroRotator;
//
//		// If not currently running
//		if (Action == NULL)
//		{
//			if (MoveAction == EAnimateLatentAction::Move)
//			{
//				Action = new FInterpolateVelocityToAction(OverTime, LatentInfo, Character, true, bRotate, bEaseOut, bEaseIn);
//
//				Action->TargetVelocity = TargetVelocity;
//				Action->bIgnoreX = bIgnoreX;
//				Action->bIgnoreY = bIgnoreY;
//				Action->bIgnoreZ = bIgnoreZ;
//
//				Action->TargetRotation = TargetRotation;
//
//				Action->InitialVelocity = InitialVelocity;
//				Action->InitialRotation = InitialRotation;
//
//				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
//			}
//		}
//		else
//		{
//			if (MoveAction == EAnimateLatentAction::Move)
//			{
//				Action->TotalTime = OverTime;
//				Action->TimeElapsed = 0.f;
//
//				Action->TargetVelocity = TargetVelocity;
//				Action->bIgnoreX = bIgnoreX;
//				Action->bIgnoreY = bIgnoreY;
//				Action->bIgnoreZ = bIgnoreZ;
//
//				Action->TargetRotation = TargetRotation;
//
//				Action->InitialVelocity = InitialVelocity;
//				Action->InitialRotation = InitialRotation;
//			}
//			else if (MoveAction == EAnimateLatentAction::Stop)
//			{
//				Action->bInterpolating = false;
//			}
//			else if (MoveAction == EAnimateLatentAction::Return)
//			{
//				//won't have functionality for now. Maybe look into this later.
//			}
//		}
//	}
//}

//void UPA_BlueprintFunctionLibrary::AnimateMovementTo(class ACharacter* Character, FVector TargetLocation, float TargetTolerance, float TargetSpeed, bool bShouldUpdateFacing, FRotator FacingTarget, float FacingTargetDuration, TEnumAsByte<EAnimateLatentAction::Type> MoveAction, FLatentActionInfo LatentInfo)
//{
//	if (UWorld* World = ((Character != NULL) ? Character->GetWorld() : NULL))
//	{
//		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
//		FInterpolateMovementToLocation* Action = LatentActionManager.FindExistingAction<FInterpolateMovementToLocation>(LatentInfo.CallbackTarget, LatentInfo.UUID);
//
//		const float CurrentTargetSpeed = (Character != NULL) ? Character->GetCharacterMovement()->MaxWalkSpeed : 0.f;
//
//		// If not currently running
//		if (Action == NULL)
//		{
//			if (MoveAction == EAnimateLatentAction::Move)
//			{
//				Action = new FInterpolateMovementToLocation(LatentInfo, Character, TargetLocation, TargetSpeed, CurrentTargetSpeed, bShouldUpdateFacing, FacingTarget, FacingTargetDuration);
//				Action->TargetTolerance = TargetTolerance;
//				//Action->TotalTime = FVector::Dist(Character->GetActorLocation(), TargetLocation) / TargetSpeed;
//				//if (TotalTime == 0.f) TotalTime = FVector::Dist(Character->GetActorLocation(), TargetLocation) / TargetMaxMovementSpeed /*Character->GetCharacterMovement()->MaxWalkSpeed*/;
//
//				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
//			}
//		}
//		else
//		{
//			if (MoveAction == EAnimateLatentAction::Move)
//			{
//				Action->TargetLocation = TargetLocation;
//				Action->TimeElapsed = 0.f;
//
//				Action->TargetMaxMovementSpeed = TargetSpeed;
//				Action->bUpdateCharacterFacing = bShouldUpdateFacing;
//				Action->TargetFacing = FacingTarget;
//				Action->TargetFacingInterpolationDuration = FacingTargetDuration;
//			}
//			else if (MoveAction == EAnimateLatentAction::Stop)
//			{
//				Action->bInterpolating = false;
//			}
//			else if (MoveAction == EAnimateLatentAction::Return)
//			{
//				//won't have functionality for now. Maybe look into this later.
//			}
//		}
//	}
//}

void UPA_BlueprintFunctionLibrary::AnimateSpringArmTo(class AActor* WorldContextObject, class UPA_PlayerSpringArmComponent* SpringArmComponent, bool bInterpolateToPlayer, float Tolerance, float Speed, FVector TargetLocation, bool bInterpLocation, bool bLocationIsComparisonTarget, FRotator TargetRotation, bool bInterpRotation, bool bRotationIsComparisonTarget, float TargetArmLength, bool bInterpArmLength, bool bArmLengthIsComparisonTarget, FLatentActionInfo LatentInfo, float Aperture)
{
	if (SpringArmComponent == NULL)
		return;

	if (UWorld* World = ((WorldContextObject != NULL) ? WorldContextObject->GetWorld() : NULL))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		FInterpolatePlayerCamera* Action = LatentActionManager.FindExistingAction<FInterpolatePlayerCamera>(LatentInfo.CallbackTarget, LatentInfo.UUID);

		// If not currently running
		if (Action == NULL)
		{
			if (bInterpolateToPlayer)
			{
				SpringArmComponent->ResetCameraToDefault();
				SpringArmComponent->StopManualCameraTranslation();
				SpringArmComponent->ResetDesiredApertureToDefault();

				Action = new FInterpolatePlayerCamera(LatentInfo,
					SpringArmComponent,
					FVector::ZeroVector,
					SpringArmComponent->GetDesiredRotation(),
					SpringArmComponent->DesiredTargetArmLength,
					bLocationIsComparisonTarget,
					bRotationIsComparisonTarget,
					bArmLengthIsComparisonTarget);

				Action->bInterpolateLocation = bInterpLocation;
				Action->bInterpolateRotation = bInterpRotation;
				Action->bInterpolateArmLength = bInterpArmLength;
				Action->Tolerance = Tolerance;

				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
			}
			else
			{
				//TODO: do we need to make location interpolation optional too? Because that's the entire reason this latent action exists.
				FRotator NewTargetRot = bInterpRotation ? TargetRotation : SpringArmComponent->GetDesiredRotation();
				float NewTargetArmLength = bInterpArmLength ? TargetArmLength : SpringArmComponent->DesiredTargetArmLength;

				SpringArmComponent->StartManualCameraTranslation(TargetLocation, NewTargetRot, NewTargetArmLength, false, Speed, bInterpRotation, false);
				SpringArmComponent->SetDesiredAperture(Aperture);

				Action = new FInterpolatePlayerCamera(LatentInfo,
					SpringArmComponent,
					TargetLocation,
					NewTargetRot,
					NewTargetArmLength,
					bLocationIsComparisonTarget,
					bRotationIsComparisonTarget,
					bArmLengthIsComparisonTarget);

				Action->bInterpolateLocation = bInterpLocation;
				Action->bInterpolateRotation = bInterpRotation;
				Action->bInterpolateArmLength = bInterpArmLength;
				Action->Tolerance = Tolerance;

				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
			}
		}
	}
}

bool UPA_BlueprintFunctionLibrary::GetAnimSequenceData(ACharacter* Character, FName StateMachineName, FName StateName, float& AnimLength, float& AnimRemainingTime, float& AnimWeight)
{
	AnimLength = 0.f;
	AnimRemainingTime = 0.f;
	AnimWeight = 0.f;

	TArray<FAnimNode_StateMachine> StateMachines;
	UObject* AnimInstanceObject = Character->GetMesh()->GetAnimInstance();

	IAnimClassInterface* AnimBlueprintClass = IAnimClassInterface::GetFromClass(Character->GetMesh()->GetAnimInstance()->GetClass());

	if (AnimBlueprintClass)
	{
		const TArray<UStructProperty*>& AnimNodeProperties = AnimBlueprintClass->GetAnimNodeProperties();
		for (int32 MachineIndex = 0; MachineIndex < AnimNodeProperties.Num(); MachineIndex++)
		{
			UStructProperty* Property = AnimNodeProperties[AnimNodeProperties.Num() - 1 - MachineIndex];
			if (Property && Property->Struct == FAnimNode_StateMachine::StaticStruct())
			{
				FAnimNode_StateMachine* StateMachine = Property->ContainerPtrToValuePtr<FAnimNode_StateMachine>(AnimInstanceObject);
				if (StateMachine)
				{
					if (const FBakedAnimationStateMachine* MachineDescription = Character->GetMesh()->GetAnimInstance()->GetMachineDescription(AnimBlueprintClass, StateMachine))
					{
						if (MachineDescription->MachineName == StateMachineName)
						{
							const TArray<UStructProperty*>& MachineNodeProperties = AnimBlueprintClass->GetAnimNodeProperties();

							for (int32 StateIndex = 0; StateIndex < MachineDescription->States.Num(); StateIndex++)
							{
								const FBakedAnimationState& State = MachineDescription->States[StateIndex];

								if (State.StateName == StateName)
								{
									for (int32 PlayerIndex = 0; PlayerIndex < State.PlayerNodeIndices.Num(); PlayerIndex++)
									{
										checkSlow(State.PlayerNodeIndices[PlayerIndex] < MachineNodeProperties.Num());
										UStructProperty* AssetPlayerProperty = MachineNodeProperties[MachineNodeProperties.Num() - 1 - State.PlayerNodeIndices[PlayerIndex]];
										if (AssetPlayerProperty && AssetPlayerProperty->Struct->IsChildOf(FAnimNode_AssetPlayerBase::StaticStruct()))
										{
											FAnimNode_AssetPlayerBase* AssetPlayer = AssetPlayerProperty->ContainerPtrToValuePtr<FAnimNode_AssetPlayerBase>(AnimInstanceObject);
											if (AssetPlayer)
											{
												AnimLength = AssetPlayer->GetAnimAsset()->GetMaxCurrentTime();
												AnimRemainingTime = AssetPlayer->GetAnimAsset()->GetMaxCurrentTime() - AssetPlayer->GetAccumulatedTime();
												AnimWeight = AssetPlayer->GetCachedBlendWeight();

												return true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

float UPA_BlueprintFunctionLibrary::GetAnimationLength(UAnimSequenceBase* Animation)
{
	if (Animation)
	{
		return Animation->GetPlayLength() / Animation->RateScale;
	}

	return 0.f;
}

bool UPA_BlueprintFunctionLibrary::CanContinueCombo(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
		return false;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		return MeleeInterface->CanContinueComboAttack();
	}

	return false;
}

int32 UPA_BlueprintFunctionLibrary::GetComboCount(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
		return INDEX_NONE;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		return MeleeInterface->GetComboCount();
	}

	return INDEX_NONE;
}

bool UPA_BlueprintFunctionLibrary::ClearNormalAttack(UAnimInstance* AnimInstance, bool bCanReset)
{
	if (!AnimInstance)
		return false;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface && bCanReset)
	{
		MeleeInterface->ResetNormalAttack();
		return true;
	}

	return false;
}

bool UPA_BlueprintFunctionLibrary::ClearChargeAttack(UAnimInstance* AnimInstance, bool bCanReset)
{
	if (!AnimInstance)
		return false;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface && bCanReset)
	{
		MeleeInterface->ResetChargeAttack();
		return true;
	}

	return false;
}

bool UPA_BlueprintFunctionLibrary::CanContinueCharge(UAnimInstance* AnimInstance)
{
	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		return MeleeInterface->CanContinueChargeAttack();
	}

	return false;
}

bool UPA_BlueprintFunctionLibrary::IsAttackPlaying(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
		return false;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		return MeleeInterface->IsAttackPlaying();
	}

	return false;
}

float UPA_BlueprintFunctionLibrary::GetAttackRelevantTimeRemaining(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
		return 0.f;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		return MeleeInterface->GetAttackRelevantTimeRemaining();
	}

	return 0.f;
}

float UPA_BlueprintFunctionLibrary::GetAttackRelevantTimeRemainingRatio(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
		return 0.f;

	IPA_Interface_MeleeAttack* MeleeInterface = Cast<IPA_Interface_MeleeAttack>(AnimInstance->GetSkelMeshComponent()->GetOwner());

	if (MeleeInterface)
	{
		return MeleeInterface->GetAttackRelevantTimeRemainingRatio();
	}

	return 0.f;
}

//Environmental Calculations
float UPA_BlueprintFunctionLibrary::GetLightingAmountAtLocation(AActor* Actor, FVector Location, bool bDrawDebugLines)
{
	float Result = 0.f;

	if (Actor != NULL)
	{
		for (TActorIterator<APointLight> ActorItr(Actor->GetWorld()); ActorItr; ++ActorItr)
		{
			APointLight* PointLight = *ActorItr;
			if (PointLight)
			{
				UPointLightComponent* PointLightComp = PointLight->PointLightComponent;
				if (PointLightComp)
				{
					FVector Start = PointLight->GetActorLocation();
					float LightDistance = FVector::Dist(Location, Start);
					float LightRadius = PointLightComp->AttenuationRadius;
					float LightStrength = PointLightComp->Intensity;

					FCollisionQueryParams Params;
					Params.AddIgnoredActor(PointLight);
					Params.AddIgnoredActor(Actor);
					Params.bTraceComplex = true;

					//if the distance is too big, skip to the next iteration so we can save on traces.
					if (LightDistance > LightRadius)
						continue;

					FHitResult Hit;

					//make sure our light doesn't get blocked by anything. 
					bool bHit = Actor->GetWorld()->LineTraceSingleByChannel(Hit, Start, Location, ECC_Visibility, Params);

					if (!bHit)
					{
						//Make sure we 0 the intensity value the moment we are outside of the light's attenuation range, but with a smooth decrease to 0. 
						float LightStrengthAtAttenuation = FMath::Clamp(1.f - FMath::Square(LightDistance / LightRadius), 0.f, 1.f);

						//Calculate the light strength modifier based on the light strength itself and the distance of the player from the source. 
						float LightStrengthAtDistance = FMath::Clamp(LightStrength / FMath::Square(LightDistance), 0.f, 1.f);

						Result += LightStrength * (LightStrengthAtDistance * LightStrengthAtAttenuation);

						if (bDrawDebugLines)
							DrawDebugLine(Actor->GetWorld(), Start, Location, FColor::Green, false, 1.f);
					}
					else if (bDrawDebugLines)
					{
						DrawDebugLine(Actor->GetWorld(), Start, Hit.ImpactPoint, FColor::Green, false, 1.f);
						DrawDebugPoint(Actor->GetWorld(), Hit.ImpactPoint, 20.f, FColor::Green, false, 1.f);
						DrawDebugLine(Actor->GetWorld(), Hit.ImpactPoint, Location, FColor::Red, false, 1.f);
					}
				}
			}
		}
	}

	//UE_LOG(AnyLog, Log, TEXT("Brightness of Actor %f"), Result);

	return Result;
}

//Post processing
void UPA_BlueprintFunctionLibrary::SetBloomIntensity(ACharacter* Character, float NewBloom, bool bForceOverride)
{
	//TODO: Like Chromatic Aberration, include an option to search for specific PP volumes.
	if (Character)
	{
		//for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		//{
		//	APostProcessVolume* PPVolume = *ActorItr;
		//	if (PPVolume->bUnbound)
		//	{
		//		PPVolume->Settings.bOverride_BloomIntensity = bForceOverride;
		//		PPVolume->Settings.BloomIntensity = NewBloom;
		//	}
		//}

		APostProcessVolume* HighestPrioVolume = NULL;
		float HighestPrio = -1.f;

		for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		{
			APostProcessVolume* PPVolume = *ActorItr;
			if (PPVolume->bUnbound
				&& PPVolume->Priority > HighestPrio)
			{
				HighestPrio = PPVolume->Priority;
				HighestPrioVolume = PPVolume;
			}
		}

		if (HighestPrioVolume != NULL)
		{
			HighestPrioVolume->Settings.bOverride_BloomIntensity = bForceOverride;
			HighestPrioVolume->Settings.BloomIntensity = NewBloom;
		}
	}
}

void UPA_BlueprintFunctionLibrary::GetDOPFocal(ACharacter* Character, float& FocalDistance, float& FocalRegion)
{
	FocalDistance = 0;
	FocalRegion = 0;

	//TODO: //TODO: Like Chromatic Aberration, include an option to search for specific PP volumes.
	if (Character)
	{
		//for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		//{
		//	APostProcessVolume* PPVolume = *ActorItr;
		//	if (PPVolume->bUnbound)
		//	{
		//		if (PPVolume->Settings.bOverride_DepthOfFieldFocalDistance) FocalDistance = PPVolume->Settings.DepthOfFieldFocalDistance;
		//		if (PPVolume->Settings.bOverride_DepthOfFieldFocalRegion) FocalRegion = PPVolume->Settings.DepthOfFieldFocalRegion;
		//	}
		//}

		APostProcessVolume* HighestPrioVolume = NULL;
		float HighestPrio = -1.f;

		for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		{
			APostProcessVolume* PPVolume = *ActorItr;
			if (PPVolume->bUnbound
				&& PPVolume->Priority > HighestPrio)
			{
				HighestPrio = PPVolume->Priority;
				HighestPrioVolume = PPVolume;
			}
		}

		if (HighestPrioVolume != NULL)
		{
			FocalDistance = HighestPrioVolume->Settings.DepthOfFieldFocalDistance;
			FocalRegion = HighestPrioVolume->Settings.DepthOfFieldFocalRegion;
		}
	}
}

void UPA_BlueprintFunctionLibrary::SetDOPFocal(ACharacter* Character, bool bSetFocalDistance, float NewFocalDistance, bool bSetFocalRegion, float NewFocalRegion, bool bForceOverride)
{
	//TODO: Like Chromatic Aberration, include an option to search for specific PP volumes.
	if (Character)
	{
		//for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		//{
		//	APostProcessVolume* PPVolume = *ActorItr;
		//	if (PPVolume->bUnbound)
		//	{
		//		if (bSetFocalDistance)
		//		{
		//			PPVolume->Settings.bOverride_DepthOfFieldFocalDistance = bForceOverride;
		//			PPVolume->Settings.DepthOfFieldFocalDistance = NewFocalDistance;
		//		}

		//		if (bSetFocalRegion)
		//		{
		//			PPVolume->Settings.bOverride_DepthOfFieldFocalRegion = bForceOverride;
		//			PPVolume->Settings.DepthOfFieldFocalRegion = NewFocalRegion;
		//		}
		//	}
		//}

		APostProcessVolume* HighestPrioVolume = NULL;
		float HighestPrio = -1.f;

		for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		{
			APostProcessVolume* PPVolume = *ActorItr;
			if (PPVolume->bUnbound
				&& PPVolume->Priority > HighestPrio)
			{
				HighestPrio = PPVolume->Priority;
				HighestPrioVolume = PPVolume;
			}
		}

		if (HighestPrioVolume != NULL)
		{
			if (bSetFocalDistance)
			{
				HighestPrioVolume->Settings.bOverride_DepthOfFieldFocalDistance = bForceOverride;
				HighestPrioVolume->Settings.DepthOfFieldFocalDistance = NewFocalDistance;
			}

			if (bSetFocalRegion)
			{
				HighestPrioVolume->Settings.bOverride_DepthOfFieldFocalRegion = bForceOverride;
				HighestPrioVolume->Settings.DepthOfFieldFocalRegion = NewFocalRegion;
			}
		}
	}
}

void UPA_BlueprintFunctionLibrary::SetChromaticAbberation(ACharacter* Character, float NewChromaticAbberation, float NewChromaticAbberationStartOffset, bool bForceOverride, float TargetPriorityVolume)
{
	if (Character)
	{
		APostProcessVolume* HighestPrioVolume = NULL;
		float HighestPrio = -1.f;

		for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		{
			APostProcessVolume* PPVolume = *ActorItr;
			if (PPVolume->bUnbound)
			{
				if (TargetPriorityVolume > -1.f && PPVolume->Priority == TargetPriorityVolume)
				{
					HighestPrio = PPVolume->Priority;
					break;
				}
				else if (PPVolume->Priority > HighestPrio)
				{
					HighestPrio = PPVolume->Priority;
					HighestPrioVolume = PPVolume;
				}
			}
		}

		if (HighestPrioVolume != NULL)
		{
			HighestPrioVolume->Settings.bOverride_SceneFringeIntensity = bForceOverride;
			HighestPrioVolume->Settings.bOverride_ChromaticAberrationStartOffset = bForceOverride;
			HighestPrioVolume->Settings.SceneFringeIntensity = NewChromaticAbberation;
			HighestPrioVolume->Settings.ChromaticAberrationStartOffset = NewChromaticAbberationStartOffset;
		}
	}
}

void UPA_BlueprintFunctionLibrary::SetBlendable(ACharacter* Character, UMaterialInterface* Blendable, float BlendAmount, bool bForceOtherBlendsToZero)
{
	//TODO: Make it also take into account higher priority volumes, possibly using the PostProcessVolumes array from World
	//TODO: Like Chromatic Aberration, include an option to search for specific PP volumes.
	if (Character && Blendable)
	{
		for (TActorIterator<APostProcessVolume> ActorItr(Character->GetWorld()); ActorItr; ++ActorItr)
		{
			APostProcessVolume* PPVolume = *ActorItr;

			if (PPVolume->bUnbound)
			{
				if (bForceOtherBlendsToZero)
				{
					UObject* Object = Cast<UObject>(Blendable);

					if (Object)
					{
						for (int32 BlendIndex = 0, BlendCount = PPVolume->Settings.WeightedBlendables.Array.Num(); BlendIndex < BlendCount; ++BlendIndex)
						{
							if (PPVolume->Settings.WeightedBlendables.Array[BlendIndex].Object != Object)
							{
								//reset all blendables to 0 except for the active one.
								PPVolume->Settings.WeightedBlendables.Array[BlendIndex].Weight = 0.f;
							}
						}
					}
				}

				PPVolume->Settings.AddBlendable(Blendable, BlendAmount); //AddBlendable already updates any existing ones by itself. 
			}
		}
	}
}

FString UPA_BlueprintFunctionLibrary::GetScreenshot(UObject* WorldContextObject, FString ScreenshotName)
{
	//First make a screenshot
	FScreenshotRequest::RequestScreenshot(ScreenshotName, false, false);

	//The get the screenshot path
	FString ScreenshotPath = FPaths::ProjectSavedDir();
	FString Platform = UGameplayStatics::GetPlatformName();
	ScreenshotPath += "Screenshots/" + Platform + "/" + ScreenshotName;

	return ScreenshotPath;
}

//Dialogue
void UPA_BlueprintFunctionLibrary::SpawnDialogue(APlayerController* Controller, TArray<AActor*> NPCs, UDialogue* Dialogue)
{
	if (Controller
		&& Dialogue
		&& NPCs.Num() != 0)
	{
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(Controller->GetHUD());
		if (HUD)
			HUD->SpawnDialogue(NPCs, Dialogue);
	}
}

void UPA_BlueprintFunctionLibrary::RemoveDialogue(APlayerController* Controller, UDialogue* Dialogue)
{
	if (Controller
		&& Dialogue)
	{
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(Controller->GetHUD());
		if (HUD)
			HUD->RemoveDialogue(Dialogue);
	}
}

void UPA_BlueprintFunctionLibrary::SpawnItemGetPopup(ACharacter* Player, class UPA_Asset_Item* ItemData, TSubclassOf<class UPA_HudElements_ItemGet> ItemPopupClass)
{
	if (Player)
	{
		//THIS WILL STOP SPAWNING DIALOGUE WHEN YOU SPAWN A DIALOGUE ON THE CHARACTER WHEN IT'S TURNED INTO AN AI.
		APlayerController* PC = Cast<APlayerController>(Player->GetController());
		if (PC)
		{
			APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());
			if (HUD)
			{
				if (HUD->ItemPopupInstance != NULL)
					HUD->ItemPopupInstance->DeconstructItemPopup();

				HUD->SpawnItemGetWindow(ItemData, ItemPopupClass);
			}
		}
	}
}

void UPA_BlueprintFunctionLibrary::RemoveItemGetPopup(ACharacter* Player)
{
	if (Player)
	{
		APlayerController* PC = Cast<APlayerController>(Player->GetController());
		if (PC)
		{
			APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());
			if (HUD
				&& HUD->ItemPopupInstance)
				HUD->ItemPopupInstance->DeconstructItemPopup();
		}
	}
}

FString UPA_BlueprintFunctionLibrary::GetTextWithEnters(FText InputString)
{
	FString ReturnText = InputString.ToString();

	if (!ReturnText.IsEmpty())
	{
		bool bReachedEndOfText = false;

		int32 CurrentIdx = 0;

		while (!bReachedEndOfText)
		{
			FString CurrentLetter = ReturnText.Mid(CurrentIdx, 2);
			if (CurrentLetter == "*E")
			{
				ReturnText.RemoveAt(CurrentIdx, 2);
				ReturnText.InsertAt(CurrentIdx, "\n");
				UE_LOG(AnyLog, Warning, TEXT("Found a n directive"));
			}

			CurrentIdx++;
			if (CurrentIdx > ReturnText.Len())
				bReachedEndOfText = true;

		}
	}

	return ReturnText;

	//return NewString.ToString(); //is this even going to work?
}

//Camera
void UPA_BlueprintFunctionLibrary::GetOrthographicCameraFrustumEdges(class UCineCameraComponent* CameraComponent, FVector& TopCenter, FVector& LeftCenter, FVector& BottomCenter, FVector& RightCenter)
{
	if (CameraComponent)
	{
		FMinimalViewInfo CameraView;
		CameraComponent->GetCameraView(0, CameraView);

		float width = CameraView.OrthoWidth;
		float height = width / CameraView.AspectRatio;

		TopCenter.Set(CameraView.Location.X, CameraView.Location.Y, CameraView.Location.Z + (height * 0.5f));
		BottomCenter.Set(CameraView.Location.X, CameraView.Location.Y, CameraView.Location.Z - (height * 0.5f));
		LeftCenter.Set(CameraView.Location.X, CameraView.Location.Y - (width * 0.5f), CameraView.Location.Z);
		RightCenter.Set(CameraView.Location.X, CameraView.Location.Y + (width * 0.5f), CameraView.Location.Z);
	}
}

void UPA_BlueprintFunctionLibrary::GetPerspectiveCameraFrustumEdges(FVector OwnerWorldLocation, class UCineCameraComponent* CameraComponent, class USpringArmComponent* SpringArmComponent, FVector& TopCenter, FVector& LeftCenter, FVector& BottomCenter, FVector& RightCenter)
{
	if (CameraComponent
		&& SpringArmComponent
		&& SpringArmComponent->TargetArmLength > 0
		&& CameraComponent->GetWorld())
	{
		FMinimalViewInfo CameraView;
		CameraComponent->GetCameraView(0, CameraView);
		float CameraDistance = SpringArmComponent->TargetArmLength;
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(CameraComponent->GetWorld());

		//Just in case to prevent division by zero. Besides, if the viewportsize is zero, we weren't able to get the data anyway. 
		if (ViewportSize.X == 0 || ViewportSize.Y == 0)
			return;

		float hFOV = FMath::DegreesToRadians(CameraComponent->GetHorizontalFieldOfView());
		float vFOV = 2 * FMath::Atan(FMath::Tan(hFOV * 0.5) * (ViewportSize.Y / ViewportSize.X));
		float vFOVoffset_top = 0; //in case our camera has an angle, we need to calculate an offset.
		float vFOVoffset_bottom = 0; //in case our camera has an angle, we need to calculate an offset.
		float halfheight = FMath::Tan(vFOV / 2) * CameraDistance;
		float halfwidth = FMath::Tan(hFOV / 2) * CameraDistance;

		//calculate the offset if our springarm has a pitch rotation.
		//We need two different lengths; one using CameraDistance as hypotenuse (long) and one using the halfheight as hypotenuse (short). 
		//Depending on the camera angle, we need to offset both the bottom and top Z with either the short or the long offset.
		if (SpringArmComponent->PreviousDesiredRot.Pitch != 0)
		{
			//if our camera views from upwards, we need the bottom offset to be long and the top to be short.
			if (SpringArmComponent->PreviousDesiredRot.Pitch < 0)
			{
				float CameraAngle = FMath::Abs(SpringArmComponent->PreviousDesiredRot.Pitch);
				vFOVoffset_top = ((CameraDistance * FMath::Cos(vFOV * 0.25)) * FMath::Acos(vFOV * 0.25)) * 0.5; //top one is short one
				vFOVoffset_bottom = (CameraDistance / FMath::Sin(90.f - CameraAngle - (vFOV * 0.5))) * FMath::Sin(vFOV * 0.5); //bottom one is long one
			}
			else
			{
				float CameraAngle = FMath::Abs(SpringArmComponent->PreviousDesiredRot.Pitch);
				vFOVoffset_bottom = ((CameraDistance * FMath::Cos(vFOV * 0.25)) * FMath::Acos(vFOV * 0.25)) * 0.5; //top one is short one
				vFOVoffset_top = (CameraDistance / FMath::Sin(90.f - CameraAngle - (vFOV * 0.5))) * FMath::Sin(vFOV * 0.5); //bottom one is long one
			}
		}
		else
		{
			//if there's no pitch rotation, we can just use the halfheight.
			vFOVoffset_top = halfheight;
			vFOVoffset_bottom = halfheight;
		}

		TopCenter.Set(OwnerWorldLocation.X, OwnerWorldLocation.Y, OwnerWorldLocation.Z + vFOVoffset_top);
		BottomCenter.Set(OwnerWorldLocation.X, OwnerWorldLocation.Y, OwnerWorldLocation.Z + vFOVoffset_bottom); //since the result is negative, we add it to keep it negative.
		LeftCenter.Set(OwnerWorldLocation.X, OwnerWorldLocation.Y + halfwidth, OwnerWorldLocation.Z);
		RightCenter.Set(OwnerWorldLocation.X, OwnerWorldLocation.Y - halfwidth, OwnerWorldLocation.Z);
	}
}

//-------------------------------------------------------------------------------------------------------
// INPUT RELATED FUNCTIONS
//-------------------------------------------------------------------------------------------------------

float UPA_BlueprintFunctionLibrary::GetModifiedFloat(float Input, float DeadzoneLow, float DeadzoneHigh)
{
	float Output = 0.f;

	Output = FMath::Clamp(FMath::Abs(Input) - DeadzoneLow, 0.f, DeadzoneHigh);

	if (Output <= 0)
		return 0.f;

	return FMath::Clamp(Output / (DeadzoneHigh = DeadzoneLow), 0.f, 1.f);
}

float UPA_BlueprintFunctionLibrary::GetInputStrength(float InputX, float InputY, float DeadzoneLow, float DeadzoneHigh)
{
	FVector2D Input = FVector2D(InputX, InputY);
	float ModFloat = GetModifiedFloat(Input.Size(), DeadzoneLow, DeadzoneHigh);
	Input.Normalize();

	if (Input.Size() == 0.f)
		return 0.f;

	return FMath::Clamp(ModFloat / Input.Size(), 0.f, 1.f);
}

void UPA_BlueprintFunctionLibrary::GetCorrectedInput(float InputX, float InputY, float DeadzoneLow, float DeadzoneHigh, float& OutputX, float& OutputY)
{
	FVector2D Input = FVector2D(InputX, InputY);
	Input.Normalize();
	Input *= GetInputStrength(InputX, InputY, DeadzoneLow, DeadzoneHigh);
	
	OutputX = Input.X;
	OutputY = Input.Y;
}

//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
// COIN RELATED FUNCTIONS
//-------------------------------------------------------------------------------------------------------

int32 UPA_BlueprintFunctionLibrary::GetCoinCount(APiratesAdventureCharacter* Character)
{
	if (Character)
	{
		return Character->CoinCount;
	}

	return -1;
}

void UPA_BlueprintFunctionLibrary::AddCoin(class APiratesAdventureCharacter* Character, int32 CoinNum)
{
	if (Character)
	{
		Character->CoinCount += CoinNum;
		if (Character->CoinCount > 9999) Character->CoinCount = 9999;
		if (Character->CoinCount < 0) Character->CoinCount = 0;
	}
}

//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
// AI RELATED FUNCTIONS
//-------------------------------------------------------------------------------------------------------

bool UPA_BlueprintFunctionLibrary::GetPolygonsFromNavMesh(const ACharacter* Character, TArray<FNavPolyData> &PolyRefs)
{
	UWorld* World = Character ? Character->GetWorld() : nullptr;
	if (!World)
	{
		UE_LOG(AnyLog, Warning, TEXT("NO WORLD"));
		return false;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	
	if (NavSys)
	{
		//UNavMovementComponent* MoveComp = Cast<UNavMovementComponent>(Controller->GetComponentByClass(UNavMovementComponent::StaticClass()));
		UNavMovementComponent* MoveComp = Character->GetMovementComponent();
		if (!MoveComp)
		{
			UE_LOG(AnyLog, Warning, TEXT("NO MOVEMENT COMP WHILE TRYING TO GET NAVMESH POLIES"));
			return false;
		}

		const FNavAgentProperties& AgentProperties = MoveComp->GetNavAgentPropertiesRef();
		const ANavigationData* NavData = NavSys->GetNavDataForProps(AgentProperties);
		const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);

		if (NavMesh)
		{
			TArray<FNavPoly> EachPolys;
			//TArray<FVector> RecordedPolyCenters;
			FBox TileBounds;

			for (int32 i = 0; i < NavMesh->GetNavMeshTilesCount(); i++)
			{
				TileBounds = NavMesh->GetNavMeshTileBounds(i);
				if (TileBounds.IsValid == 0) continue;

				EachPolys.Empty(); //Make sure EachPolys is emptied out before we get new data. 
				NavMesh->GetPolysInTile(i, EachPolys);
				if (EachPolys.Num() > 0)
				{
					for (int32 j = 0; j < EachPolys.Num(); j++)
					{
						FNavPolyData NewData;
						NewData.TileID = i;
						NewData.TileCenter = TileBounds.GetCenter();
						NewData.PolyID = FMath::FloorToInt((float)EachPolys[j].Ref);
						NewData.PolyIDRef = EachPolys[j].Ref;

						//NavMesh->GetPolyCenter(EachPolys[j].Ref, NewData.PolyCenter); //this has very questionable centers.
						NavMesh->GetPolyVerts(EachPolys[j].Ref, NewData.PolyVerts);
						NewData.PolyCenter = UKismetMathLibrary::GetVectorArrayAverage(NewData.PolyVerts); //This saves us from calling that function tons of times.

						//Trace if our poly is not encroached
						TArray<FHitResult> Hits;
						TArray<AActor> ActorsToIgnore;
						static const FName SphereTraceMultiName(TEXT("SphereTraceMulti"));
						FCollisionQueryParams Params(TEXT(""), false, Character);
						bool bHit = World->SweepMultiByChannel(Hits, NewData.PolyCenter + FVector(0, 0, 40), NewData.PolyCenter + FVector(0, 0, 60), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(40.f), Params);
						if (bHit)
						{
							UE_LOG(AnyLog, Warning, TEXT("We delated a nav poly from the array because of encrouchment"));
							continue; //if we have a hit, don't add it to the array.
						}

						TArray<NavNodeRef> Temp;
						NavMesh->GetPolyNeighbors(EachPolys[j].Ref, Temp);
						int32 NeighbourCount = Temp.Num();

						//UE_LOG(AnyLog, Warning, TEXT("TileIndex %d and PolyIndex %d has a total of %d neighbours"), i, j, NeighbourCount);

						PolyRefs.Add(NewData);
					}
				}
			}

			//Next we will want to assign to all polys potential connections so we don't have to do that later
			//The first loop iterates every element. The second loop is responsible for comparing all elements to its peers. The final loop is the actual neighbour checking.
			for (int32 k = 0; k < PolyRefs.Num(); k++)
			{
				TArray<NavNodeRef> NeighbourPolys;
				NavMesh->GetPolyNeighbors(PolyRefs[k].PolyIDRef, NeighbourPolys);

				//UE_LOG(AnyLog, Warning, TEXT("Index %d has a total of %d neighbours"), k, NeighbourPolys.Num());

				for (NavNodeRef Ref : NeighbourPolys)
				{
					for (int32 l = 0; l < PolyRefs.Num(); l++)
					{
						if (k != l) //make sure we are not comparing the same thing
						{
							if (PolyRefs[l].PolyIDRef == Ref)
							{
								PolyRefs[k].ConnectedPolys.Add(l);
								continue; //quit the current loop execution, because we already have a hit.
							}

							//make sure we also sample other polys, in particular those on a different Z plane. 
							if (FVector::Dist2D(PolyRefs[k].PolyCenter, PolyRefs[l].PolyCenter) <= NavMesh->TileSizeUU)
								PolyRefs[k].NearPolys.Add(l);
						}
					}
				}
			}

			return true;
		}
	}
	else
		UE_LOG(AnyLog, Warning, TEXT("NO NAVSYS WHILE TRYING TO GET NAVMESH POLIES"));
				
	UE_LOG(AnyLog, Warning, TEXT("EVERYTHING FAILED WHILE TRYING TO GET NAVMESH POLIES"));
	return false; //if the entire thing fails
}