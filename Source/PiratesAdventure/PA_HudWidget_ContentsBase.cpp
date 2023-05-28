// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_HudWidget_ContentsBase.h"
#include "PiratesAdventure.h"
#include "PA_UMG_WrappedTextBlock.h"
#include "PA_HudElements_IngameMenu.h"

void UPA_HudWidget_ContentsBase::ConstructContentsWidget(TArray<FInventoryItem> Items)
{
	if (!OwnerWidget)
		return;

	//UCanvasPanel* RootPanel = Cast<UCanvasPanel>(OwnerWidget->GetRootWidget());
	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());

	if (RootPanel)
	{
		float ScrollerWidth = ((Cellpadding.Left + Cellpadding.Right + MinDesiredCellWidth) * MaxColumnSize) + ScrollerPadding.Right;
		float ContentWindowSize = OwnerWidget->MenuContentContainerSlot->GetSize().X;

		if (Tabs.Num() != 0)
		{
			//if we have tabs, we need a title.
			TitleText = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
			TitleText->SetFont(TitleFont);
			TitleText->SetColorAndOpacity(TitleColorAndOpacity);
			TitleText->SetShadowColorAndOpacity(TitleShadowColorAndOpacity);
			TitleText->SetShadowOffset(TitleShadowOffset);
			TitleText->SetJustification(ETextJustify::Center);

			TitleTextSlot = RootPanel->AddChildToCanvas(TitleText);
			TitleTextSlot->SetSize(FVector2D(ScrollerWidth - ScrollerPadding.Right, TitleHeight));
			TitleTextSlot->SetPosition(FVector2D(ContentWindowSize - ScrollerWidth, ScrollerPadding.Top));
		}

		//Set up the scroller
		Scroller = NewObject<UScrollBox>(UScrollBox::StaticClass());
		if (!Scroller) return;
		Scroller->Orientation = EOrientation::Orient_Vertical;
		Scroller->ScrollBarVisibility = ESlateVisibility::Collapsed;
		Scroller->ConsumeMouseWheel = EConsumeMouseWheel::Never;
		Scroller->ScrollbarThickness = FVector2D::ZeroVector;
		Scroller->WidgetStyle = ScrollerStyle;

		ScrollerSlot = RootPanel->AddChildToCanvas(Scroller);
		if (TitleText != NULL)
		{
			ScrollerSlot->SetPosition(FVector2D(ContentWindowSize - ScrollerWidth, ScrollerPadding.Top + TitleHeight));
			ScrollerSlot->SetSize(FVector2D(ScrollerWidth, OwnerWidget->MenuContentContainerSize.Y - TitleHeight));
		}
		else
		{
			ScrollerSlot->SetPosition(FVector2D(ContentWindowSize - ScrollerWidth, 0));
			ScrollerSlot->SetSize(FVector2D(ScrollerWidth, OwnerWidget->MenuContentContainerSize.Y));
		}

		//Next set up the uniform grid panel in the scroller.
		//ScrollerContent = NewObject<UUniformGridPanel>(UUniformGridPanel::StaticClass());
		//ScrollerContent->SetMinDesiredSlotWidth(MinDesiredCellWidth);
		//ScrollerContent->SetMinDesiredSlotHeight(MinDesiredCellHeight);
		//ScrollerContent->SetSlotPadding(Cellpadding);

		ScrollerContent = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());

		ScrollerContentSlot = Cast<UScrollBoxSlot>(Scroller->AddChild(ScrollerContent));
		if (!ScrollerContentSlot) return;
		ScrollerContentSlot->SetHorizontalAlignment(HAlign_Right);
		ScrollerContentSlot->SetPadding(ScrollerPadding);
	}
}

//--------------------------------------------------------------------------------------------
//    POPULATION AND NAVIGATION FUNCTIONALITY
//--------------------------------------------------------------------------------------------

FInventoryItem UPA_HudWidget_ContentsBase::GetItemFromIndex(int32 ItemIndex) const
{
	FInventoryItem NewItem;

	return NewItem;
}

int32 UPA_HudWidget_ContentsBase::GetMaxItemIndex() const
{
	return INDEX_NONE;
}

void UPA_HudWidget_ContentsBase::GetDataFromTab(int32 TabIndex)
{
	//do something
}

int32 UPA_HudWidget_ContentsBase::GetMaxTabIndex() const
{
	return INDEX_NONE;
}