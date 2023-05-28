// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_HudElements_Status.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
//#include "InterpolateFloat.h"
#include "PA_HudElements_IngameMenu.h"

void UPA_HudElements_Status::NativeConstruct()
{
	Super::NativeConstruct();

	if (OwnerHUD)
	{
		APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OwnerHUD->PlayerOwner->GetCharacter());
		if (Character)
		{
			if (HealthVesselMaterialTemplate && HealthVessels.Num() == 0)
			{
				UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
				if (!RootPanel)
					return;

				FAnchors StatusIndicatorAnchors;
				StatusIndicatorAnchors.Minimum = FVector2D(0, 1);
				StatusIndicatorAnchors.Maximum = FVector2D(0, 1);

				HealthbarPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
				if (!HealthbarPanel) return;
				UCanvasPanelSlot* HealthbarPanelSlot = RootPanel->AddChildToCanvas(HealthbarPanel);
				HealthbarPanelSlot->SetAutoSize(true);
				HealthbarPanelSlot->SetAnchors(StatusIndicatorAnchors);
				HealthbarPanelSlot->SetPosition(VesselBarMargin);

				for (int32 VesselIndex = 0; VesselIndex <= (int32)Character->CurrentMaxHealth; VesselIndex++)
				{
					class UImage* VesselImage = NewObject<UImage>(UImage::StaticClass());
					if (VesselImage)
					{
						VesselImage->Brush.DrawAs = ESlateBrushDrawType::Image;
						VesselImage->Brush.ImageSize = VesselSize;
						VesselImage->SetBrushFromMaterial(HealthVesselMaterialTemplate);
						HealthVesselMaterials.Add(VesselImage->GetDynamicMaterial());

						UCanvasPanelSlot* VesselSlot = HealthbarPanel->AddChildToCanvas(VesselImage);
						VesselSlot->SetAutoSize(true);
						VesselSlot->SetPosition(FVector2D(VesselMargin.X * (VesselIndex % 10), VesselMargin.Y * FMath::FloorToInt(VesselIndex / 10.f)));
						VesselSlot->SetZOrder(VesselIndex);

						HealthVessels.Add(VesselImage);
					}
				}

				//Initialize the vessels based on current health.
				OldHealth = Character->RemainingHealth;
				CurrentHealth = OldHealth;
				ForceHealthAmount();
			}

			//if (FullScreenFadeMaterial)
			//{
			//	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
			//	if (!RootPanel)
			//		return;

			//	FullScreenFadeImage = NewObject<UImage>(UImage::StaticClass());
			//	if (FullScreenFadeImage)
			//	{
			//		//TODO: test whether this scales properly when setting other resolutions
			//		FullScreenFadeImage->Brush.DrawAs = ESlateBrushDrawType::Image;
			//		FullScreenFadeImage->SetBrushFromMaterial(FullScreenFadeMaterial);
			//		FullScreenFadeImage->SetColorAndOpacity(FLinearColor(1.0, 1.0, 1.0, 0.0));
			//		FullScreenFadeImage->SetVisibility(ESlateVisibility::Hidden);

			//		FullScreenFadeSlot = RootPanel->AddChildToCanvas(FullScreenFadeImage);
			//		FullScreenFadeSlot->SetPosition(FVector2D(0, 0));
			//		FullScreenFadeSlot->SetSize(FVector2D(1920, 1080));
			//		FullScreenFadeSlot->SetAutoSize(false);
			//		FullScreenFadeSlot->SetZOrder(100);
			//	}
			//}
		}
	}
}

void UPA_HudElements_Status::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bCanAnimateHealthBarDamage)
	{
		if (HealthVesselsToUpdate > 0)
		{
			if (!HealthVesselMaterials.IsValidIndex(MinIndex + HealthVesselsToUpdate)) return;

			//first get the material value
			float OldVesselValue;
			HealthVesselMaterials[MinIndex + HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameDamageAlpha, OldVesselValue);
			if (OldVesselValue <= 0.f)
			{
				HealthVesselsToUpdate--;
				if (HealthVesselsToUpdate == 0)
				{
					bCanAnimateHealthBarDamage = false;
					return;
				}

				if (!HealthVesselMaterials.IsValidIndex(MinIndex + HealthVesselsToUpdate)) return;

				HealthVesselMaterials[MinIndex + HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameDamageAlpha, OldVesselValue);
			}

			float TargetVesselValue;
			HealthVesselMaterials[MinIndex + HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameTargetAlpha, TargetVesselValue);
			HealthVesselMaterials[MinIndex + HealthVesselsToUpdate]->SetScalarParameterValue(ParameterNameDamageAlpha, FMath::FInterpConstantTo(OldVesselValue, TargetVesselValue, InDeltaTime, VesselAnimationSpeed));
		}
	}

	if (bCanAnimateHealthBarRecovery)
	{
		if (HealthVesselsToUpdate >= 0)
		{
			if (!HealthVesselMaterials.IsValidIndex(MaxIndex - HealthVesselsToUpdate)) return;

			//first get the material value
			float OldVesselValue, TargetVesselValue;
			HealthVesselMaterials[MaxIndex - HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameTargetAlpha, OldVesselValue);
			HealthVesselMaterials[MaxIndex - HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameDamageAlpha, TargetVesselValue);

			if (OldVesselValue >= TargetVesselValue)
			{
				HealthVesselsToUpdate--;
				if (HealthVesselsToUpdate < 0)
				{
					bCanAnimateHealthBarRecovery = false;
					return;
				}

				if (!HealthVesselMaterials.IsValidIndex(MaxIndex - HealthVesselsToUpdate)) return;

				HealthVesselMaterials[MaxIndex - HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameTargetAlpha, OldVesselValue);
				HealthVesselMaterials[MaxIndex - HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameDamageAlpha, TargetVesselValue);
			}

			//HealthVesselMaterials[MinIndex + HealthVesselsToUpdate]->GetScalarParameterValue(ParameterNameTargetAlpha, TargetVesselValue);
			HealthVesselMaterials[MaxIndex - HealthVesselsToUpdate]->SetScalarParameterValue(ParameterNameTargetAlpha, FMath::FInterpConstantTo(OldVesselValue, TargetVesselValue, InDeltaTime, VesselAnimationSpeed));
		}
	}
}

float UPA_HudElements_Status::ProcessDamage(float Damage)
{
	if (Damage < 0) Damage *= -1; //minor failsafe so we don't get weird animations.

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OwnerHUD->PlayerOwner->GetCharacter());
	if (GetWorld()
		&& Character
		&& CurrentHealth > 0.f)
	{	
		OldHealth = CurrentHealth;
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, -1.f, 19.f); //TODO, link this to the max health in the character

		//if (CurrentHealth < 0) CurrentHealth = 0;
		MinIndex = FMath::FloorToInt(CurrentHealth);
		MaxIndex = FMath::CeilToInt(OldHealth);

		ForceHealthAmount();
		HealthVesselsToUpdate = MaxIndex - MinIndex;

		for (int32 VesselIndex = MaxIndex; VesselIndex > MinIndex; VesselIndex--)
		{
			if (HealthVesselMaterials.IsValidIndex(VesselIndex))
			{
				HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameRecoveryAlpha, 0);
				HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameDamageAlpha, 1);

				if (VesselIndex != MinIndex + 1)
				{
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, 0);
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 0);
				}
				else
				{
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, FMath::Frac(CurrentHealth));

					//only set the active vessel if we have enough vessels. When the player is dead, we don't need to set an active vessel.
					if (FMath::Frac(CurrentHealth) != 0.f)
					{
						HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 1);
					}
					else if(CurrentHealth > 0.f)
					{
						HealthVesselMaterials[VesselIndex - 1]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 1);
					}
				}
			}
		}

		//finally, in case the old damage comes with a fracture, make sure to setup the final damage alpha correctly
		if (FMath::Frac(OldHealth) != 0.f && HealthVesselMaterials.IsValidIndex((int32)OldHealth + 1))
		{
			HealthVesselMaterials[(int32)OldHealth + 1]->SetScalarParameterValue(ParameterNameDamageAlpha, FMath::Frac(OldHealth));
		}

		if (!bCanAnimateHealthBarDamage)
			GetWorld()->GetTimerManager().SetTimer(UpdateHealthVesselsTimerDamage, this, &UPA_HudElements_Status::StartHealthBarDamageAnimation, DelayBeforeVesselBarAnimationStart, false);

		return CurrentHealth;
	}

	return 0.f;
}

float UPA_HudElements_Status::ProcessRecovery(float Recovery)
{
	if (Recovery < 0) Recovery *= -1; //minor failsafe so we don't get weird animations.

	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OwnerHUD->PlayerOwner->GetCharacter());
	if (GetWorld()
		&& Character
		&& CurrentHealth < Character->CurrentMaxHealth)
	{
		OldHealth = CurrentHealth;
		CurrentHealth = FMath::Clamp(CurrentHealth + Recovery, -1.f, 19.f); 
		
		MinIndex = FMath::FloorToInt(OldHealth);
		MaxIndex = FMath::CeilToInt(CurrentHealth);
		//turn off the current final heart anim in advance, since we will update the MinIndex later.
		HealthVesselMaterials[MinIndex]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 0.f);

		bool bOldHealthIsFraction = FMath::Frac(OldHealth) != 0.f; 
		bool bNewHealthIsFraction = FMath::Frac(CurrentHealth) != 0.f;
		
		MinIndex++;

		ForceHealthAmount(); //force HealthAmount regardless of what happens, so we can cancel out any lingering health animation that may cause trouble.
		HealthVesselsToUpdate = MaxIndex - MinIndex;

		for (int32 VesselIndex = MinIndex; VesselIndex <= MaxIndex; VesselIndex++)
		{
			if (HealthVesselMaterials.IsValidIndex(VesselIndex))
			{
				//first initialize the vessels so we can start filling them up
				HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameRecoveryAlpha, 1.f);
				HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 0.f);

				//then determine whether we have fractional health on the first and or last vessel on the list
				if (VesselIndex == MinIndex && bOldHealthIsFraction)
				{
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameDamageAlpha, 1.f);
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, FMath::Frac(OldHealth));
				}
				else if (VesselIndex == MaxIndex && bNewHealthIsFraction)
				{
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, 0.f);
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameDamageAlpha, FMath::Frac(CurrentHealth));
				}
				else
				{
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameDamageAlpha, 1.f);
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, 0.f);
				}

				//finally set the final heart anim
				if (VesselIndex == MaxIndex)
				{
					HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 1.f);
				}
			}
		}

		if (!bCanAnimateHealthBarDamage)
			GetWorld()->GetTimerManager().SetTimer(UpdateHealthVesselsTimerDamage, this, &UPA_HudElements_Status::StartHealthBarRecoveryAnimation, DelayBeforeVesselBarAnimationStart, false);

		return CurrentHealth;
	}

	return 0.f;
}

void UPA_HudElements_Status::ForceHealthAmount()
{
	UE_LOG(AnyLog, Log, TEXT("FORCING HEALTH AMOUNT"));

	if (UWorld* World = GetWorld())
	{
		bCanAnimateHealthBarDamage = false;
		bCanAnimateHealthBarRecovery = false;

		if (World->GetTimerManager().IsTimerActive(UpdateHealthVesselsTimerDamage))
			World->GetTimerManager().ClearTimer(UpdateHealthVesselsTimerDamage);
			
		if(World->GetTimerManager().IsTimerActive(UpdateHealthVesselsTimerRecovery))
			World->GetTimerManager().ClearTimer(UpdateHealthVesselsTimerRecovery);

		//first set the materials to a basic on and off state
		for (int32 VesselIndex = 0; VesselIndex < HealthVesselMaterials.Num(); VesselIndex++)
		{
			HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameRecoveryAlpha, 0);
			HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameDamageAlpha, 0);

			if (VesselIndex > CurrentHealth)
			{
				HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, 0);
			}
			else
			{
				HealthVesselMaterials[VesselIndex]->SetScalarParameterValue(ParameterNameTargetAlpha, 1);
			}
		}

		//Then process any decimals in health, as well as set the final heart anim.
		if (FMath::Frac(CurrentHealth) != 0.f && HealthVesselMaterials.IsValidIndex(FMath::FloorToInt(CurrentHealth) + 1))
		{	
			HealthVesselMaterials[FMath::FloorToInt(CurrentHealth) + 1]->SetScalarParameterValue(ParameterNameTargetAlpha, FMath::Frac(CurrentHealth));
			HealthVesselMaterials[FMath::FloorToInt(CurrentHealth) + 1]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 1);
		}
		else
		{
			HealthVesselMaterials[FMath::FloorToInt(CurrentHealth)]->SetScalarParameterValue(ParameterNameFinalHeartAlpha, 1);
		}
	}
}

void UPA_HudElements_Status::UpdateHealth(float DeltaTime)
{

}

//--------------------------------------------------------------------------------------------------------------------------------
//  FULLSCREEN TRANSITION
//--------------------------------------------------------------------------------------------------------------------------------

//void UPA_HudElements_Status::StartOpenFadeIn()
//{
//	if (GetWorld())
//	{
//		//Set up latent info
//		FLatentActionInfo LatentInfo;
//		LatentInfo.UUID = 1;
//		LatentInfo.Linkage = 0;
//		LatentInfo.ExecutionFunction = FName(TEXT("FinishFadeInCallback"));
//		LatentInfo.CallbackTarget = this;
//
//		FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
//		FInterpolateFloat* Action = LatentActionManager.FindExistingAction<FInterpolateFloat>(LatentInfo.CallbackTarget, LatentInfo.UUID);
//
//		if (Action == NULL)
//		{
//			//TODO: add something here for options sake that either speeds up the process or skips it; for speedrunners
//			Action = new FInterpolateFloat(FadeDuration, LatentInfo, this);
//
//			Action->TargetObject = this;
//			Action->InitialValue = 0.f;
//			Action->TargetValue = 1.f;
//			Action->UpdateFunction = FName(TEXT("SetFade"));
//
//			//first make sure our Fullscreen fade is turned on and invisible
//			FullScreenFadeImage->SetVisibility(ESlateVisibility::Visible);
//			FullScreenFadeImage->SetOpacity(0.f);
//
//			//then start the animation
//			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
//
//			//UE_LOG(AnyLog, Warning, TEXT("STARTED FADING IN"));
//
//			return;
//		}
//	}
//
//	UE_LOG(AnyLog, Warning, TEXT("NO WORLD"));
//}
//
//void UPA_HudElements_Status::StartCloseFadeIn()
//{
//	if (GetWorld())
//	{
//		//Set up latent info
//		FLatentActionInfo LatentInfo;
//		LatentInfo.UUID = 1;
//		LatentInfo.Linkage = 0;
//		LatentInfo.ExecutionFunction = FName(TEXT("FinishFadeOutCallback"));
//		LatentInfo.CallbackTarget = this;
//
//		FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
//		FInterpolateFloat* Action = LatentActionManager.FindExistingAction<FInterpolateFloat>(LatentInfo.CallbackTarget, LatentInfo.UUID);
//
//		if (Action == NULL)
//		{
//			//TODO: add something here for options sake that either speeds up the process or skips it; for speedrunners
//			Action = new FInterpolateFloat(FadeDuration, LatentInfo, this);
//
//			Action->TargetObject = this;
//			Action->InitialValue = 0.f;
//			Action->TargetValue = 1.f;
//			Action->UpdateFunction = FName(TEXT("SetFade"));
//
//			//first make sure our Fullscreen fade is turned on and invisible
//			FullScreenFadeImage->SetVisibility(ESlateVisibility::Visible);
//			FullScreenFadeImage->SetOpacity(0.f);
//
//			//then start the animation
//			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
//
//			//UE_LOG(AnyLog, Warning, TEXT("STARTED FADING IN"));
//
//			return;
//		}
//	}
//
//	UE_LOG(AnyLog, Warning, TEXT("NO WORLD"));
//}
//
//void UPA_HudElements_Status::StartOpenFadeOut()
//{
//	if (GetWorld())
//	{
//		//Set up latent info
//		FLatentActionInfo LatentInfo;
//		LatentInfo.UUID = 1;
//		LatentInfo.Linkage = 0;
//		LatentInfo.ExecutionFunction = FName(TEXT("FinalizeOpenFadeCallback"));
//		LatentInfo.CallbackTarget = this;
//
//		FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
//		FInterpolateFloat* Action = LatentActionManager.FindExistingAction<FInterpolateFloat>(LatentInfo.CallbackTarget, LatentInfo.UUID);
//
//		if (Action == NULL)
//		{
//			//TODO: add something here for options sake that either speeds up the process or skips it; for speedrunners
//			Action = new FInterpolateFloat(FadeDuration, LatentInfo, this);
//
//			Action->TargetObject = this;
//			Action->InitialValue = 1.f;
//			Action->TargetValue = 0.f;
//			Action->UpdateFunction = FName(TEXT("SetFadeTest"));
//
//			//Start the animation
//			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
//
//			//UE_LOG(AnyLog, Warning, TEXT("STARTED FADING OUT"));
//
//			return;
//		}
//	}
//
//	UE_LOG(AnyLog, Warning, TEXT("NO WORLD"));
//}
//
//void UPA_HudElements_Status::StartCloseFadeOut()
//{
//	if (GetWorld())
//	{
//		//Set up latent info
//		FLatentActionInfo LatentInfo;
//		LatentInfo.UUID = 1;
//		LatentInfo.Linkage = 0;
//		LatentInfo.ExecutionFunction = FName(TEXT("FinalizeCloseFadeCallback"));
//		LatentInfo.CallbackTarget = this;
//
//		FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
//		FInterpolateFloat* Action = LatentActionManager.FindExistingAction<FInterpolateFloat>(LatentInfo.CallbackTarget, LatentInfo.UUID);
//
//		if (Action == NULL)
//		{
//			//TODO: add something here for options sake that either speeds up the process or skips it; for speedrunners
//			Action = new FInterpolateFloat(FadeDuration, LatentInfo, this);
//
//			Action->TargetObject = this;
//			Action->InitialValue = 1.f;
//			Action->TargetValue = 0.f;
//			Action->UpdateFunction = FName(TEXT("SetFadeTest"));
//
//			//Start the animation
//			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
//
//			//UE_LOG(AnyLog, Warning, TEXT("STARTED FADING OUT"));
//
//			return;
//		}
//	}
//
//	//UE_LOG(AnyLog, Warning, TEXT("NO WORLD"));
//}
//
//void UPA_HudElements_Status::SetFade(float NewValue)
//{
//	//UE_LOG(AnyLog, Warning, TEXT("FADING IN AT %f"), NewValue);
//
//	if (FullScreenFadeImage)
//		FullScreenFadeImage->SetOpacity(NewValue);
//}
//
//void UPA_HudElements_Status::SetFadeTest(float NewValue)
//{
//	//UE_LOG(AnyLog, Warning, TEXT("FADING OUT AT %f"), NewValue);
//
//	if (FullScreenFadeImage)
//		FullScreenFadeImage->SetOpacity(NewValue);
//}
//
//void UPA_HudElements_Status::FinishFadeInCallback()
//{
//	SetElementsToInvisible();
//
//	//if (OwnerHUD && OwnerHUD->IngameMenuInstance)
//	//{
//	//	//OwnerHUD->IngameMenuInstance->FinalizeLoadBookUI();
//	//	StartOpenFadeOut();
//	//}
//
//	//VISIBLE/INVISIBLE METHOD
//	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	//if (Character)
//	//{
//	//	Character->FinalizeLoadBookUI();
//	//	StartOpenFadeOut();
//	//}
//
//	////LOAD/UNLOAD METHOD
//	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	//if (Character)
//	//{
//	//	Character->bIsFading = false;
//
//	//	if (Character->CanProceedWithMainMenuAnimation())
//	//	{
//	//		switch (Character->CurrentLoadType)
//	//		{
//	//		case ELoadBookUIType::BOOK_OpenMainMenu:
//	//			Character->OpenMainMenuDelegate.Broadcast();
//	//			break;
//
//	//		case ELoadBookUIType::BOOK_OpenIngameMenu:
//	//			Character->OpenIngameMenuDelegate.Broadcast();
//	//			break;
//	//		}
//
//	//		Character->FinalizeLoadBookUI();
//	//		StartOpenFadeOut();
//	//	}
//	//	else
//	//	{
//	//		//create loading icon here and set it to invisible in the StartOpenFadeout function
//	//	}
//	//}
//}
//
//void UPA_HudElements_Status::FinishFadeOutCallback()
//{
//	SetElementsToVisible();
//
//	//if (OwnerHUD && OwnerHUD->IngameMenuInstance)
//	//{
//	//	//OwnerHUD->IngameMenuInstance->UnloadBookUI();
//	//	//StartCloseFadeOut();
//	//}
//
//	////VISIBLE/INVISIBLE METHOD
//	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	//if (Character)
//	//{
//	//	Character->UnloadBookUI();
//	//	StartCloseFadeOut();
//	//}
//
//	////LOAD/UNLOAD METHOD
//	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	//if (Character)
//	//{
//	//	Character->bIsFading = false;
//
//	//	if (Character->CanProceedWithMainMenuAnimation())
//	//	{
//	//		Character->UnloadBookUI();
//	//		StartCloseFadeOut();
//	//	}
//	//	else
//	//	{
//	//		//create loading icon here and set it to invisible in the StartCloseFadeout function
//	//	}
//	//}
//}
//
//void UPA_HudElements_Status::FinalizeOpenFadeCallback()
//{
//	FullScreenFadeImage->SetOpacity(0.f);
//	FullScreenFadeImage->SetVisibility(ESlateVisibility::Hidden);
//}
//
//void UPA_HudElements_Status::FinalizeCloseFadeCallback()
//{
//	//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	//if (Character)
//	//	Character->FinalizeUnloadBookUI();
//
//	//if (OwnerHUD && OwnerHUD->IngameMenuInstance)
//	//	OwnerHUD->IngameMenuInstance->FinalizeUnloadBookUI();
//
//	FullScreenFadeImage->SetOpacity(0.f);
//	FullScreenFadeImage->SetVisibility(ESlateVisibility::Hidden);
//}


//--------------------------------------------------------------------------------------------------------------------------------
//  MISC
//--------------------------------------------------------------------------------------------------------------------------------

//used for turning off everything but the fullscreen fade (since the fullscreen fade is there to hide the turning off)
void UPA_HudElements_Status::SetElementsToInvisible()
{
	HealthbarPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UPA_HudElements_Status::SetElementsToVisible()
{
	HealthbarPanel->SetVisibility(ESlateVisibility::Visible);
}