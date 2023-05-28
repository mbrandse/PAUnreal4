// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_HudElements_IngameMenu.h"
#include "PiratesAdventure.h"
#include "Components/RetainerBox.h"
#include "PiratesAdventureCharacter.h"
#include "PA_UMG_WrappedTextBlock.h"
#include "PA_HudWidget_ContentsBase.h"
#include "PA_ISMA_InteractiveBook.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "PA_HudElements_Status.h"
#include "PA_HudWidget_InventoryItem.h"

void UPA_HudElements_IngameMenu::NativeConstruct()
{
	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
	UWorld* World = GetWorld();

	if (RootPanel && World)
	{
		//First set up the menu item panel and buttons.
		if (MenuItems.Num() != 0)
		{
			MenuItemContainer = NewObject<UVerticalBox>(UVerticalBox::StaticClass());
			if (!MenuItemContainer) return;
			MenuItemContainer->SetRenderTransformPivot(FVector2D::ZeroVector);
			MenuItemContainer->SetVisibility(ESlateVisibility::Visible);

			MenuItemContainerSlot = RootPanel->AddChildToCanvas(MenuItemContainer);
			MenuItemContainerSlot->SetSize(MenuItemContainerSize);
			MenuItemContainerSlot->SetZOrder(20);

			int32 ItemCount = MenuItems.Num();
			float TopPadding = (MenuItemContainerSize.Y - (ItemCount * MenuItemSize.Y)) / (ItemCount + 1);
			float LeftPadding = (MenuItemContainerSize.X - MenuItemSize.X) / 2;

			for (int32 ItemIndex = 0; ItemIndex < ItemCount; ItemIndex++)
			{
				//TODO: FILL IN WITH FINAL BUTTONS. RIGHT NOW IT'S JUST AN IMAGE AND SOME TEXT.
				//first make the elements
				MenuItems[ItemIndex].Container = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
				MenuItems[ItemIndex].Background = NewObject<UImage>(UImage::StaticClass());
				MenuItems[ItemIndex].TextBlock = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
				if (!MenuItems[ItemIndex].Container || !MenuItems[ItemIndex].Background || !MenuItems[ItemIndex].TextBlock) return;

				MenuItems[ItemIndex].ContainerSlot = MenuItemContainer->AddChildToVerticalBox(MenuItems[ItemIndex].Container);
				MenuItems[ItemIndex].ContainerSlot->SetPadding(FMargin(LeftPadding, TopPadding, 0.f, 0.f));
				MenuItems[ItemIndex].ContainerSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
				MenuItems[ItemIndex].ContainerSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

				//set up image
				MenuItems[ItemIndex].Background->Brush.DrawAs = ESlateBrushDrawType::Image;
				MenuItems[ItemIndex].Background->Brush.ImageSize = MenuItemSize;
				//Add material later
				//MenuItems[ItemIndex].Background->SetBrushFromMaterial(/*Material*/);

				MenuItems[ItemIndex].BackgroundSlot = MenuItems[ItemIndex].Container->AddChildToCanvas(MenuItems[ItemIndex].Background);
				MenuItems[ItemIndex].BackgroundSlot->SetSize(MenuItemSize);
				MenuItems[ItemIndex].BackgroundSlot->SetZOrder(0);

				//and finally set up the context text
				MenuItems[ItemIndex].TextBlock->SetColorAndOpacity(MenuItemTextNormalStyle.ColorAndOpacity);
				MenuItems[ItemIndex].TextBlock->SetShadowOffset(MenuItemTextNormalStyle.ShadowOffset);
				MenuItems[ItemIndex].TextBlock->SetShadowColorAndOpacity(MenuItemTextNormalStyle.ShadowColorAndOpacity);
				MenuItems[ItemIndex].TextBlock->SetJustification(ETextJustify::Left);
				MenuItems[ItemIndex].TextBlock->SetFont(MenuItemTextNormalStyle.Font);
				MenuItems[ItemIndex].TextBlock->SetText(MenuItems[ItemIndex].Text);
				MenuItems[ItemIndex].TextBlock->SetMargin(MenuItemTextNormalMargin);

				MenuItems[ItemIndex].TextBlockSlot = MenuItems[ItemIndex].Container->AddChildToCanvas(MenuItems[ItemIndex].TextBlock);
				MenuItems[ItemIndex].TextBlockSlot->SetSize(MenuItemSize);
				MenuItems[ItemIndex].TextBlockSlot->SetZOrder(1);
			}

			SetMenuItemIndex(0);
		}

		//Setup the content panel for the content window. 
		MenuContentMask = NewObject<URetainerBox>(URetainerBox::StaticClass());
		UCanvasPanel* MenuContentMaskPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass()); //we need an additional canvaspanel to get renewed access to canvaspanel slots.
		MenuContentContainer = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		if (!MenuContentContainer || !MenuContentMaskPanel || !MenuContentMask) return;

		MenuContentMaskMaterial = UMaterialInstanceDynamic::Create(MenuContentMaskMaterialTemplate, this);
		MenuContentMask->SetEffectMaterial(MenuContentMaskMaterial);
		//MenuContentMaskMaterial = MenuContentMask->GetEffectMaterial();
		MenuContentMask->SetTextureParameter(MenuContentMaskTextureParameter);
		MenuContentMask->PhaseCount = FMath::Max<int32>(MenuContentsRefreshRate, 1); //make sure our PhaseCount is at least 1, so that we actually get refreshed
		MenuContentMask->SetVisibility(ESlateVisibility::Hidden);
		MenuContentMaskSlot = RootPanel->AddChildToCanvas(MenuContentMask);
		MenuContentMaskSlot->SetPosition(FVector2D::ZeroVector);
		MenuContentMaskSlot->SetSize(FVector2D(1920, 1080));
		MenuContentMaskSlot->SetZOrder(5);

		UPanelSlot* MenuContentMaskPanelSlot = MenuContentMask->AddChild(MenuContentMaskPanel);

		MenuContentContainer->SetRenderTransformPivot(FVector2D::ZeroVector);
		MenuContentContainer->SetVisibility(ESlateVisibility::Hidden);
		MenuContentContainerSlot = MenuContentMaskPanel->AddChildToCanvas(MenuContentContainer);
		MenuContentContainerSlot->SetSize(MenuContentContainerSize);
		MenuContentContainerSlot->SetZOrder(0);

		//Set up the scroller
		Scroller = NewObject<UScrollBox>(UScrollBox::StaticClass());
		if (!Scroller) return;
		Scroller->Orientation = EOrientation::Orient_Vertical;
		Scroller->ScrollBarVisibility = ESlateVisibility::Collapsed;
		Scroller->ConsumeMouseWheel = EConsumeMouseWheel::Never;
		Scroller->ScrollbarThickness = FVector2D::ZeroVector;

		ScrollerSlot = MenuContentContainer->AddChildToCanvas(Scroller);
		ScrollerSlot->SetZOrder(1);

		//Setup the uniform grid panel in the scroller.
		//ScrollerContents = NewObject<UUniformGridPanel>(UUniformGridPanel::StaticClass());
		ScrollerContents = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());

		ScrollerContentsSlot = Cast<UScrollBoxSlot>(Scroller->AddChild(ScrollerContents));
		if (!ScrollerContentsSlot) return;
		//ScrollerContentsSlot->SetHorizontalAlignment(HAlign_Right);
		ScrollerContentsSlot->SetHorizontalAlignment(HAlign_Left);

		//Setup the scrollbar elements.
		ScrollbarContainer = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		ScrollbarTopArrow = NewObject<UImage>(UImage::StaticClass());
		ScrollbarBottomArrow = NewObject<UImage>(UImage::StaticClass());
		Scrollbar = NewObject<UImage>(UImage::StaticClass());
		if (!ScrollbarContainer || !ScrollbarTopArrow || !ScrollbarBottomArrow || !Scrollbar) return;

		ScrollbarContainerSlot = MenuContentContainer->AddChildToCanvas(ScrollbarContainer);
		ScrollbarContainerSlot->SetAutoSize(true);
		ScrollbarContainerSlot->SetPosition(FVector2D::ZeroVector);

		ScrollbarTopArrow->Brush.DrawAs = ESlateBrushDrawType::Image;
		if(ScrollbarTopArrowMaterial) ScrollbarTopArrow->SetBrushFromMaterial(ScrollbarTopArrowMaterial);

		ScrollbarTopArrowSlot = ScrollbarContainer->AddChildToCanvas(ScrollbarTopArrow);
		ScrollbarTopArrowSlot->SetAutoSize(false);
		ScrollbarTopArrowSlot->SetSize(ScrollbarArrowSize);
		ScrollbarTopArrowSlot->SetPosition(FVector2D::ZeroVector);

		Scrollbar->Brush.DrawAs = ESlateBrushDrawType::Box;
		if (ScrollbarMaterial) Scrollbar->SetBrushFromMaterial(ScrollbarMaterial);
		Scrollbar->Brush.Margin = ScrollbarMaterialMargin;
		Scrollbar->SetRenderTransformPivot(FVector2D(0.5, 0));

		ScrollbarSlot = ScrollbarContainer->AddChildToCanvas(Scrollbar);
		ScrollbarSlot->SetAutoSize(false);
		ScrollbarSlot->SetSize(FVector2D(ScrollbarArrowSize.X, ScrollbarHeight));
		ScrollbarSlot->SetPosition(FVector2D(0.f, ScrollbarArrowSize.Y));

		ScrollbarBottomArrow->Brush.DrawAs = ESlateBrushDrawType::Image;
		if (ScrollbarBottomArrowMaterial) ScrollbarBottomArrow->SetBrushFromMaterial(ScrollbarBottomArrowMaterial);
		ScrollbarBottomArrow->Brush.ImageSize = ScrollbarArrowSize;

		ScrollbarBottomArrowSlot = ScrollbarContainer->AddChildToCanvas(ScrollbarBottomArrow);
		ScrollbarBottomArrowSlot->SetAutoSize(false);
		ScrollbarBottomArrowSlot->SetSize(ScrollbarArrowSize);
		ScrollbarBottomArrowSlot->SetPosition(FVector2D(0, ScrollbarArrowSize.Y + ScrollbarHeight));

		//Finaly set up the sublevel contents window.
		SublevelContentContainer = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		if (!SublevelContentContainer) return;
		SublevelContentContainer->SetRenderTransformPivot(FVector2D::ZeroVector);
		SublevelContentContainer->SetVisibility(ESlateVisibility::Hidden);
		SublevelContentContainerSlot = RootPanel->AddChildToCanvas(SublevelContentContainer);
		SublevelContentContainerSlot->SetSize(MenuContentContainerSize);
		SublevelContentContainerSlot->SetZOrder(10);

		SublevelIcon = NewObject<UImage>(UImage::StaticClass());
		if (!SublevelIcon) return;
		SublevelIcon->Brush.DrawAs = ESlateBrushDrawType::Image;
		if (SublevelIconMaterial) SublevelIcon->SetBrushFromMaterial(SublevelIconMaterial);
		SublevelIconSlot = SublevelContentContainer->AddChildToCanvas(SublevelIcon);
		SublevelIconSlot->SetSize(SublevelIconSize);
		SublevelIconSlot->SetPosition(FVector2D(SublevelContentContainerMargin.Left, SublevelContentContainerMargin.Top));

		SublevelTitle = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
		if (!SublevelTitle) return;
		SublevelTitle->SetFont(SublevelTitleStyle.Font);
		SublevelTitle->SetColorAndOpacity(SublevelTitleStyle.ColorAndOpacity);
		SublevelTitle->SetShadowColorAndOpacity(SublevelTitleStyle.ShadowColorAndOpacity);
		SublevelTitle->SetShadowOffset(SublevelTitleStyle.ShadowOffset);
		SublevelTitle->SetMargin(SublevelTitleMargin);
		FText PlaceholderTitleText = NSLOCTEXT("Temp", "Temp", "Temporary Title");
		SublevelTitle->SetText(PlaceholderTitleText);
		UCanvasPanelSlot* SublevelTitleSlot = SublevelContentContainer->AddChildToCanvas(SublevelTitle);
		SublevelTitleSlot->SetSize(FVector2D(MenuContentContainerSize.X - SublevelIconSize.X - SublevelContentContainerMargin.Left - SublevelContentContainerMargin.Right - SublevelIconTextSpacing, SublevelTitleSize));
		SublevelTitleSlot->SetPosition(FVector2D(SublevelContentContainerMargin.Left + SublevelIconSize.X + SublevelIconTextSpacing, SublevelContentContainerMargin.Top));

		SublevelScroller = NewObject<UScrollBox>(UScrollBox::StaticClass());
		SublevelScroller->Orientation = EOrientation::Orient_Vertical;
		SublevelScroller->ScrollBarVisibility = ESlateVisibility::Collapsed;
		SublevelScroller->ConsumeMouseWheel = EConsumeMouseWheel::Never;
		SublevelScroller->ScrollbarThickness = FVector2D::ZeroVector;
		SublevelScrollerSlot = SublevelContentContainer->AddChildToCanvas(SublevelScroller);
		SublevelScrollerSlot->SetZOrder(1);
		SublevelScrollerSlot->SetPosition(FVector2D(SublevelContentContainerMargin.Left + SublevelIconSize.X + SublevelIconTextSpacing, SublevelContentContainerMargin.Top + SublevelTitleSize + SublevelIconTextSpacing));
		SublevelScrollerSlot->SetSize(FVector2D(MenuContentContainerSize.X - SublevelIconSize.X - SublevelContentContainerMargin.Left - SublevelContentContainerMargin.Right - SublevelIconTextSpacing, SublevelContentSize));

		SublevelScrollerContents = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
		if (!SublevelScrollerContents) return;
		SublevelScrollerContents->SetFont(SublevelContentStyle.Font);
		SublevelScrollerContents->SetColorAndOpacity(SublevelContentStyle.ColorAndOpacity);
		SublevelScrollerContents->SetShadowColorAndOpacity(SublevelContentStyle.ShadowColorAndOpacity);
		SublevelScrollerContents->SetShadowOffset(SublevelContentStyle.ShadowOffset);
		FText PlaceholderContentText = NSLOCTEXT("Temp2", "Temp2", "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
		SublevelScrollerContents->SetText(PlaceholderContentText);
		SublevelScrollerContentsSlot = Cast<UScrollBoxSlot>(SublevelScroller->AddChild(SublevelScrollerContents));
		if (!SublevelScrollerContentsSlot) return;
		SublevelScrollerContentsSlot->SetHorizontalAlignment(HAlign_Fill);
	}
}

void UPA_HudElements_IngameMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	static int32 Tester = 0;

	if (BookInstance
		&& GetWorld())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			//Set the position, rotation and scale of the Menu Item panel to match the mesh in the Interactive Book mesh.
			if (MenuItemContainer
				&& MenuItemContainer->bIsEnabled)
			{
				if (MenuItemContainer->GetVisibility() == ESlateVisibility::Visible)
				{
					SetWidgetRenderTransform(BookInstance->MenuItemsPlane, MenuItemContainer, MenuItemContainerSize, MenuItemsTopLeftSocket, MenuItemsTopRightSocket);
				}
				else
				{
					MenuItemContainer->SetVisibility(ESlateVisibility::Visible);
				}
			}

			if (MenuContentContainer
				&& MenuContentContainer->bIsEnabled)
			{
				if (MenuContentContainer->GetVisibility() == ESlateVisibility::Visible)
				{
					//TODO: fix this so the sublevel index actually reflects the current index
					SetWidgetRenderTransform(BookInstance->GetSubLevelMesh(0), MenuContentContainer, MenuContentContainerSize, MenuContentsTopLeftSocket, MenuContentsTopRightSocket);

					//Deal with scrolling
					if (Scroller->GetScrollOffset() != ScrollerScrollTarget)
					{
						Scroller->SetScrollOffset(FMath::FInterpTo(Scroller->GetScrollOffset(), ScrollerScrollTarget, InDeltaTime, ScrollerScrollSpeed));
						ScrollbarSlot->SetPosition(FVector2D(0, FMath::FInterpTo(ScrollbarSlot->GetPosition().Y, ScrollerScrollbarTarget, InDeltaTime, ScrollerScrollSpeed * 2)));
					}
				}
			}

			if (SublevelContentContainer
				&& SublevelContentContainer->bIsEnabled)
			{
				if (SublevelContentContainer->GetVisibility() == ESlateVisibility::Visible)
				{
					SetWidgetRenderTransform(BookInstance->GetSubLevelMesh(1), SublevelContentContainer, MenuContentContainerSize, MenuContentsTopLeftSocket, MenuContentsTopRightSocket);

					//TODO: give sublevel scrolling too!
				}
			}

			if (MenuContentMaskMaterial)
			{
				if (bAnimateMask)
				{
					UE_LOG(AnyLog, Log, TEXT("WE HAVE A MATERIAL, SO LET'S HAVE SOME FUN WITH IT."));

					//TODO: for now update every frame, but build in a function that makes sure this gets updated only when the interactive book is animating.
					UStaticMeshComponent* MaskAMesh = BookInstance->GetSubLevelMesh(1);
					UStaticMeshComponent* MaskBMesh = BookInstance->MenuItemsPlane;
					if (!MaskAMesh || !MaskBMesh) return;

					FVector MaskATopLeftPos = MaskAMesh->GetSocketLocation(MenuContentsTopLeftSocket);
					FVector MaskATopRightPos = MaskAMesh->GetSocketLocation(MenuContentsTopRightSocket);
					FVector MaskBTopLeftPos = MaskBMesh->GetSocketLocation(MenuItemsTopLeftSocket);
					FVector MaskBTopRightPos = MaskBMesh->GetSocketLocation(MenuItemsTopRightSocket);

					FVector2D MaskATopLeftProjected, MaskATopRightProjected, MaskBTopLeftProjected, MaskBTopRightProjected, MenuItemsOffset;
					float MaskAAngle, MaskAScale, MaskBAngle, MaskBScale;

					UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, MaskATopLeftPos, MaskATopLeftProjected);
					UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, MaskATopRightPos, MaskATopRightProjected);
					UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, MaskBTopLeftPos, MaskBTopLeftProjected);
					UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, MaskBTopRightPos, MaskBTopRightProjected);

					MaskAScale = (MaskATopLeftProjected - MaskATopRightProjected).Size();
					MaskAAngle = FMath::Atan2((MaskATopRightProjected.Y + 3000) - (MaskATopLeftProjected.Y + 3000), (MaskATopRightProjected.X + 3000) - (MaskATopLeftProjected.X + 3000));
					MaskAAngle *= (180 / PI);

					//The offsets are necessary for the material mask for the menu items, since the menu item field is not square. We need to make it square due to material limitations.
					float MaskBOffsetScale = MenuItemContainerSize.Y / MenuItemContainerSize.X;
					MenuItemsOffset = (MaskBTopRightProjected - MaskBTopLeftProjected) * MaskBOffsetScale;
					MaskBScale = (MaskBTopLeftProjected - MaskBTopRightProjected).Size();
					MaskBAngle = FMath::Atan2((MaskBTopRightProjected.Y + 3000) - (MaskBTopLeftProjected.Y + 3000), (MaskBTopRightProjected.X + 3000) - (MaskBTopLeftProjected.X + 3000));
					MaskBAngle *= (180 / PI);

					MenuContentMaskMaterial->SetScalarParameterValue("MaskA_XPos", MaskATopLeftProjected.X);
					MenuContentMaskMaterial->SetScalarParameterValue("MaskA_YPos", MaskATopLeftProjected.Y);
					MenuContentMaskMaterial->SetScalarParameterValue("MaskA_Scale", 1.f / (MaskAScale / 1920.f)); //since in materials, scale is determined by times (4 means 4 times as small), as well as screenres, not size of widget.
					MenuContentMaskMaterial->SetScalarParameterValue("MaskA_Angle", MaskAAngle);

					MenuContentMaskMaterial->SetScalarParameterValue("MaskB_XPos", MaskBTopRightProjected.X - MenuItemsOffset.X);
					MenuContentMaskMaterial->SetScalarParameterValue("MaskB_YPos", MaskBTopRightProjected.Y - MenuItemsOffset.Y);
					MenuContentMaskMaterial->SetScalarParameterValue("MaskB_Scale", (1.f / (MaskBScale / 1920.f)) / MaskBOffsetScale);
					//UE_LOG(AnyLog, Log, TEXT("CurrentMaskScale %f"), MaskBScale / MenuItemContainerSize.X);
					MenuContentMaskMaterial->SetScalarParameterValue("MaskB_Angle", MaskBAngle);
				}
			}
		}
	}
	else if(MenuItemContainer && MenuItemContainer->GetVisibility() == ESlateVisibility::Visible)
	{
		//Hide elements when we are not using them.
		SetIngameUIVisibility(ESlateVisibility::Hidden);
	}
}

void UPA_HudElements_IngameMenu::SetWidgetRenderTransform(UStaticMeshComponent* Mesh, UWidget* Widget, FVector2D WidgetSize, FName TopLeftSocket, FName TopRightSocket)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC && Mesh && Widget)
	{
		FVector TopLeftPos = Mesh->GetSocketLocation(TopLeftSocket);
		FVector TopRightPos = Mesh->GetSocketLocation(TopRightSocket);
		FVector2D TopLeftProjected, TopRightProjected;
		float NewAngle, NewScale;

		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, TopLeftPos, TopLeftProjected);
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, TopRightPos, TopRightProjected);

		NewScale = (TopLeftProjected - TopRightProjected).Size();
		NewAngle = FMath::Atan2((TopRightProjected.Y + 3000) - (TopLeftProjected.Y + 3000), (TopRightProjected.X + 3000) - (TopLeftProjected.X + 3000));
		NewAngle *= (180 / PI);

		Widget->SetRenderTranslation(TopLeftProjected);
		Widget->SetRenderScale(FVector2D(NewScale / WidgetSize.X, NewScale / WidgetSize.X));
		Widget->SetRenderTransformAngle(NewAngle);
	}
}

void UPA_HudElements_IngameMenu::SetMenuItemsVisibility(ESlateVisibility NewVisibility)
{
	if (MenuItemContainer != NULL)
		MenuItemContainer->SetVisibility(NewVisibility);
}

//---------------------------------------------------------------------------------------------------------------//
// ITEM CONSTRUCTION
//---------------------------------------------------------------------------------------------------------------//

FInventoryItem UPA_HudElements_IngameMenu::ConstructNewInventoryItem(class APA_Item_Base* NewItem)
{
	//for now, we will ignore NewItem and just make a basic white square.

	FInventoryItem NewInventoryItem;

	NewInventoryItem.Container = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass()); 

	NewInventoryItem.Icon = NewObject<UImage>(UImage::StaticClass());
	NewInventoryItem.Icon->Brush.DrawAs = ESlateBrushDrawType::Image;
	NewInventoryItem.IconSlot = NewInventoryItem.Container->AddChildToCanvas(NewInventoryItem.Icon);

	//Font and those kind of variables will be added later. Position and size related variables (as well as the uniform gridslot) will be assigned upon usage in submenus.

	return NewInventoryItem;
}

//---------------------------------------------------------------------------------------------------------------//
// BOOK UI NAVIGATION
//---------------------------------------------------------------------------------------------------------------//

void UPA_HudElements_IngameMenu::ProcessVerticalNavigation(int32 Direction)
{
	switch (CurrentMenuLevel)
	{
	case EMenuLevel::ML_MenuItem:
		SetMenuItemIndex(Direction);
		break;

	case EMenuLevel::ML_MenuContent:
		SetScrollerIndex(Direction * CurrentScroller.MaxRowSize);
		break;

	case EMenuLevel::ML_MenuContentSublevel:
		//just set the scroll here. 
		break;
	}
}

void UPA_HudElements_IngameMenu::ProcessHorizontalNavigation(int32 Direction)
{
	switch (CurrentMenuLevel)
	{
	case EMenuLevel::ML_MenuContent:
		SetScrollerIndex(Direction);
		break;

	case EMenuLevel::ML_MenuContentTabs:
		SetTabIndex(Direction);
		break;
	}
}

void UPA_HudElements_IngameMenu::ProcessConfirm()
{
	switch (CurrentMenuLevel)
	{
	case EMenuLevel::ML_MenuItem:
		SetContentWindow(MenuItems[CurrentMenuItemIndex].MenuType);
		break;

	case EMenuLevel::ML_MenuContent:
		SetContentSublevelWindow(CurrentScrollerIndex);
		break;
	}
}

void UPA_HudElements_IngameMenu::ProcessCancel()
{
	switch (CurrentMenuLevel)
	{
	case EMenuLevel::ML_MenuItem: //make sure this takes into account main menu too.
		CloseIngameMenu();
		break;

	case EMenuLevel::ML_MenuContent:
	case EMenuLevel::ML_MenuContentTabs:
		CurrentMenuLevel = EMenuLevel::ML_MenuItem;
		UnfocusScroller();
		break;

	case EMenuLevel::ML_MenuContentSublevel:
		CurrentMenuLevel = EMenuLevel::ML_MenuContent;
		SetBookinstanceSubLevel(0);
	}
}

void UPA_HudElements_IngameMenu::SetMenuItemIndex(int32 NewIndex)
{
	//TODO: change this when the buttons have been finalized.
	int32 OldIndex = CurrentMenuItemIndex;
	CurrentMenuItemIndex += NewIndex;
	if (CurrentMenuItemIndex >= MenuItems.Num())
	{
		CurrentMenuItemIndex = 0;
	}
	else if (CurrentMenuItemIndex < 0)
	{
		CurrentMenuItemIndex = MenuItems.Num() - 1;
	}

	MenuItems[OldIndex].Background->SetColorAndOpacity(FLinearColor::White);
	MenuItems[CurrentMenuItemIndex].Background->SetColorAndOpacity(FLinearColor::Gray);
}

void UPA_HudElements_IngameMenu::SetScrollerIndex(int32 NewIndex)
{
	int32 OldIndex = CurrentScrollerIndex;
	//AdjustedIndex is used for when we can scroll. When we can scroll, we generate empty items to make sure we can still select all items, 
	//but we cannot have the player select those items. Hence why we cannot use the GetChildrenCount like that, since that one also includes empties. 
	int32 AdjustedIndex = ScrollerContents->GetChildrenCount() <= CurrentScroller.MaxRowSize * 3 ? 0 : CurrentScroller.MaxRowSize;

	CurrentScrollerIndex += NewIndex; 
	UE_LOG(AnyLog, Log, TEXT("OLDINDEX %d NEWINDEX %d"), OldIndex, CurrentScrollerIndex);

	if (CurrentScrollerIndex >= ScrollerContents->GetChildrenCount() - AdjustedIndex
		|| CurrentScrollerIndex < 0)
	{
		CurrentScrollerIndex = OldIndex;
		return;
	}

	UPA_HudWidget_InventoryItem* OldItem = NULL;
	UPA_HudWidget_InventoryItem* NewItem = NULL;

	UWidget* OldWidget = ScrollerContents->GetChildAt(OldIndex);
	if (OldWidget) OldItem = Cast<UPA_HudWidget_InventoryItem>(OldWidget);

	UWidget* NewWidget = ScrollerContents->GetChildAt(CurrentScrollerIndex);
	if (OldWidget) NewItem = Cast<UPA_HudWidget_InventoryItem>(NewWidget);

	if (OldItem && NewItem)
	{
		OldItem->SetInventoryItemState(EInventoryItemState::Normal);
		NewItem->SetInventoryItemState(EInventoryItemState::Rollover);
	}

	SetTargetScrollOffset(CurrentScrollerIndex);
}

void UPA_HudElements_IngameMenu::UnfocusScroller()
{
	//TODO: make sure this gets turned into valid inventory item widgets.
	UWidget* CurrentWidget = ScrollerContents->GetChildAt(CurrentScrollerIndex);
	if (CurrentWidget)
	{
		UPA_HudWidget_InventoryItem* CurrentItem = Cast<UPA_HudWidget_InventoryItem>(CurrentWidget);
		if (CurrentItem)
			CurrentItem->SetInventoryItemState(EInventoryItemState::Normal);
	}

	//TODO; MAYBE ADD AN OPTION THAT "REMEMBERS" THE POSITION OF THE SCROLLER INDEX, BY SIMPLY NOT RESETTING IT TO 0.
	//MAY CAUSE PROBLEMS WITH OTHER MENUS THOUGH.
	CurrentScrollerIndex = 0;

	SetTargetScrollOffset(CurrentScrollerIndex);
}

void UPA_HudElements_IngameMenu::SetTabIndex(int32 NewIndex)
{

}

void UPA_HudElements_IngameMenu::SetContentWindow(EMenuType NewContentType)
{
	TArray<APA_Item_Base*> TempArray;
	TempArray.Empty();

	switch (NewContentType)
	{
		case EMenuType::MT_Back:
			CloseIngameMenu();
			break;

		case EMenuType::MT_Item:
			//TODO: Add item specific inventory array here
			ResizeScroller(ItemContentsScroller); 
			PopulateInventory(TempArray);
			ResizeScrollbar();
			ScrollerScrollTarget = 0;
			SetFeedback(CurrentScroller.ContentFeedbacks, CurrentScroller.SublevelFeedbacks);
			SetScrollerIndex(0);
			SetBookinstanceSubLevel(0);
			CurrentMenuLevel = EMenuLevel::ML_MenuContent;
			break;
	}
}

void UPA_HudElements_IngameMenu::SetContentSublevelWindow(int32 ItemIndex)
{
	//TODO: Set content info window from itemdata, using ItemIndex.
	CurrentMenuLevel = EMenuLevel::ML_MenuContentSublevel;
	SetBookinstanceSubLevel(1);
}


void UPA_HudElements_IngameMenu::SetBookinstanceSubLevel(int32 SubLevelIndex)
{
	if (BookInstance)
	{
		BookInstance->OpenSubLevel(SubLevelIndex);
	}
}

//---------------------------------------------------------------------------------------------------------------//
// UPDATE SCROLLER
//---------------------------------------------------------------------------------------------------------------//

void UPA_HudElements_IngameMenu::ResizeScroller(FMenuScrollerData ScrollerData)
{
	if (ScrollerSlot && ScrollerContents && ScrollerContentsSlot)
	{
		//TODO: This doesn't take into account the title text OR scrollerpadding yet. Fix it.
		//float NewScrollerWidth = ((ScrollerData.Cellpadding.Left + ScrollerData.Cellpadding.Right + InventoryItemSize/*ScrollerData.MinDesiredCellWidth*/) * ScrollerData.MaxRowSize) + ScrollerData.ScrollerPadding.Right;
		//NewScrollerWidth = NewScrollerWidth > MenuContentContainerSize.X ? MenuContentContainerSize.X : NewScrollerWidth; //make sure it doesn't exceed the size of the main box.

		//ScrollerSlot->SetPosition(FVector2D(MenuContentContainerSize.X - NewScrollerWidth, 0.f));
		//ScrollerSlot->SetSize(FVector2D(NewScrollerWidth, MenuContentContainerSize.Y));

		float ScrollContentsOffset = MenuContentContainerSize.X - 200.f;
		//The position also sets the location for the scrollbar
		ScrollerSlot->SetPosition(FVector2D(200.f, ScrollerData.ScrollerPadding.Top)); //TODO: SHOULD WE SET THIS MORE DYNAMICALLY, OR CAN WE SIMPLY KEEP IT AT 200 FOR EVERYTHING?
		ScrollerSlot->SetSize(FVector2D(ScrollContentsOffset - ScrollerData.ScrollerPadding.Right, MenuContentContainerSize.Y));

		//ScrollerContents->SetMinDesiredSlotWidth(ScrollerData.MinDesiredCellWidth);
		//ScrollerContents->SetMinDesiredSlotHeight(ScrollerData.MinDesiredCellHeight);
		//ScrollerContents->SetSlotPadding(ScrollerData.Cellpadding);

		ScrollerContentsSlot->SetPadding(ScrollerData.ScrollerPadding);
		
		//first set up the max row size, for use with inventory population and the likes
		ScrollerData.MaxRowSize = FMath::FloorToInt((ScrollContentsOffset - ScrollerData.ScrollerPadding.Left - ScrollerData.ScrollerPadding.Right) / (ScrollerData.Cellpadding.Right + InventoryItemSize));
		//then, depending on the item height, set a max column size as well
		ScrollerData.MaxColumnSize = FMath::CeilToInt((MenuContentContainerSize.Y * 0.5) / (InventoryItemSize + ScrollerData.Cellpadding.Bottom));

		CurrentScroller = ScrollerData;
	}
}

void UPA_HudElements_IngameMenu::ResizeScrollbar()
{
	if (ScrollbarContainerSlot && ScrollerSlot)
	{
		//ScrollbarContainerSlot->SetPosition(FVector2D(ScrollerSlot->GetPosition().X - ScrollbarPadding.X, ScrollerSlot->GetPosition().Y + ScrollbarPadding.Y));
		//int32 FullScrollerRowCount = FMath::CeilToInt(((float)ScrollerContents->GetChildrenCount() - (float)CurrentScroller.MaxRowSize) / (float)CurrentScroller.MaxRowSize);
		//float FullScrollerContentSize = FullScrollerRowCount * (CurrentScroller.Cellpadding.Bottom + InventoryItemSize);
		//
		//if (FullScrollerContentSize >= MenuItemContainerSize.Y * 0.5)
		//{
		//	ScrollbarContainer->SetVisibility(ESlateVisibility::Hidden);
		//}
		//else
		//{
		//	Scrollbar->SetRenderScale(FVector2D(1.f, (float)3 / FullScrollerContentSize));
		//}

		ScrollbarContainerSlot->SetPosition(FVector2D(ScrollerSlot->GetPosition().X - ScrollbarPadding.X, ScrollerSlot->GetPosition().Y + ScrollbarPadding.Y));
		int32 FullScrollerColumnSize = FMath::CeilToInt(((float)InventoryItems.Num() - (float)CurrentScroller.MaxRowSize) / (float)CurrentScroller.MaxRowSize);

		if (FullScrollerColumnSize <= CurrentScroller.MaxColumnSize)
		{
			ScrollbarContainer->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			Scrollbar->SetRenderScale(FVector2D(1.f, (float)3 / FullScrollerColumnSize));
		}
	}
}

void UPA_HudElements_IngameMenu::SetFeedback(TArray<EMenuScrollerFeedbackType> ContentFeedbackToSet, TArray<EMenuScrollerFeedbackType> SublevelFeedbackToSet)
{
	if (MenuContentFeedback.Num() == 0)
	{
		for (int32 Index = 0; Index < 4; Index++)
		{
			FMenuScrollerFeedback NewFeedback;
			float NewYLoc = FeedbackLocation.Y + (Index * (FeedbackIconSize + FeedbackSpacing));

			NewFeedback.Icon = NewObject<UImage>(UImage::StaticClass());
			NewFeedback.Icon->Brush.DrawAs = ESlateBrushDrawType::Image;
			switch (Index)
			{
			case 0:
				if(FeedbackBackIcon)
					NewFeedback.Icon->SetBrushFromMaterial(FeedbackBackIcon);
				break;

			case 1: 
				if (FeedbackIcon2)
					NewFeedback.Icon->SetBrushFromMaterial(FeedbackIcon2);
				break;

			case 2: 
				if (FeedbackIcon3)
					NewFeedback.Icon->SetBrushFromMaterial(FeedbackIcon3);
				break;

			case 3: 
				if (FeedbackIcon4)
					NewFeedback.Icon->SetBrushFromMaterial(FeedbackIcon4);
				break;
			}
			NewFeedback.Icon->SetVisibility(ESlateVisibility::Hidden);
			NewFeedback.IconSlot = MenuContentContainer->AddChildToCanvas(NewFeedback.Icon);
			NewFeedback.IconSlot->SetSize(FVector2D(FeedbackIconSize, FeedbackIconSize));
			NewFeedback.IconSlot->SetPosition(FVector2D(FeedbackLocation.X, NewYLoc));

			NewFeedback.TextBlock = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
			NewFeedback.TextBlock->SetFont(FeedbackStyle.Font);
			NewFeedback.TextBlock->SetColorAndOpacity(FeedbackStyle.ColorAndOpacity);
			NewFeedback.TextBlock->SetShadowColorAndOpacity(FeedbackStyle.ShadowColorAndOpacity);
			NewFeedback.TextBlock->SetShadowOffset(FeedbackStyle.ShadowOffset);
			NewFeedback.TextBlock->SetMargin(FeedbackMargin);
			NewFeedback.TextBlock->SetJustification(ETextJustify::Left);
			//FText TempText = NSLOCTEXT("temp", "temp", "Temporary");
			//NewFeedback.TextBlock
			NewFeedback.TextBlock->SetVisibility(ESlateVisibility::Hidden);

			NewFeedback.TextBlockSlot = MenuContentContainer->AddChildToCanvas(NewFeedback.TextBlock);
			NewFeedback.TextBlockSlot->SetSize(FVector2D(FeedbackTextLength, FeedbackIconSize));
			NewFeedback.TextBlockSlot->SetPosition(FVector2D(FeedbackLocation.X + FeedbackIconSize + FeedbackSpacing, NewYLoc));

			MenuContentFeedback.Emplace(NewFeedback);
		}
	}

	if (SublevelContentFeedback.Num() == 0)
	{
		for (int32 Index = 0; Index < 4; Index++)
		{
			FMenuScrollerFeedback NewFeedback;
			float NewYLoc = FeedbackLocation.Y + (Index * (FeedbackIconSize + FeedbackSpacing));

			NewFeedback.Icon = NewObject<UImage>(UImage::StaticClass());
			NewFeedback.Icon->Brush.DrawAs = ESlateBrushDrawType::Image;
			switch (Index)
			{
			case 0: NewFeedback.Icon->SetBrushFromMaterial(FeedbackBackIcon);
				break;

			case 1: NewFeedback.Icon->SetBrushFromMaterial(FeedbackIcon2);
				break;

			case 2: NewFeedback.Icon->SetBrushFromMaterial(FeedbackIcon3);
				break;

			case 3: NewFeedback.Icon->SetBrushFromMaterial(FeedbackIcon4);
				break;
			}
			NewFeedback.Icon->SetVisibility(ESlateVisibility::Hidden);
			NewFeedback.IconSlot = SublevelContentContainer->AddChildToCanvas(NewFeedback.Icon);
			NewFeedback.IconSlot->SetSize(FVector2D(FeedbackIconSize, FeedbackIconSize));
			NewFeedback.IconSlot->SetPosition(FVector2D(FeedbackLocation.X, NewYLoc));

			NewFeedback.TextBlock = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
			NewFeedback.TextBlock->SetFont(FeedbackStyle.Font);
			NewFeedback.TextBlock->SetColorAndOpacity(FeedbackStyle.ColorAndOpacity);
			NewFeedback.TextBlock->SetShadowColorAndOpacity(FeedbackStyle.ShadowColorAndOpacity);
			NewFeedback.TextBlock->SetShadowOffset(FeedbackStyle.ShadowOffset);
			NewFeedback.TextBlock->SetMargin(FeedbackMargin);
			NewFeedback.TextBlock->SetJustification(ETextJustify::Left);
			//FText TempText = NSLOCTEXT("temp", "temp", "Temporary");
			//NewFeedback.TextBlock
			NewFeedback.TextBlock->SetVisibility(ESlateVisibility::Hidden);

			NewFeedback.TextBlockSlot = SublevelContentContainer->AddChildToCanvas(NewFeedback.TextBlock);
			NewFeedback.TextBlockSlot->SetSize(FVector2D(FeedbackTextLength, FeedbackIconSize));
			NewFeedback.TextBlockSlot->SetPosition(FVector2D(FeedbackLocation.X + FeedbackIconSize + FeedbackSpacing, NewYLoc));

			SublevelContentFeedback.Emplace(NewFeedback);
		}
	}

	//now populate the objects and set the populated ones to visible, while turning off the rest
	for (int32 PopulateIndex = 0; PopulateIndex < MenuContentFeedback.Num(); PopulateIndex++)
	{
		if (ContentFeedbackToSet.IsValidIndex(PopulateIndex))
		{
			FText* NewFeedbackText = Feedback.Find(ContentFeedbackToSet[PopulateIndex]);
			if (NewFeedbackText && MenuContentFeedback.IsValidIndex(PopulateIndex))
			{
				MenuContentFeedback[PopulateIndex].Icon->SetVisibility(ESlateVisibility::Visible);
				MenuContentFeedback[PopulateIndex].TextBlock->SetText(*NewFeedbackText);
				MenuContentFeedback[PopulateIndex].TextBlock->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			MenuContentFeedback[PopulateIndex].Icon->SetVisibility(ESlateVisibility::Hidden);
			MenuContentFeedback[PopulateIndex].TextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	for (int32 PopulateIndex = 0; PopulateIndex < SublevelContentFeedback.Num(); PopulateIndex++)
	{
		if (SublevelFeedbackToSet.IsValidIndex(PopulateIndex))
		{
			FText* NewFeedbackText = Feedback.Find(SublevelFeedbackToSet[PopulateIndex]);
			if (NewFeedbackText && MenuContentFeedback.IsValidIndex(PopulateIndex))
			{
				SublevelContentFeedback[PopulateIndex].Icon->SetVisibility(ESlateVisibility::Visible);
				SublevelContentFeedback[PopulateIndex].TextBlock->SetText(*NewFeedbackText);
				SublevelContentFeedback[PopulateIndex].TextBlock->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			SublevelContentFeedback[PopulateIndex].Icon->SetVisibility(ESlateVisibility::Hidden);
			SublevelContentFeedback[PopulateIndex].TextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

//TODO: make sure this also accounts for tabs eventually.
void UPA_HudElements_IngameMenu::PopulateInventory(TArray<APA_Item_Base *> Items)
{
	if (InventoryItems.Num() == 0)
	{
		for (int32 Index = 0; Index < 25; Index++)
		{
			UPA_HudWidget_InventoryItem* NewItem = NewObject<UPA_HudWidget_InventoryItem>(UPA_HudWidget_InventoryItem::StaticClass());
			FString StartString = FString("Item ") + FString::FromInt(Index);

			//TODO: MAKE SURE THIS IS ONLY DONE WHEN THE ICON SIZE IS CHANGED. RIGHT NOW IT IS CALCULATED EVERY TIME, WHICH IS JUST WASTEFUL.
			//Resize the fonts based on the icon size
			float FontScale = InventoryItemSize / BaseInventoryItemSize;
			FInventoryItemStyle NewStyle = InventoryItemStyle;
			NewStyle.TitleTextNormalStyle.Font.Size = FMath::FloorToInt(NewStyle.TitleTextNormalStyle.Font.Size * FontScale);
			NewStyle.TitleTextRolloverStyle.Font.Size = FMath::FloorToInt(NewStyle.TitleTextRolloverStyle.Font.Size * FontScale);
			NewStyle.TitleTextActiveStyle.Font.Size = FMath::FloorToInt(NewStyle.TitleTextActiveStyle.Font.Size * FontScale);
			NewStyle.TitleTextInactiveStyle.Font.Size = FMath::FloorToInt(NewStyle.TitleTextInactiveStyle.Font.Size * FontScale);
			NewStyle.AmmoTextNormalStyle.Font.Size = FMath::FloorToInt(NewStyle.AmmoTextNormalStyle.Font.Size * FontScale);
			NewStyle.AmmoTextZeroStyle.Font.Size = FMath::FloorToInt(NewStyle.AmmoTextZeroStyle.Font.Size * FontScale);
			NewStyle.AmmoTextInactiveStyle.Font.Size = FMath::FloorToInt(NewStyle.AmmoTextInactiveStyle.Font.Size * FontScale);
			NewStyle.NewTextNormalStyle.Font.Size = FMath::FloorToInt(NewStyle.NewTextNormalStyle.Font.Size * FontScale);

			NewItem->SetInventoryStyle(NewStyle);
			NewItem->SetSize(FVector2D(InventoryItemSize, InventoryItemSize));
			NewItem->SetTitleText(FText::FromString(StartString));

			InventoryItems.Add(NewItem);

			UCanvasPanelSlot* NewItemSlot = ScrollerContents->AddChildToCanvas(NewItem);

			FVector2D NewPosition;
			NewPosition.X = (Index % CurrentScroller.MaxRowSize) * (InventoryItemSize + CurrentScroller.Cellpadding.Right) + CurrentScroller.Cellpadding.Left;
			NewPosition.Y = FMath::FloorToInt((float)Index / (float)CurrentScroller.MaxRowSize) * (InventoryItemSize + CurrentScroller.Cellpadding.Bottom);

			NewItemSlot->SetPosition(NewPosition);
			NewItemSlot->SetSize(FVector2D(InventoryItemSize, InventoryItemSize));
		}

		//the blanks are required so we don't scroll past what's visible
		if (InventoryItems.Num() >= CurrentScroller.MaxRowSize * 3)
		{
			for (int32 BlankIndex = InventoryItems.Num(); BlankIndex < InventoryItems.Num() + CurrentScroller.MaxRowSize; BlankIndex++)
			{
				UImage* Blank = NewObject<UImage>(UImage::StaticClass());
				Blank->Brush.DrawAs = ESlateBrushDrawType::Image;
				Blank->SetColorAndOpacity(FLinearColor::Black);
				//Blank->Brush.ImageSize = FVector2D(CurrentScroller.MinDesiredCellWidth, CurrentScroller.MinDesiredCellHeight);
				Blank->Brush.ImageSize = FVector2D(InventoryItemSize, InventoryItemSize);

				//UUniformGridSlot* BlankSlot = ScrollerContents->AddChildToUniformGrid(Blank);
				UCanvasPanelSlot* BlankSlot = ScrollerContents->AddChildToCanvas(Blank);

				FVector2D NewPosition;
				NewPosition.X = (BlankIndex % CurrentScroller.MaxRowSize) * (InventoryItemSize + CurrentScroller.Cellpadding.Right) + CurrentScroller.Cellpadding.Left;
				NewPosition.Y = FMath::FloorToInt((float)BlankIndex / (float)CurrentScroller.MaxRowSize) * (InventoryItemSize + CurrentScroller.Cellpadding.Bottom);

				BlankSlot->SetPosition(NewPosition);
				BlankSlot->SetSize(FVector2D(InventoryItemSize, InventoryItemSize));
				//BlankSlot->SetHorizontalAlignment(HAlign_Center);
				//BlankSlot->SetVerticalAlignment(VAlign_Center);
				//BlankSlot->SetRow(FMath::FloorToInt((float)BlankIndex / (float)CurrentScroller.MaxRowSize));
				//BlankSlot->SetColumn(BlankIndex % CurrentScroller.MaxRowSize);
			}
		}
	}
}

void UPA_HudElements_IngameMenu::SetTargetScrollOffset(int32 Index)
{
	int32 ScrollTargetIndex = (Index / CurrentScroller.MaxRowSize) - 2;

	ScrollerScrollTarget = ScrollTargetIndex * (InventoryItemSize + CurrentScroller.Cellpadding.Top + CurrentScroller.Cellpadding.Bottom);
	ScrollerScrollTarget = ScrollerScrollTarget < 0 ? 0 : ScrollerScrollTarget;

	int32 MaxScrollSteps = FMath::CeilToInt(((float)ScrollerContents->GetChildrenCount() - (float)CurrentScroller.MaxRowSize) / (float)CurrentScroller.MaxRowSize) - CurrentScroller.MaxColumnSize;//3; //since we can only have 3 rows.
	if (MaxScrollSteps > 0)
	{
		float ScrollbarIncrement = (ScrollbarHeight * (1.f - Scrollbar->RenderTransform.Scale.Y)) / MaxScrollSteps;

		ScrollerScrollbarTarget = (ScrollTargetIndex * ScrollbarIncrement) + ScrollbarArrowSize.Y;
		ScrollerScrollbarTarget = ScrollerScrollbarTarget < ScrollbarArrowSize.Y ? ScrollbarArrowSize.Y : ScrollerScrollbarTarget;
	}
}

//---------------------------------------------------------------------------------------------------------------//
// BOOK UI OPEN AND CLOSE
//---------------------------------------------------------------------------------------------------------------//

void UPA_HudElements_IngameMenu::OpenMainMenu()
{
	UWorld* World = GetWorld();
	if (!IsValid(BookInstance) && World)
	{
		for (TActorIterator<APA_ISMA_InteractiveBook> ActorItr(World); ActorItr; ++ActorItr)
		{
			BookInstance = *ActorItr;
			if (IsValid(BookInstance)) break;
		}
	}

	CurrentLoadType = ELoadBookUIType::BOOK_OpenMainMenu;
	this->SetVisibility(ESlateVisibility::Visible);

	//if (OwnerHUD)
	//	OwnerHUD->StatusIndicatorInstance->StartOpenFadeIn();

	if (BookInstance)
		BookInstance->OpenMainMenu();
}

void UPA_HudElements_IngameMenu::CloseMainMenu()
{
	CurrentLoadType = ELoadBookUIType::BOOK_CloseMainMenu;

	if (BookInstance)
		BookInstance->CloseMainMenu();
}

void UPA_HudElements_IngameMenu::OpenIngameMenu()
{
	UWorld* World = GetWorld();
	if (!IsValid(BookInstance) && World)
	{
		for (TActorIterator<APA_ISMA_InteractiveBook> ActorItr(World); ActorItr; ++ActorItr)
		{
			BookInstance = *ActorItr;
			if (IsValid(BookInstance)) break;
		}
	}

	CurrentLoadType = ELoadBookUIType::BOOK_OpenIngameMenu;
	this->SetVisibility(ESlateVisibility::Visible);
	OwnerHUD->EnableHUDInput(OwnerHUD->PlayerOwner);
	SetMenuItemIndex(0);

	//For now directly access the menu functions
	//Later include the capture component too. For now delete all capture component crap.
	//if (OwnerHUD)
	//	OwnerHUD->StatusIndicatorInstance->StartOpenFadeIn();

	if (BookInstance)
		BookInstance->OpenIngameMenu();
}

void UPA_HudElements_IngameMenu::CloseIngameMenu()
{
	CurrentLoadType = ELoadBookUIType::BOOK_CloseIngameMenu;

	if (BookInstance)
	{
		BookInstance->CloseIngameMenu();
	}

	OwnerHUD->DisableInput(OwnerHUD->PlayerOwner);
}

void UPA_HudElements_IngameMenu::SetIngameUIVisibility(ESlateVisibility NewVisibility)
{
	MenuItemContainer->SetVisibility(NewVisibility);
	MenuContentMask->SetVisibility(NewVisibility);
	MenuContentContainer->SetVisibility(NewVisibility);
	SublevelContentContainer->SetVisibility(NewVisibility);
}

void UPA_HudElements_IngameMenu::TurnOffMaskAnimation()
{
	bAnimateMask = false;
}

void UPA_HudElements_IngameMenu::TurnOnMaskAnimation()
{
	bAnimateMask = true;
}