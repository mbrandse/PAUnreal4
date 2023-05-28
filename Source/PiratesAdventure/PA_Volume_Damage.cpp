// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.


#include "PA_Volume_Damage.h"
#include "PA_Interface_GlobalCharacter.h"
#include "PiratesAdventureCharacter.h"
#include "PiratesAdventure.h"
#include "Kismet/GameplayStatics.h"

APA_Volume_Damage::APA_Volume_Damage()
{
	bIsContinuousDamage = true;
	bSkipEntryDamage = false;

	Damage = 1.f;
	Interval = 4.f;
}

void APA_Volume_Damage::ActorEnteredVolume(class AActor* Other)
{
	if (UWorld* World = GetWorld())
	{
		UE_LOG(AnyLog, Warning, TEXT("%s ENTERED DAMAGE VOLUME"), *(Other->GetName()));

		bool bHasProcessedCharacter = false;

		ACharacter* OverlappingCharacter = Cast<ACharacter>(Other);
		OverlappingPlayer = Cast<APiratesAdventureCharacter>(Other);

		if (CharacterClassLimits.Num() != 0)
		{
			if (!CheckForAllowedClass(OverlappingCharacter))
				return;
		}

		if (bIsContinuousDamage
			&& OverlappingCharacter)
		{	
			IPA_Interface_GlobalCharacter* Interface = Cast<IPA_Interface_GlobalCharacter>(Other);
			UDamageType* DamageTypeObject = DamageType->GetDefaultObject<UDamageType>();

			if (Interface
				&& OverlappingCharacter
				&& DamageTypeObject)
			{
				if(!bSkipEntryDamage
					&& Interface->Execute_CanTakeDamage(OverlappingCharacter))
					Interface->Execute_OnProcessTimedDamage(OverlappingCharacter, DamageTypeObject, Damage, NulledElement);

				OverlappingActors.Add(OverlappingCharacter, FMath::FloorToInt(UGameplayStatics::GetTimeSeconds(World)));
				if (!World->GetTimerManager().IsTimerActive(UpdateTimerHandle))
				{
					World->GetTimerManager().SetTimer(UpdateTimerHandle, this, &APA_Volume_Damage::UpdateAllOverlappingActors, 1.f, true);
				}

				OverlappingPlayer->AirDecrease = AirDecrease;

				bHasProcessedCharacter = true;
			}
		}
		else if(OverlappingPlayer)
		{	
			UWorld* World = GetWorld();
			if (OverlappingPlayer
				&& World)
			{
				OverlappingPlayer->GetCameraBoom()->StartManualCameraTranslation(
					OverlappingPlayer->GetCameraBoom()->GetComponentLocation(),
					OverlappingPlayer->GetCameraBoom()->GetComponentRotation(),
					OverlappingPlayer->GetCameraBoom()->DesiredTargetArmLength,
					false, 1.f);
				World->GetTimerManager().SetTimer(UpdateTimerHandle, this, &APA_Volume_Damage::CallDelayedCharacterCheckpoint, Interval, false);
				OverlappingPlayer->DisableMovement();

				bHasProcessedCharacter = true;
			}
		}

		if (!bHasProcessedCharacter)
		{
			UE_LOG(AnyLog, Warning, TEXT("Trying to process the actor who just entered"));

			if (Other->bCanBeDamaged
				&& ActorDamageType != NULL)
			{
				UE_LOG(AnyLog, Warning, TEXT("Trying to damage the actor"));

				UGameplayStatics::ApplyDamage(Other, Damage, World->GetFirstPlayerController(), this, ActorDamageType);
			}
		}
	}
}

void APA_Volume_Damage::ActorLeavingVolume(class AActor* Other)
{
	if (UWorld* World = GetWorld())
	{
		ACharacter* Character = Cast<ACharacter>(Other);

		if (AirDecrease != 0.f)
		{
			APiratesAdventureCharacter* PlayerCharacter = Cast<APiratesAdventureCharacter>(Other);
			if (PlayerCharacter)
			{
				PlayerCharacter->AirDecrease = 0.f;
			}
		}

		if (Character)
		{
			if (OverlappingActors.Contains(Character))
			{
				OverlappingActors.Remove(Character);
				if (OverlappingActors.Num() == 0)
				{
					World->GetTimerManager().ClearTimer(UpdateTimerHandle);
				}
			}
		}
	}
}

//This is a rough interpretation of a variable timer system
void APA_Volume_Damage::UpdateAllOverlappingActors()
{
	if (UWorld* World = GetWorld())
	{
		int32 CurrentTime = FMath::FloorToInt(UGameplayStatics::GetTimeSeconds(World));
		for (auto& Element : OverlappingActors)
		{
			ACharacter* Character = Element.Key;
			UDamageType* DamageTypeObject = DamageType->GetDefaultObject<UDamageType>();
			int32 IntervalINT = FMath::FloorToInt(Interval);
			APiratesAdventureCharacter* PlayerCharacter = Cast<APiratesAdventureCharacter>(Character);

			if (AirDecrease != 0.f
				&& PlayerCharacter
				&& PlayerCharacter->RemainingAir > 0.f)
			{
				//don't do anything if the remaining air is more than 0, and this is an air decrease volume.
			}
			else if (Character
				&& DamageTypeObject)
			{

				IPA_Interface_GlobalCharacter* Interface = Cast<IPA_Interface_GlobalCharacter>(Character);
				if (Interface)
				{
					if (Interface->Execute_CanTakeDamage(Character))
					{
						if ((CurrentTime - Element.Value) % IntervalINT == 0)
							Interface->Execute_OnProcessTimedDamage(Character, DamageTypeObject, Damage, NulledElement);
					}
					else
					{
						OverlappingActors.Remove(Character);
					}
				}
			}
		}
	}
}

void APA_Volume_Damage::CallDelayedCharacterCheckpoint()
{
	if (OverlappingPlayer)
	{
		OverlappingPlayer->LoadManualCheckpoint(Damage, DamageType);
		OverlappingPlayer->GetCameraBoom()->StopManualCameraTranslation();
	}
}

bool APA_Volume_Damage::CheckForAllowedClass(ACharacter* ReferenceToCheck)
{
	if (CharacterClassLimits.Num() != 0)
	{
		for (int32 i = 0; i < CharacterClassLimits.Num(); i++)
		{
			if (CharacterClassLimits[i] == ReferenceToCheck->GetClass())
				return true;
		}

		return false;
	}

	return true;
}