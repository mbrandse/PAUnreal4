// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_HudElements_ContextPopup.h"
#include "PiratesAdventure.h"
#include "WidgetLayoutLibrary.h"
#include "PiratesAdventureCharacter.h"

void UPA_HudElements_ContextPopup::NativeConstruct()
{
	Super::NativeConstruct();

	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
	UWorld* World = GetWorld();

	if (OwnerHUD && World)
	{
		FVector2D PlayerPosition2D;
		//APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(OwnerHUD->PlayerOwner->GetCharacter());

		if (PopupImageMaterialTemplate)
		{
			//first make the elements
			PopupImage = NewObject<UImage>(UImage::StaticClass());
			PopupText = NewObject<UTextBlock>(UTextBlock::StaticClass());
			ChangeText = NewObject<UTextBlock>(UTextBlock::StaticClass());
			if (!PopupImage || !PopupText || !ChangeText)
				return;

			//set up image
			PopupImage->Brush.DrawAs = ESlateBrushDrawType::Image;
			PopupImage->Brush.ImageSize = FVector2D(50.f, 50.f);
			PopupImage->SetBrushFromMaterial(PopupImageMaterialTemplate);
			PopupImageMaterial = PopupImage->GetDynamicMaterial();
			PopupImage->SetVisibility(ESlateVisibility::Hidden);

			PopupImageSlot = RootPanel->AddChildToCanvas(PopupImage);
			PopupImageSlot->SetAutoSize(true);
			PopupImageSlot->SetPosition(FVector2D::ZeroVector);
			PopupImageSlot->SetZOrder(0);

			//Since shadow opacity don't have separate setters.
			FLinearColor ShadowAlphaZero = TextShadowColorAndOpacity;
			ShadowAlphaZero.A = 0.f;

			//and finally set up the context text
			PopupText->SetColorAndOpacity(TextColorAndOpacity);
			PopupText->SetOpacity(0.f); //since we animate this with the intro loop, we don't need this visible right away. The text color and opacity will be used as a max.
			PopupText->SetShadowOffset(TextShadowOffset);
			PopupText->SetShadowColorAndOpacity(ShadowAlphaZero);
			PopupText->SetJustification(TextJustification);
			PopupText->Font = TextFont;
			PopupText->SetVisibility(ESlateVisibility::Hidden);

			//FText TestHUDText = NSLOCTEXT("TestNameSpace", "TestKey", "TESTING");
			//PopupText->SetText(TestHUDText); //temporary

			PopupTextSlot = RootPanel->AddChildToCanvas(PopupText);
			PopupTextSlot->SetPosition(FVector2D::ZeroVector); //temporary too. Make this more dynamic taking into account the height of the text field. If possible that is.
			PopupTextSlot->SetAutoSize(true);
			PopupTextSlot->SetZOrder(1);

			ChangeText->SetColorAndOpacity(TextColorAndOpacity);
			ChangeText->SetOpacity(0.f);
			ChangeText->SetShadowOffset(TextShadowOffset);
			ChangeText->SetShadowColorAndOpacity(ShadowAlphaZero);
			ChangeText->SetJustification(TextJustification);
			ChangeText->Font = TextFont;
			ChangeText->SetVisibility(ESlateVisibility::Hidden);

			ChangeTextSlot = RootPanel->AddChildToCanvas(ChangeText);
			ChangeTextSlot->SetPosition(FVector2D::ZeroVector);
			ChangeTextSlot->SetAutoSize(true);
			ChangeTextSlot->SetZOrder(2);
		}
	}
}

void UPA_HudElements_ContextPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* World = GetWorld();

	if (OwnerHUD && World)
	{
		if (OwnerHUD->OwnerCharacter
			&& PopupImageSlot
			&& PopupImage->GetVisibility() != ESlateVisibility::Hidden
			&& PopupTextSlot
			&& PopupText->GetVisibility() != ESlateVisibility::Hidden
			&& ChangeTextSlot)
		{
			FVector2D PlayerPosition2D;
			FVector2D TextBoxSize = PopupText->GetDesiredSize();
			FVector PlayerPosition = OwnerHUD->OwnerCharacter->GetActorLocation();
			PlayerPosition.Z += OwnerHUD->OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

			UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(OwnerHUD->PlayerOwner, PlayerPosition, PlayerPosition2D);

			PopupImageSlot->SetPosition(FVector2D(PlayerPosition2D.X - (PopupImageSize.X * 0.5), PlayerPosition2D.Y - (TextBoxSize.Y * 4.f)));
			PopupTextSlot->SetPosition(FVector2D(PlayerPosition2D.X - (TextBoxSize.X * 0.5), PlayerPosition2D.Y - (TextBoxSize.Y * 2.f)));
			ChangeTextSlot->SetPosition(FVector2D(PlayerPosition2D.X - (TextBoxSize.X * 0.5), PlayerPosition2D.Y - (TextBoxSize.Y * 2.f)));

			if (HudElementState != EHudElementState::HES_NoLoop)
			{
				FLinearColor ShadowAlphaZero = TextShadowColorAndOpacity;
				ShadowAlphaZero.A = 0.f;
				FLinearColor ShadowAlphaNew = TextShadowColorAndOpacity;

				switch (HudElementState)
				{
				case EHudElementState::HES_StartLoop:
					PopupText->SetOpacity(FMath::FInterpConstantTo(PopupText->ColorAndOpacity.GetSpecifiedColor().A, TextColorAndOpacity.A, InDeltaTime, FadeSpeed));
					ShadowAlphaNew.A = FMath::FInterpConstantTo(PopupText->ShadowColorAndOpacity.A, TextShadowColorAndOpacity.A, InDeltaTime, FadeSpeed);
					PopupText->SetShadowColorAndOpacity(ShadowAlphaNew);

					if (ShadowAlphaNew.A >= TextColorAndOpacity.A)
						HudElementState = EHudElementState::HES_NoLoop;
					break;

				case EHudElementState::HES_StopLoop:
					PopupText->SetOpacity(FMath::FInterpConstantTo(PopupText->ColorAndOpacity.GetSpecifiedColor().A, 0.f, InDeltaTime, FadeSpeed));
					ShadowAlphaNew.A = FMath::FInterpConstantTo(PopupText->ShadowColorAndOpacity.A, 0.f, InDeltaTime, FadeSpeed);
					PopupText->SetShadowColorAndOpacity(ShadowAlphaNew);

					//UE_LOG(AnyLog, Log, TEXT("ANIMATING THE CONTEXT ACTION STOP LOOP"));

					if (ShadowAlphaNew.A <= 0.f)
					{
						HudElementState = EHudElementState::HES_NoLoop;
						PopupText->SetVisibility(ESlateVisibility::Hidden);
						PopupImage->SetVisibility(ESlateVisibility::Hidden);
						PopupText->SetText(FText::GetEmpty());
					}
					break;

				case EHudElementState::HES_ChangeLoop:
					ChangeText->SetOpacity(FMath::FInterpConstantTo(ChangeText->ColorAndOpacity.GetSpecifiedColor().A, 0.f, InDeltaTime, FadeSpeed*2.f));
					ShadowAlphaNew.A = FMath::FInterpConstantTo(ChangeText->ShadowColorAndOpacity.A, 0.f, InDeltaTime, FadeSpeed*2.f);
					ChangeText->SetShadowColorAndOpacity(ShadowAlphaNew);

					PopupText->SetOpacity(FMath::FInterpConstantTo(PopupText->ColorAndOpacity.GetSpecifiedColor().A, TextColorAndOpacity.A, InDeltaTime, FadeSpeed*2.f));
					ShadowAlphaNew.A = FMath::FInterpConstantTo(PopupText->ShadowColorAndOpacity.A, TextShadowColorAndOpacity.A, InDeltaTime, FadeSpeed*2.f);
					PopupText->SetShadowColorAndOpacity(ShadowAlphaNew);

					if (ShadowAlphaNew.A >= TextColorAndOpacity.A)
						HudElementState = EHudElementState::HES_NoLoop;
					break;
				}
			}
		}
	}
}

void UPA_HudElements_ContextPopup::PopulateText(FText NewText, FLinearColor NewColor, bool bShowButton)
{
	if (!NewText.IsEmpty())
	{
		PopupText->SetText(NewText);
		PopupText->SetVisibility(ESlateVisibility::Visible);
		PopupText->SetColorAndOpacity(NewColor);
		PopupImage->SetVisibility(ESlateVisibility::Visible);

		if (!bShowButton)
		{
			PopupImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
		}
		else
		{
			PopupImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}
		HudElementState = EHudElementState::HES_StartLoop;
	}
}

void UPA_HudElements_ContextPopup::ReplaceText(FText NewText, FLinearColor NewColor, bool bShowButton)
{
	//UE_LOG(AnyLog, Log, TEXT("REPLACETEXT CALLED"));

	if (!NewText.IsEmpty())
	{
		//if our PopupText is empty to begin with, let's make sure we call the PopulateText instead
		if (PopupText->GetText().IsEmpty())
		{
			PopulateText(NewText, NewColor, bShowButton);
			return;

			////for testing
			//PopupImage->SetVisibility(ESlateVisibility::Visible);
			//FText HUDText = NSLOCTEXT("Test", "Test", "TESTING");
			//PopupText->SetText(HUDText);
			//PopupText->SetColorAndOpacity(TextColorAndOpacity);
			//PopupText->SetShadowColorAndOpacity(TextShadowColorAndOpacity);
		}

		//first set up the change text
		ChangeText->SetText(PopupText->GetText());
		ChangeText->SetColorAndOpacity(PopupText->ColorAndOpacity);
		ChangeText->SetShadowColorAndOpacity(PopupText->ShadowColorAndOpacity);
		ChangeText->SetVisibility(ESlateVisibility::Visible);

		PopupText->SetText(NewText);
		PopupText->SetOpacity(0.f);
		FLinearColor ShadowColor = TextShadowColorAndOpacity;
		ShadowColor.A = 0.f;
		PopupText->SetShadowColorAndOpacity(ShadowColor);
		PopupText->SetVisibility(ESlateVisibility::Visible);

		HudElementState = EHudElementState::HES_ChangeLoop;
	}
}

void UPA_HudElements_ContextPopup::DePopulateText()
{
	HudElementState = EHudElementState::HES_StopLoop;
}