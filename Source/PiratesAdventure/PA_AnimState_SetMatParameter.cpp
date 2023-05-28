// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_AnimState_SetMatParameter.h"
#include "PiratesAdventure.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PiratesAdventureCharacter.h"
//#include "PA_SkeletalMeshActor_Base.h"
#include "PA_BlueprintFunctionLibrary.h"
#include "PA_Interface_GlobalCharacter.h"

void UPA_AnimState_SetMatParameter::NotifyBegin(USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration)
{
	// Only start morphing if we've got valid params
	if (ValidateParameters(MeshComp))
	{
		AnimDuration = TotalDuration;
		ElapsedTime = 0.f;
		
		Received_NotifyBegin(MeshComp, Animation, TotalDuration);
	}
}

void UPA_AnimState_SetMatParameter::NotifyTick(USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (ValidateParameters(MeshComp)
		&& Animation
		&& MaterialParameterCollection != NULL)
	{
		ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
		if (Character)
		{
			ElapsedTime += FrameDeltaTime * (1.f / (AnimDuration / Animation->RateScale));
			float DurationPct = FMath::Clamp(ElapsedTime, 0.f, 1.f);
			float BlendPct;

			if (bEaseIn)
			{
				if (bEaseOut)
				{
					// EASE IN/OUT
					BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, CurveIntensity);
				}
				else
				{
					// EASE IN
					BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, CurveIntensity));
				}
			}
			else
			{
				if (bEaseOut)
				{
					// EASE OUT
					BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / CurveIntensity));
				}
				else
				{
					// LINEAR
					BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
				}
			}

			//clamp BlendPCT just in case
			BlendPct = FMath::Clamp(BlendPct, 0.f, 1.f);
			float AnimValue = FMath::Lerp(StartValue, TargetValue, BlendPct);

			IPA_Interface_GlobalCharacter* CharacterInterface = Cast<IPA_Interface_GlobalCharacter>(Character);
			if (CharacterInterface)
			{
				CharacterInterface->UpdateScalarMaterialParameterSet(MaterialParameterCollection, ParameterName, AnimValue);
			}
		}
	}

	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UPA_AnimState_SetMatParameter::NotifyEnd(USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation)
{
	Received_NotifyEnd(MeshComp, Animation);
}

bool UPA_AnimState_SetMatParameter::ValidateParameters(USkeletalMeshComponent* MeshComp)
{
	bool bValid = false;

	if (MaterialParameterCollection != NULL)
	{
		bValid = true;
	}

	//UE_LOG(AnyLog, Log, TEXT("bValid %d"), bValid);

	return bValid;
}



