// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_NavArea_Jump.h"
#include "PiratesAdventure.h"

UPA_NavArea_Jump::UPA_NavArea_Jump()
{
	FNavAreaHelper::Set(AreaFlags, ENavAreaFlag::Jump);
}

//void UPA_NavArea_Jump::FinishDestroy()
//{
//	if (HasAnyFlags(RF_ClassDefaultObject)
//#if WITH_HOT_RELOAD
//		&& !GIsHotReload
//#endif
//		)
//	{
//		UE_LOG(LogTemp, Error, TEXT("Did not unregister %s, because of bug in UE4"), *(GetClass()->GetName()));
//		// UNavigationSystem::RequestAreaUnregistering(GetClass());
//	}
//	UObject::FinishDestroy();
//}