// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PA_HudWidget_InventoryItemStyle.h"
#include "PA_HudElements_IngameMenu.generated.h"

/*

This class will be the main functionality class for everything related to the main menu. 
With the exception of perhaps the options menu (due to its complicated functionality), all functionality is handled here.

-handles the different kinds of navigation between the menu levels being menu items, content, tab and sublevel.
-handles changing item and button states

The old system required level streaming, which is too risky to use and annoying to setup. Instead, now we will simply use a blueprint 
that we will search for on beginplay. 

*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIngameMenuSignature);

//used to indicate what menu to open
UENUM()
enum class ELoadBookUIType : uint8
{
	BOOK_Inactive UMETA(DisplayName = "Inactive"),
	BOOK_OpenMainMenu UMETA(DisplayName = "Open Main Menu"),
	BOOK_CloseMainMenu UMETA(DisplayName = "Close Main Menu"),
	BOOK_OpenIngameMenu UMETA(DisplayName = "Open Ingame Menu"),
	BOOK_CloseIngameMenu UMETA(DisplayName = "Close Ingame Menu")
};

//Is partly responsible for controls within the 3 menu levels. The sub levels within the Menu Contents operate slightly different. 
UENUM()
enum class EMenuLevel : uint8
{
	//Do we need separate levels for other menu types here?
	ML_MenuItem UMETA(DisplayName = "Menu Item"),
	ML_MenuContent UMETA(DisplayName = "Menu Content"),
	ML_MenuContentTabs UMETA(DisplayName = "Menu Content Tabs"),
	ML_MenuContentSublevel UMETA(DisplayName = "Menu Content Sublevel"),
};

//the current open menu. Determines the initial action / initial content window.
UENUM()
enum class EMenuType : uint8
{
	MT_Back UMETA(DisplayName = "Menu Back"),
	MT_Item UMETA(DisplayName = "Menu Items"),
	MT_Status UMETA(DisplayName = "Menu Status"),
	MT_Collectible UMETA(DisplayName = "Menu Collectibles"),
	MT_Options UMETA(DisplayName = "Menu Options"),
	MT_Save UMETA(DisplayName = "Menu Save"),
	MT_Quit UMETA(DisplayName = "Menu Quit")
};

USTRUCT(Blueprintable)
struct FMenuItem
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		class UCanvasPanel* Container;

	UPROPERTY()
		class UVerticalBoxSlot* ContainerSlot;

	UPROPERTY()
		class UImage* Icon;

	UPROPERTY()
		class UCanvasPanelSlot* IconSlot;

	UPROPERTY()
		class UImage* Background;

	UPROPERTY()
		class UCanvasPanelSlot* BackgroundSlot;

	UPROPERTY()
		class UImage* Selector;

	UPROPERTY()
		class UCanvasPanelSlot* SelectorSlot;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* TextBlock;

	UPROPERTY()
		class UCanvasPanelSlot* TextBlockSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Item")
		EMenuType MenuType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Item")
		FText Text;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Item")
		class UMaterialInterface* Standard;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Item")
		class UMaterialInterface* Rollover;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Item")
		class UMaterialInterface* Active;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Item")
		class UMaterialInterface* Inactive;
};

USTRUCT(Blueprintable)
struct FInventoryItem //change this into a separate widget rather than a struct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		class UCanvasPanel* Container;

	UPROPERTY()
		class UCanvasPanelSlot* ContainerSlot;
		//class UUniformGridSlot* ContainerSlot;

	UPROPERTY()
		class UImage* Icon;

	UPROPERTY()
		class UCanvasPanelSlot* IconSlot;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* TextBlock;

	UPROPERTY()
		class UCanvasPanelSlot* TextBlockSlot;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* AmmoBlock;

	UPROPERTY()
		class UCanvasPanelSlot* AmmoBlockSlot;

	UPROPERTY()
		class APA_Item_Base* ItemData;
};

USTRUCT(Blueprintable)
struct FMenuScrollerTab
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		class UImage* Icon;

	UPROPERTY()
		class UCanvasPanelSlot* IconSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		FText TitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		class UMaterialInterface* Standard;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		class UMaterialInterface* Rollover;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		class UMaterialInterface* Active;
};

UENUM()
enum class EMenuScrollerFeedbackType : uint8
{
	//this is purely meant as a default value, without any effect. 
	SF_None UMETA(DisplayName = "None"),
	SF_Back UMETA(DisplayName = "Back"),
	SF_Select UMETA(DisplayName = "Select"),
	SF_Info UMETA(DisplayName = "Info"),
	SF_Equip UMETA(DisplayName = "Equip"),
};

USTRUCT(Blueprintable)
struct FMenuScrollerFeedback
{
	GENERATED_USTRUCT_BODY()

public:
	//TODO: make this dependent on key bindings later on, when you actually have the ability to redefine key bindings
	UPROPERTY()
		class UImage* Icon;

	UPROPERTY()
		class UCanvasPanelSlot* IconSlot;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* TextBlock;

	UPROPERTY()
		class UCanvasPanelSlot* TextBlockSlot;

	//TODO: make sure we assign the icons dynamically, based on the current keybinding/controller being used.
	//This seems to cause a garbage collection error? -> add it back when you actually use it.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Feedback")
	//	class UMaterialInterface* IconMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Feedback")
		EMenuScrollerFeedbackType FeedbackType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Feedback")
		FText FeedbackText;
};

USTRUCT(Blueprintable)
struct FMenuScrollerData
{
	GENERATED_USTRUCT_BODY()

public: 
	//This determines whether we should render a render target next to the inventory items. Is only used for player status at the moment.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		bool bHasRenderTarget;

	//If we have a title, how high should it be? Width depends on scroller window.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		float TitleHeight;

	//The padding of the borders of the scroller.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		FMargin ScrollerPadding = FMargin(0, 20, 80, 20);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		FScrollBoxStyle ScrollerStyle;

	//The padding of each individual cell.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		FMargin Cellpadding = FMargin(20);

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
	//	float MinDesiredCellHeight = 175;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
	//	float MinDesiredCellWidth = 175;

	//The maximum number of columns allowed for this content window
	//TODO: make this dependent on icon size rather than setting it manually.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
	UPROPERTY()
		int32 MaxRowSize = 1;

	//If we are making the row size variable, we also need to make the max column size variable.
	UPROPERTY()
		int32 MaxColumnSize = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = EHorizontalAlignment::HAlign_Center;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		TEnumAsByte<EVerticalAlignment> VerticalAlignment = EVerticalAlignment::VAlign_Center;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		TArray<FMenuScrollerTab> Tabs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		TArray<EMenuScrollerFeedbackType> ContentFeedbacks;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Content Scroller")
		TArray<EMenuScrollerFeedbackType> SublevelFeedbacks;
};

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudElements_IngameMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	//--------------------------------------------------------------------------------------------
	//    BASIC VARIABLES
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		class APA_PlayerHUD* OwnerHUD;

	UFUNCTION()
		void SetWidgetRenderTransform(UStaticMeshComponent* Mesh, UWidget* Widget, FVector2D WidgetSize, FName TopLeftSocket, FName TopRightSocket);

	//--------------------------------------------------------------------------------------------
	//    NAVIGATION
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		int32 CurrentMenuItemIndex;

	UPROPERTY()
		EMenuLevel CurrentMenuLevel = EMenuLevel::ML_MenuItem;

	UPROPERTY()
		int32 CurrentScrollerIndex;

	UPROPERTY()
		int32 CurrentTabIndex;

	UFUNCTION()
		void ProcessVerticalNavigation(int32 Direction);

	UFUNCTION()
		void ProcessHorizontalNavigation(int32 Direction);

	UFUNCTION()
		void ProcessConfirm();

	UFUNCTION()
		void ProcessCancel();

	UFUNCTION()
		void SetMenuItemIndex(int32 NewIndex);

	//Set the current item on the scroller to rollover. If bFocusOnly is false, it will also reset the previous one back to normal. 
	UFUNCTION()
		void SetScrollerIndex(int32 NewIndex);

	//Needed to reset the scroller for when we go back to the main menu item level.
	UFUNCTION()
		void UnfocusScroller();

	//Set the current tab to rollover. If bFocusOnly is false, it will also reset the previous one back to normal. 
	UFUNCTION()
		void SetTabIndex(int32 NewIndex);

	//Takes care of spawning (i.e. activating the right content type for the content window.
	UFUNCTION()
		void SetContentWindow(EMenuType NewContentType);

	//TODO: make this a bit more advanced
	UFUNCTION()
		void SetBookinstanceSubLevel(int32 SubLevelIndex);

	//--------------------------------------------------------------------------------------------
	//    MENU ITEM PANEL
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		class UVerticalBox* MenuItemContainer;

	UPROPERTY()
		class UCanvasPanelSlot* MenuItemContainerSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Menu Items")
		TArray<FMenuItem> MenuItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Menu Items")
		FVector2D MenuItemContainerSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Menu Items")
		FVector2D MenuItemSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Menu Items")
		class UMaterialInterface* MenuItemSelector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FTextBlockStyle MenuItemTextNormalStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FMargin MenuItemTextNormalMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FTextBlockStyle MenuItemTextRolloverStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FMargin MenuItemTextRolloverMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FTextBlockStyle MenuItemTextActiveStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FMargin MenuItemTextActiveMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FTextBlockStyle MenuItemTextInactiveStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Menu Items")
		FMargin MenuItemTextInactiveMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Rendering")
		FName MenuItemsTopLeftSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Rendering")
		FName MenuItemsTopRightSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Rendering")
		FName MenuContentsTopLeftSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Rendering")
		FName MenuContentsTopRightSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Rendering")
		FName MenuTabsTopLeftSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Rendering")
		FName MenuTabsTopRightSocket;

	UPROPERTY()
		class APA_ISMA_InteractiveBook* BookInstance;

	UFUNCTION()
		void SetMenuItemsVisibility(ESlateVisibility NewVisibility);

	//--------------------------------------------------------------------------------------------
	//    CONTENT CONTAINER PANEL
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		class URetainerBox* MenuContentMask;

	UPROPERTY()
		class UCanvasPanelSlot* MenuContentMaskSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		class UMaterialInterface* MenuContentMaskMaterialTemplate;

	UPROPERTY()
		class UMaterialInstanceDynamic* MenuContentMaskMaterial;

	UPROPERTY()
		int32 MenuContentsRefreshRate = 1;

	UPROPERTY()
		bool bAnimateMask = false;

	UFUNCTION()
		void TurnOffMaskAnimation();

	UFUNCTION()
		void TurnOnMaskAnimation();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FName MenuContentMaskTextureParameter;

	UPROPERTY()
		class UCanvasPanel* MenuContentContainer;

	UPROPERTY()
		class UCanvasPanelSlot* MenuContentContainerSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FVector2D MenuContentContainerSize;

	UPROPERTY()
		FMenuScrollerData CurrentScroller;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FMenuScrollerData ItemContentsScroller;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FMenuScrollerData EquipmentContentsScroller;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FMenuScrollerData CollectibleContentScroller;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FMenuScrollerData OptionsContentScroller;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents")
		FMenuScrollerData SaveContentScroller;

	//UPROPERTY()
	//	class UPA_HudWidget_ContentsBase* TextScrollerWidget;

	//TODO: add the collectibles 6-button widget, options and save later.

	//--------------------------------------------------------------------------------------------
	//    CONTENT CONTAINER - INVENTORY ITEMS
	//--------------------------------------------------------------------------------------------

	void ResizeScroller(FMenuScrollerData ScrollerData);

	//since we need to set this one up AFTER the items have been initialized, we cannot do this in the scroller function.
	void ResizeScrollbar();

	void PopulateInventory(TArray<APA_Item_Base*> Items);

	void SetTargetScrollOffset(int32 Index);

	UPROPERTY()
		class UScrollBox* Scroller;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollerSlot;

	//the target to aim for in the case scrolling is necessary in the scroller. 
	UPROPERTY()
		float ScrollerScrollTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		float ScrollerScrollSpeed;

	//The speed of the scrollbar should be slightly higher than the scroller, since they both animate over different distances. 
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
	//	float ScrollerScrollbarSpeed;

	UPROPERTY()
		class UCanvasPanel* ScrollbarContainer;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollbarContainerSlot;

	UPROPERTY()
		class UImage* ScrollbarTopArrow;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollbarTopArrowSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		class UMaterialInterface* ScrollbarTopArrowMaterial;

	UPROPERTY()
		class UImage* ScrollbarBottomArrow;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollbarBottomArrowSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		class UMaterialInterface* ScrollbarBottomArrowMaterial;

	//We are using the same size for both the top and bottom arrow.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		FVector2D ScrollbarArrowSize;

	//The scrollbar takes the position of the scroller and places it to the left of it. The padding determines the distance to the scroller.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		FVector2D ScrollbarPadding;

	UPROPERTY()
		class UImage* Scrollbar;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollbarSlot;

	//Since the scrollbar needs to scale, the size of the scrollbar is assumed to be when at full size (100%). Width is taken from the arrows.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		float ScrollbarHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		class UMaterialInterface* ScrollbarMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Scroller")
		FMargin ScrollbarMaterialMargin;

	UPROPERTY()
		class UCanvasPanel* ScrollerContents;
		//class UUniformGridPanel* ScrollerContents;

	UPROPERTY()
		class UScrollBoxSlot* ScrollerContentsSlot;

	//the target to aim for in the case scrolling is necessary in the scroller. 
	UPROPERTY()
		float ScrollerScrollbarTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle TabTitleStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin TabTitleMargin;

	UPROPERTY()
		TArray<class UPA_HudWidget_InventoryItem*> InventoryItems;

	//The size of the icon at its biggest. Used to determine the scale of the text as well.
	UPROPERTY(EditDefaultsOnly, Category = "Ingame Menu - Contents Inventory Icons")
		float BaseInventoryItemSize = 175.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		float InventoryItemSize = 175.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FInventoryItemStyle InventoryItemStyle;

	//TODO: add fonts for rollovers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle InventoryItemTitleNormalStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin InventoryItemTitleNormalMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle InventoryItemTitleRolloverStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin InventoryItemTitleRolloverMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle InventoryItemTitleInactiveStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin InventoryItemTitleInactiveMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle InventoryItemAmmoNormalStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin InventoryItemAmmoNormalMargin;

	//This is the color used for when ammo count reaches 0.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle InventoryItemAmmoZeroStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin InventoryItemAmmoZeroMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FTextBlockStyle InventoryItemAmmoInactiveStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Inventory Icons")
		FMargin InventoryItemAmmoInactiveMargin;

	//The basic array that holds all the item data for the item submenu.
	//TODO: Do we need this if we are going to make our own slate implemention?
	UFUNCTION()
		FInventoryItem ConstructNewInventoryItem(class APA_Item_Base* NewItem);

	UPROPERTY()
		TArray<FInventoryItem> ItemArrayItems;

	//TODO: add the other arrays here as well

	//--------------------------------------------------------------------------------------------
	//    CONTENT CONTAINER - CONTENT FEEDBACK FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Contents Feedback")
		TMap<EMenuScrollerFeedbackType, FText> Feedback;
		//TArray<FMenuScrollerFeedback> Feedback;

	UPROPERTY()
		TArray<FMenuScrollerFeedback> MenuContentFeedback;

	UPROPERTY()
		TArray<FMenuScrollerFeedback> SublevelContentFeedback;

	//The first icon will ALWAYS be the back icon. Other icons can be variable in content, but this will remain back.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		class UMaterialInterface* FeedbackBackIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		class UMaterialInterface* FeedbackIcon2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		class UMaterialInterface* FeedbackIcon3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		class UMaterialInterface* FeedbackIcon4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		FVector2D FeedbackLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		FTextBlockStyle FeedbackStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		float FeedbackIconSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		float FeedbackTextLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		float FeedbackSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame Menu - Contents Feedback")
		FMargin FeedbackMargin;

	UFUNCTION()
		void SetFeedback(TArray<EMenuScrollerFeedbackType> ContentFeedbackToSet, TArray<EMenuScrollerFeedbackType> SublevelFeedbackToSet);

	//--------------------------------------------------------------------------------------------
	//    SUBLEVEL CONTENT CONTAINER
	//--------------------------------------------------------------------------------------------
	
	UFUNCTION()
		void SetContentSublevelWindow(int32 ItemIndex);

	UPROPERTY()
		class UCanvasPanel* SublevelContentContainer;

	UPROPERTY()
		class UCanvasPanelSlot* SublevelContentContainerSlot;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* SublevelTitle;

	UPROPERTY()
		class UImage* SublevelIcon;

	UPROPERTY()
		class UCanvasPanelSlot* SublevelIconSlot;

	UPROPERTY()
		class UScrollBox* SublevelScroller;

	UPROPERTY()
		class UCanvasPanelSlot* SublevelScrollerSlot;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* SublevelScrollerContents;

	UPROPERTY()
		class UScrollBoxSlot* SublevelScrollerContentsSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		FMargin SublevelContentContainerMargin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		class UMaterialInterface* SublevelIconMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		FVector2D SublevelIconSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		float SublevelIconTextSpacing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		FTextBlockStyle SublevelTitleStyle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		float SublevelTitleSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		FMargin SublevelTitleMargin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		FTextBlockStyle SublevelContentStyle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		float SublevelContentSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ingame Menu - Sublevel")
		FMargin SublevelContentMargin;

	//--------------------------------------------------------------------------------------------
	//    MENU LOADING AND UNLOADING
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		ELoadBookUIType CurrentLoadType;

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void OpenMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void CloseMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void OpenIngameMenu();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void CloseIngameMenu();

	UFUNCTION()
		void SetIngameUIVisibility(ESlateVisibility NewVisibility);
};
