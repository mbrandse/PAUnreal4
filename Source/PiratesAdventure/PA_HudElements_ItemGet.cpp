// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_HudElements_ItemGet.h"
#include "PiratesAdventure.h"
#include "PA_UMG_WrappedTextBlock.h"
#include "WidgetLayoutLibrary.h"
#include "PA_Asset_Item.h"

void UPA_HudElements_ItemGet::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPA_HudElements_ItemGet::ConstructItemPopup(class UPA_Asset_Item* NewItemData)
{
	ItemData = NewItemData;
	if (!ItemData)
		return;

	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());

	if (RootPanel)
	{
		float BasicIconOffset = ItemData->ItemData.Icon ? 0.f : IconBGSize.X * 0.5f;

		if (!ItemData->ItemData.Icon)
		{
			UE_LOG(AnyLog, Log, TEXT("NO ICON DETECTED"));
		}
		else
		{
			UE_LOG(AnyLog, Log, TEXT("ICON DETECTED"));
		}

		//First setup the main panels; only spawn icon slot when we have an item and icon to display
		MainPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		PopupBGPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		IconBGPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());

		if (!MainPanel)
			return;

		MainPanel->SetRenderTransformPivot(FVector2D(0.5, 0.5));
		MainPanel->SetRenderScale(OverallWindowScale);

		MainPanelSlot = RootPanel->AddChildToCanvas(MainPanel); //we set location during play, since in the construction we cannot get viewport size
		MainPanelSlot->SetSize(FVector2D(PopupBGSize.X + BasicIconOffset, PopupBGSize.Y));
		MainPanelSlot->SetAutoSize(true);

		IconBGPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		IconBGPanel->SetRenderTransformPivot(FVector2D(0.5, 0.5));
		IconBGPanel->SetRenderScale(FVector2D::ZeroVector);
		UCanvasPanelSlot* IconBGPanelSlot = MainPanel->AddChildToCanvas(IconBGPanel);
		IconBGPanelSlot->SetSize(FVector2D(IconBGSize.X, PopupBGSize.Y));
		IconBGPanelSlot->SetZOrder(1);

		PopupBGPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		PopupBGPanel->SetRenderTransformPivot(FVector2D(0.5, 0.5));
		PopupBGPanel->SetRenderScale(FVector2D::ZeroVector);
		UCanvasPanelSlot* PopupBGPanelSlot = MainPanel->AddChildToCanvas(PopupBGPanel);
		PopupBGPanelSlot->SetSize(FVector2D(PopupBGSize.X, PopupBGSize.Y));
		PopupBGPanelSlot->SetPosition(FVector2D(IconBGSize.X * 0.5, 0));
		PopupBGPanelSlot->SetZOrder(0);

		if (IconBGMaterialTemplate)
		{
			UImage* IconBGImage = NewObject<UImage>(UImage::StaticClass());
			//IconBGImage->SetColorAndOpacity(FLinearColor(1, 0, 0, 1));
			IconBGImage->SetBrushFromMaterial(IconBGMaterialTemplate);
			UCanvasPanelSlot* IconBGImageSlot = IconBGPanel->AddChildToCanvas(IconBGImage);
			IconBGImageSlot->SetSize(FVector2D(IconBGSize.X, IconBGSize.Y));
			IconBGImageSlot->SetPosition(FVector2D(0, (PopupBGSize.Y - IconBGSize.Y) * 0.5));
			IconBGImageSlot->SetZOrder(0);
		}

		if (ItemData->ItemData.Icon)
		{
			UImage* IconImage = NewObject<UImage>(UImage::StaticClass());
			//IconImage->SetColorAndOpacity(FLinearColor(1, 0, 0, 1));
			IconImage->SetBrushFromMaterial(ItemData->ItemData.Icon);
			//UMaterialInstanceDynamic* IconMat = IconImage->GetDynamicMaterial();
			//IconMat->SetScalarParameterValue(ItemData->ItemData.IconX, ItemData->ItemData.IconCoordinates.X);
			//IconMat->SetScalarParameterValue(ItemData->ItemData.IconY, ItemData->ItemData.IconCoordinates.Y);
			UCanvasPanelSlot* IconImageSlot = IconBGPanel->AddChildToCanvas(IconImage);
			FVector2D IconSize = FVector2D(32.f, 32.f);// FVector2D(ItemData->ItemData.IconSize, ItemData->ItemData.IconSize);
			//IconImageSlot->SetSize(IconSize);
			IconImageSlot->SetPosition(FVector2D((IconBGSize.X - IconSize.X) * 0.5, (PopupBGSize.Y - IconSize.Y) * 0.5));
			IconImageSlot->SetZOrder(1);
		}

		if (PopupBGMaterialTemplate)
		{
			UImage* PopupBGImage = NewObject<UImage>(UImage::StaticClass());
			//PopupBGImage->SetColorAndOpacity(FLinearColor(0, 1, 0, 1));
			PopupBGImage->SetBrushFromMaterial(PopupBGMaterialTemplate);
			UCanvasPanelSlot* PopupBGImageSlot = PopupBGPanel->AddChildToCanvas(PopupBGImage);
			PopupBGImageSlot->SetSize(FVector2D(PopupBGSize.X, PopupBGSize.Y));
			PopupBGImageSlot->SetZOrder(0);
		}

		//Title text
		UPA_UMG_WrappedTextBlock* TitleText = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
		TitleText->SetColorAndOpacity(TitleTextColorAndOpacity);
		TitleText->SetShadowOffset(TitleTextShadowOffset);
		TitleText->SetShadowColorAndOpacity(TitleTextShadowColorAndOpacity);
		TitleText->SetJustification(TitleTextJustification);
		TitleText->Font = TitleTextFont;
		TitleText->SetText(ItemData->ItemData.NameText);
		UCanvasPanelSlot* TitleTextSlot = PopupBGPanel->AddChildToCanvas(TitleText);
		TitleTextSlot->SetPosition(FVector2D(TextLeftMargin, TitleTextTopMargin));
		TitleTextSlot->SetSize(TitleTextSize);
		TitleTextSlot->SetZOrder(1);

		//Flavor text
		UPA_UMG_WrappedTextBlock* FlavorText = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
		FlavorText->SetColorAndOpacity(FlavorTextColorAndOpacity);
		FlavorText->SetShadowOffset(FlavorTextShadowOffset);
		FlavorText->SetShadowColorAndOpacity(FlavorTextShadowColorAndOpacity);
		FlavorText->SetJustification(FlavorTextJustification);
		FlavorText->Font = FlavorTextFont;
		FlavorText->SetText(ItemData->ItemData.SupportTextSmall);
		UCanvasPanelSlot* FlavorTextSlot = PopupBGPanel->AddChildToCanvas(FlavorText);
		FlavorTextSlot->SetPosition(FVector2D(TextLeftMargin, FlavorTextTopMargin));
		FlavorTextSlot->SetSize(FlavorTextSize);
		FlavorTextSlot->SetZOrder(2);

		//create divider
		UImage* DividerImage = NewObject<UImage>(UImage::StaticClass());
		DividerImage->SetColorAndOpacity(FLinearColor(0, 0, 0, 1));
		UCanvasPanelSlot* DividerImageSlot = PopupBGPanel->AddChildToCanvas(DividerImage);
		DividerImageSlot->SetPosition(FVector2D(TextLeftMargin, DividerTopMargin));
		DividerImageSlot->SetSize(DividerSize);
		DividerImageSlot->SetZOrder(3);

		bInitialized = false;
		HudElementState = EHudElementState::HES_StartLoop;
	}
}

void UPA_HudElements_ItemGet::DeconstructItemPopup()
{
	HudElementState = EHudElementState::HES_StopLoop;
}

void UPA_HudElements_ItemGet::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UWorld* World = GetWorld();

	//first make sure we actually have the position set.
	if (MainPanel
		&& World 
		&& GEngine
		&& !bInitialized)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		if (!PC) return;

		FVector2D ScreenRes;
		int32 ResX, ResY;
		GEngine->GameViewport->GetViewportSize(ScreenRes);
		PC->GetViewportSize(ResX, ResY);
		FVector2D PanelSlotSize = MainPanelSlot->GetSize();
		MainPanelSlot->SetPosition(FVector2D((float)ResX * 0.5, PopupScreenMargin.Top));

		

		bInitialized = true;
	}
	
	//then animate the windows
	float BGMinTime, BGMaxTime, IconMinTime, IconMaxTime;
	switch (HudElementState)
	{
	case EHudElementState::HES_StartLoop:
		if (MainPanel)
		{
			//First animate the Popup BG
			if (!PopupBGAnimationIntroCurve
				|| !IconBGAnimationIntroCurve)
			{
				BGStateProgress = FMath::Clamp(BGStateProgress + InDeltaTime, 0.f, 1.f);
				PopupBGPanel->SetRenderScale(FMath::Lerp(FVector2D::ZeroVector, FVector2D(1,1), BGStateProgress));
				IconBGPanel->SetRenderScale(FMath::Lerp(FVector2D::ZeroVector, FVector2D(1, 1), BGStateProgress));
				
				if (BGStateProgress >= 1.f)
				{
					BGStateProgress = 0.f;
					IconStateProgress = 0.f;
					HudElementState = EHudElementState::HES_NoLoop;
				}
			}
			else
			{
				PopupBGAnimationIntroCurve->GetTimeRange(BGMinTime, BGMaxTime);
				IconBGAnimationIntroCurve->GetTimeRange(IconMinTime, IconMaxTime);

				float AbsoluteMax = FMath::Max(BGMaxTime, IconMaxTime);

				BGStateProgress = FMath::Clamp(BGStateProgress + InDeltaTime, BGMinTime, AbsoluteMax);
				PopupBGPanel->SetRenderScale(FVector2D(PopupBGAnimationIntroCurve->GetFloatValue(BGStateProgress), PopupBGAnimationIntroCurve->GetFloatValue(BGStateProgress)));
				IconStateProgress = FMath::Clamp(BGStateProgress + InDeltaTime, IconMinTime, AbsoluteMax);
				IconBGPanel->SetRenderScale(FVector2D(IconBGAnimationIntroCurve->GetFloatValue(IconStateProgress), IconBGAnimationIntroCurve->GetFloatValue(IconStateProgress)));

				if (BGStateProgress >= AbsoluteMax)
				{
					BGStateProgress = 0.f;
					IconStateProgress = 0.f;
					HudElementState = EHudElementState::HES_NoLoop;
				}
			}
		}
		break;

	case EHudElementState::HES_StopLoop:
		if (MainPanel)
		{
			if (!PopupBGAnimationOuttroCurve
				|| !IconBGAnimationOuttroCurve)
			{
				BGStateProgress = FMath::Clamp(BGStateProgress + InDeltaTime, 0.f, 1.f);
				PopupBGPanel->SetRenderScale(FMath::Lerp(FVector2D::ZeroVector, FVector2D(1, 1), 1.f - BGStateProgress));
				IconBGPanel->SetRenderScale(FMath::Lerp(FVector2D::ZeroVector, FVector2D(1, 1), 1.f - BGStateProgress));

				if (BGStateProgress >= 1.f)
				{
					BGStateProgress = 0.f;
					IconStateProgress = 0.f;
					RemoveFromParent();
				}
			}
			else
			{
				PopupBGAnimationOuttroCurve->GetTimeRange(BGMinTime, BGMaxTime);
				IconBGAnimationOuttroCurve->GetTimeRange(IconMinTime, IconMaxTime);

				float AbsoluteMax = FMath::Max(BGMaxTime, IconMaxTime);

				BGStateProgress = FMath::Clamp(BGStateProgress + InDeltaTime, BGMinTime, AbsoluteMax);
				PopupBGPanel->SetRenderScale(FVector2D(PopupBGAnimationOuttroCurve->GetFloatValue(BGStateProgress), PopupBGAnimationOuttroCurve->GetFloatValue(BGStateProgress)));
				IconStateProgress = FMath::Clamp(BGStateProgress + InDeltaTime, IconMinTime, AbsoluteMax);
				IconBGPanel->SetRenderScale(FVector2D(IconBGAnimationOuttroCurve->GetFloatValue(IconStateProgress), IconBGAnimationOuttroCurve->GetFloatValue(IconStateProgress)));

				if (BGStateProgress >= AbsoluteMax)
				{
					BGStateProgress = 0.f;
					IconStateProgress = 0.f;
					RemoveFromParent();
				}
			}
		}
		break;
	}
}


