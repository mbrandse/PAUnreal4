// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_HudElements_RingMenu.h"
#include "PiratesAdventure.h"
#include "PA_PlayerHUD.h"
#include "WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PiratesAdventureCharacter.h"
#include "PA_Weapon_Base.h"

void UPA_HudElements_RingMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
	if (!RootPanel)
		return;

	//Make sure we populate the arrays first before we start doing anything with them. 
	if (ElementRing.Num() != 0)
	{
		for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
		{
			ElementRing[RingIndex].Image = NewObject<UImage>(UImage::StaticClass());
			ElementRing[RingIndex].Image->Brush.DrawAs = ESlateBrushDrawType::Image;
			ElementRing[RingIndex].Image->Brush.ImageSize = FVector2D(IconSize, IconSize);
			ElementRing[RingIndex].Image->SetBrushFromMaterial(ElementRing[RingIndex].MaterialTemplate);
			ElementRing[RingIndex].Material = ElementRing[RingIndex].Image->GetDynamicMaterial();

			ElementRing[RingIndex].Slot = RootPanel->AddChildToCanvas(ElementRing[RingIndex].Image);
			ElementRing[RingIndex].Slot->SetAutoSize(true);
			ElementRing[RingIndex].Slot->SetPosition(FVector2D::ZeroVector);

			if (ElementRing[RingIndex].bDefault && ElementRing[RingIndex].Material)
				ElementRing[RingIndex].Material->SetScalarParameterValue(IconActiveParameter, 1.f);

			if (OwnerHUD
				&& OwnerHUD->OwnerCharacter)
			{
				UE_LOG(AnyLog, Log, TEXT("WE HAVE A CHARACTER FOR OUR RING MENU, NOW CHECKING DEFAULT STATUS"));
				//if (!OwnerHUD->OwnerCharacter->IsRingElementUnlocked(RingType, ElementRing[RingIndex].ButtonAssociation))
				//	ElementRing[RingIndex].Image->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	SetVisibility(ESlateVisibility::Hidden);
	HudElementState = EHudElementState::HES_NoLoop; //stop animation the moment we call the construct.
}

void UPA_HudElements_RingMenu::SetActiveIcon(EButtonAssociation NewButton)
{
	for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
	{
		if (ElementRing[RingIndex].Material != NULL)
		{
			//float IconActiveValue;
			//ElementRing[RingIndex].Material->GetScalarParameterValue(IconActiveParameter, IconActiveValue);

			//if (IconActiveValue == 0.f)
			//{
				if (ElementRing[RingIndex].ButtonAssociation == NewButton)
				{
					ElementRing[RingIndex].Material->SetScalarParameterValue(IconActiveParameter, 1.f);
				}
				else
				{
					ElementRing[RingIndex].Material->SetScalarParameterValue(IconActiveParameter, 0.f); //to make sure only one button can be active at any given time.
				}
			//}
		}
	}
}

void UPA_HudElements_RingMenu::SetInactiveIcon(EButtonAssociation NewButton, bool bNewActive)
{
	bool bResetToDefault = false;

	for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
	{
		if (ElementRing[RingIndex].ButtonAssociation == NewButton)
		{
			if (!bNewActive)
			{
				//If we set the button to inactive, we also have to check whether the current icon was equipped (and if so, reset back to a default one if present)
				float CurrentValue;
				ElementRing[RingIndex].Material->GetScalarParameterValue(IconActiveParameter, CurrentValue);
				if (CurrentValue != 0.f)
				{
					ElementRing[RingIndex].Material->SetScalarParameterValue(IconActiveParameter, 0.f);
					bResetToDefault = true;
				}

				ElementRing[RingIndex].Material->SetScalarParameterValue(IconInactiveParameter, 1.f);
			}
			else
			{
				//We can safely assume that the player cannot both set the icon from inactive state and set it to selected as well.
				ElementRing[RingIndex].Material->SetScalarParameterValue(IconInactiveParameter, 0.f);
			}
			//ElementRing[RingIndex].Image->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (bResetToDefault)
	{
		for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
		{
			if (ElementRing[RingIndex].ButtonAssociation == EButtonAssociation::BUTTON_SELECT)
			{
				ElementRing[RingIndex].Material->SetScalarParameterValue(IconActiveParameter, 1.f);
			}
		}
	}
}

void UPA_HudElements_RingMenu::SetIconUnlocked(EButtonAssociation NewButton, bool bNewLockedState)
{
	for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
	{
		if (ElementRing[RingIndex].ButtonAssociation == NewButton)
		{
			if (!bNewLockedState)
			{
				ElementRing[RingIndex].Image->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				ElementRing[RingIndex].Image->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UPA_HudElements_RingMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* World = GetWorld();

	if (World)
	{
		FVector2D PlayerPosition2D, ViewportSize;
		FVector PlayerPosition;
		float XOffset, YOffset, Radians, NewAngle, NewDistance, NewScale, BlendPct, DurationPct;
		APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));

		if (!Character)
			return;

		switch (HudElementState)
		{
		case EHudElementState::HES_StartLoop:

			OnProcessOpenRingMenu(TimeElapsed);

			for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
			{
				DurationPct = TimeElapsed / IntroDuration;
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / 2.f));

				NewScale = FMath::Lerp(0.f, 1.f, BlendPct);
				ElementRing[RingIndex].Image->SetRenderScale(FVector2D(NewScale, NewScale));

				UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(UGameplayStatics::GetPlayerController(World, 0), Character->GetActorLocation(), PlayerPosition2D);
				PlayerPosition2D -= ElementRing[RingIndex].Image->GetDesiredSize() * 0.5f;

				NewAngle = FMath::Lerp(ElementRing[RingIndex].Angle - IconRotation, ElementRing[RingIndex].Angle, BlendPct);
				NewDistance = FMath::Lerp(0.f, IconDistance, BlendPct);

				Radians = NewAngle * (PI / 180.f);
				XOffset = NewDistance * FMath::Cos(Radians);
				YOffset = NewDistance * FMath::Sin(Radians);

				UGameplayStatics::SetGlobalTimeDilation(World, FMath::Lerp(1.f, MinTimeDilation, BlendPct));

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PLAYER POSITION: x: %f"), InDeltaTime));

				ElementRing[RingIndex].Image->SetRenderTranslation(FVector2D(XOffset + PlayerPosition2D.X, YOffset + PlayerPosition2D.Y));
			}

			TimeElapsed += InDeltaTime / IntroDuration;

			if (TimeElapsed >= IntroDuration)
			{
				//final set so we can make sure the icons are rotated properly, as sometimes the final tick is off. 
				for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
				{
					ElementRing[RingIndex].Image->SetRenderScale(FVector2D(1.f, 1.f));

					UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(UGameplayStatics::GetPlayerController(World, 0), Character->GetActorLocation(), PlayerPosition2D);
					PlayerPosition2D -= ElementRing[RingIndex].Image->GetDesiredSize() * 0.5f;

					Radians = ElementRing[RingIndex].Angle * (PI / 180.f);
					XOffset = IconDistance * FMath::Cos(Radians);
					YOffset = IconDistance * FMath::Sin(Radians);

					ElementRing[RingIndex].Image->SetRenderTranslation(FVector2D(XOffset + PlayerPosition2D.X, YOffset + PlayerPosition2D.Y));
				}

				//Selector->SetVisibility(ESlateVisibility::Visible);
				HudElementState = EHudElementState::HES_MainLoop;
			}

			break;

		case EHudElementState::HES_StopLoop:

			OnProcessCloseRingMenu(TimeElapsed);

			for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
			{
				DurationPct = TimeElapsed / OuttroDuration;
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 2.f));

				NewScale = FMath::Lerp(1.f, 0.f, BlendPct);
				ElementRing[RingIndex].Image->SetRenderScale(FVector2D(NewScale, NewScale));

				UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(UGameplayStatics::GetPlayerController(World, 0), Character->GetActorLocation(), PlayerPosition2D);
				PlayerPosition2D -= ElementRing[RingIndex].Image->GetDesiredSize() * 0.5f;

				NewAngle = FMath::Lerp(ElementRing[RingIndex].Angle, ElementRing[RingIndex].Angle + IconRotation, BlendPct);
				NewDistance = FMath::Lerp(IconDistance, 0.f, BlendPct);

				Radians = NewAngle * (PI / 180.f);
				XOffset = NewDistance * FMath::Cos(Radians);
				YOffset = NewDistance * FMath::Sin(Radians);

				UGameplayStatics::SetGlobalTimeDilation(World, FMath::Lerp(MinTimeDilation, 1.f, BlendPct));

				ElementRing[RingIndex].Image->SetRenderTranslation(FVector2D(XOffset + PlayerPosition2D.X, YOffset + PlayerPosition2D.Y));
			}

			TimeElapsed += InDeltaTime / OuttroDuration;

			if (TimeElapsed >= OuttroDuration)
			{
				HudElementState = EHudElementState::HES_NoLoop;
				SetVisibility(ESlateVisibility::Hidden);
				UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
			}

			break;

		case EHudElementState::HES_MainLoop:

			//Basic loop so we can make sure the ring is always centered. If this is not implemented in the main loop, 
			//We get issues when the during actions like falling, where the player is not centered
			for (int32 RingIndex = 0; RingIndex < ElementRing.Num(); RingIndex++)
			{
				ElementRing[RingIndex].Image->SetRenderScale(FVector2D(1.f, 1.f));

				UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(UGameplayStatics::GetPlayerController(World, 0), Character->GetActorLocation(), PlayerPosition2D);
				PlayerPosition2D -= ElementRing[RingIndex].Image->GetDesiredSize() * 0.5f;

				Radians = ElementRing[RingIndex].Angle * (PI / 180.f);
				XOffset = IconDistance * FMath::Cos(Radians);
				YOffset = IconDistance * FMath::Sin(Radians);

				ElementRing[RingIndex].Image->SetRenderTranslation(FVector2D(XOffset + PlayerPosition2D.X, YOffset + PlayerPosition2D.Y));
			}

			break;
		}
	}
}

//void UPA_HudElements_RingMenu::ProcessSelector()
//{
//	FVector2D PlayerPosition2D, ViewportSize;
//	FVector PlayerPosition;
//	UWorld* World = GetWorld();
//
//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PLAYER POSITION: x: %f y: %f"), PlayerPosition2D.X, PlayerPosition2D.Y));
//
//	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
//	if (!Character)
//		return;
//
//	//FIRST SET THE SCALE AND ROTATION OF THE SELECTOR
//	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(UGameplayStatics::GetPlayerController(World, 0), Character->GetActorLocation(), PlayerPosition2D);
//
//	Selector->SetRenderTranslation(FVector2D(PlayerPosition2D.X - (Selector->Brush.ImageSize.X * 0.5), PlayerPosition2D.Y));
//	Selector->SetRenderAngle((180.f) / PI * FMath::Atan2(Character->GetVerticalMovement() * -1, Character->GetHorizontalMovement()) + 90.f);
//
//	float SelectorLength = FVector2D(Character->GetHorizontalMovement(), Character->GetVerticalMovement()).Size();
//
//	Selector->SetRenderScale(FVector2D(1.f - (SelectorLength * 0.5), SelectorLength * SelectorMaxSize.Y));
//
//	//THEN USE THE SCALE AND ROTATION OF THE SELECTOR TO DETERMINE WHETHER IT HITS AN ICON, AND ACT DEPENDING ON IT.
//	float SelectorXLoc, SelectorYLoc;
//	FVector2D IconMinLoc, IconMaxLoc;
//
//	SelectorXLoc = Selector->RenderTransform.Translation.X + (Character->GetHorizontalMovement() * SelectorMaxSize.Y);
//	SelectorYLoc = Selector->RenderTransform.Translation.Y + ((Character->GetVerticalMovement() * -1) * SelectorMaxSize.Y);
//
//	bool bHit = false;
//
//	for (int32 IconIndex = 0; IconIndex < RingIcons.Num(); IconIndex++)
//	{
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CHECKING ICON LOCATION")));
//
//		IconMinLoc = RingIcons[IconIndex].Image->RenderTransform.Translation;
//		IconMaxLoc = RingIcons[IconIndex].Image->RenderTransform.Translation + RingIcons[IconIndex].Image->GetDesiredSize();
//
//		if (UKismetMathLibrary::InRange_FloatFloat(SelectorXLoc, IconMinLoc.X, IconMaxLoc.X, true, true)
//			&& UKismetMathLibrary::InRange_FloatFloat(SelectorYLoc, IconMinLoc.Y, IconMaxLoc.Y, true, true)
//			&& !RingIcons[IconIndex].bEmptySlot)
//		{
//			float IconInactiveValue, IconActiveValue;
//			RingIcons[IconIndex].Material->GetScalarParameterValue(RingIconData[IconIndex].IconInactive, IconInactiveValue);
//			RingIcons[IconIndex].Material->GetScalarParameterValue(RingIconData[IconIndex].IconActive, IconActiveValue);
//
//			if (IconInactiveValue == 0.f
//				&& IconActiveValue == 0.f)
//			{
//				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ICON SLOT EMPTY IS %d"), RingIcons[IconIndex].bEmptySlot));
//				RolloverIndex = IconIndex;
//				bHit = true;
//				RingIcons[IconIndex].Material->SetScalarParameterValue("Rollover", 1.f);
//			}
//		}
//		else
//			RingIcons[IconIndex].Material->SetScalarParameterValue("Rollover", 0.f);
//	}
//
//	if (!bHit)
//	{
//		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NO ICON SLOT")));
//		RolloverIndex = INDEX_NONE;
//	}
//
//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HITINDEX %d"), RolloverIndex));
//}

void UPA_HudElements_RingMenu::OpenRingMenu()
{
	TimeElapsed = 0.f;
	HudElementState = EHudElementState::HES_StartLoop;

	SetVisibility(ESlateVisibility::Visible);
}

void UPA_HudElements_RingMenu::CloseRingMenu()
{
	TimeElapsed = 0.f;
	HudElementState = EHudElementState::HES_StopLoop;
}

void UPA_HudElements_RingMenu::ProcessAccept()
{
	//UWorld* World = GetWorld();

	//if (World
	//	&& RolloverIndex != INDEX_NONE
	//	&& RingIconData[RolloverIndex].ItemClass)
	//{
	//	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));

	//	if (Character)
	//	{
	//		//first check if it's a weapon
	//		UBlueprint* BlueprintToCheck = Cast<UBlueprint>(RingIconData[RolloverIndex].ItemClass);

	//		if (BlueprintToCheck
	//			&& BlueprintToCheck->ParentClass->IsChildOf(UPA_Weapon_Base::StaticClass()))
	//		{
	//			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, FString::Printf(TEXT("DID IT!")));
	//			//RingIcons[RolloverIndex].Material->SetScalarParameterValue(RingIconData[RolloverIndex].IconActive, 1.f);
	//			Character->SetEquippedWeapon(Character->GetWeaponIndex(BlueprintToCheck->GeneratedClass));
	//		}

	//		ProcessActiveIcons(RolloverIndex);

	//		//TODO: Implement generic Item activation functionality. Should also update AMMO count here. 
	//	}
	//}
}

void UPA_HudElements_RingMenu::ProcessActiveIcons(int32 Index)
{
	//for (int32 RingIndex = 0; RingIndex < RingIcons.Num(); RingIndex++)
	//{
	//	if (RingIconData.IsValidIndex(RingIndex))
	//	{
	//		if (RingIndex != Index)
	//		{
	//			RingIcons[RingIndex].Material->SetScalarParameterValue(RingIconData[RingIndex].IconActive, 0.f);
	//		}
	//		else
	//			RingIcons[RingIndex].Material->SetScalarParameterValue(RingIconData[RingIndex].IconActive, 1.f);
	//	}
	//}
}

void UPA_HudElements_RingMenu::TurnOnInactiveIcons(int32 Index)
{
	//for (int32 RingIndex = 0; RingIndex < RingIcons.Num(); RingIndex++)
	//{
	//	if (RingIndex == Index && RingIconData.IsValidIndex(RingIndex))
	//	{
	//		RingIcons[RingIndex].Material->SetScalarParameterValue(RingIconData[RingIndex].IconRollover, 0.f);
	//		RingIcons[RingIndex].Material->SetScalarParameterValue(RingIconData[RingIndex].IconActive, 0.f);
	//		RingIcons[RingIndex].Material->SetScalarParameterValue(RingIconData[RingIndex].IconInactive, 1.f);
	//	}
	//}
}

void UPA_HudElements_RingMenu::TurnOffInactiveIcons(int32 Index)
{
	//for (int32 RingIndex = 0; RingIndex < RingIcons.Num(); RingIndex++)
	//{
	//	if (RingIndex == Index && RingIconData.IsValidIndex(RingIndex))
	//	{
	//		RingIcons[RingIndex].Material->SetScalarParameterValue(RingIconData[RingIndex].IconInactive, 0.f);
	//	}
	//}
}

int32 UPA_HudElements_RingMenu::GetRingIconIndex(FName ID)
{
	//for (int32 Index = 0; Index < RingIconData.Num(); Index++)
	//{
	//	if (RingIconData[Index].ID == ID)
	//		return Index;
	//}

	return INDEX_NONE;
}