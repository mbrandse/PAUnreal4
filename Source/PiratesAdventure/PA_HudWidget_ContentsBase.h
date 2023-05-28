// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PA_HudElements_IngameMenu.h"
#include "PA_HudWidget_ContentsBase.generated.h"

USTRUCT(Blueprintable)
struct FMenuContentTab
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		UImage* Icon;

	UPROPERTY()
		class UCanvasPanelSlot* IconSlot;

	UPROPERTY()
		UImage* Background;

	UPROPERTY()
		class UCanvasPanelSlot* BackgroundSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		FText Text;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		UMaterialInterface* Standard;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Tab")
		UMaterialInterface* Rollover;
};

USTRUCT(Blueprintable)
struct FMenuContentFeedback
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		UImage* Icon;

	UPROPERTY()
		class UCanvasPanelSlot* IconSlot;

	UPROPERTY()
		class UTextBlock* TextBlock;

	UPROPERTY()
		class UCanvasPanelSlot* TextBlockSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Menu Feedback")
		FText Text;
};

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudWidget_ContentsBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//--------------------------------------------------------------------------------------------
	//    BASIC FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		class UPA_HudElements_IngameMenu* OwnerWidget;

	virtual void ConstructContentsWidget(TArray<FInventoryItem> Items);

	//--------------------------------------------------------------------------------------------
	//    POPULATION AND NAVIGATION FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	virtual FInventoryItem GetItemFromIndex(int32 ItemIndex) const;

	virtual int32 GetMaxItemIndex() const; //since we cannot use the arrays for this (the content base arrays being variable and the total array being split up in categories), use this function.

	virtual void GetDataFromTab(int32 TabIndex); //this is more for sorting and should be implemented in this class.

	virtual int32 GetMaxTabIndex() const; //since tabs change per content window.

	//--------------------------------------------------------------------------------------------
	//    TAB FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Tabs")
		TArray<FMenuContentTab> Tabs;

	//--------------------------------------------------------------------------------------------
	//    SCROLL BAR FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		UImage* ScrollBarTopArrow;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollBarTopArrowSlot;

	UPROPERTY()
		UImage* ScrollBarSlider; //make sure this image is set to border or something, so we can have 9slice scaling

	UPROPERTY()
		class UCanvasPanelSlot* ScrollBarSliderSlot;

	UPROPERTY()
		UImage* ScrollBarBottomArrow;

	UPROPERTY()
		class UCanvasPanelSlot* ScrollBarBottomArrowSlot;

	//--------------------------------------------------------------------------------------------
	//    SCROLLER FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* TitleText;

	UPROPERTY()
		class UCanvasPanelSlot* TitleTextSlot;

	//Width is determined automatically depending on maximum column size and padding.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		float TitleHeight; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Scroller")
		FSlateFontInfo TitleFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Scroller")
		FMargin TitleMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Scroller")
		FLinearColor TitleColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Scroller")
		FVector2D TitleShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Scroller")
		FLinearColor TitleShadowColorAndOpacity;

	UPROPERTY()
		UScrollBox* Scroller;

	UPROPERTY()
		UCanvasPanelSlot* ScrollerSlot;

	UPROPERTY()
		class UCanvasPanel* ScrollerContent;
		//UUniformGridPanel* ScrollerContent;

	UPROPERTY()
		UScrollBoxSlot* ScrollerContentSlot;

	//The padding of the borders of the scroller.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		FMargin ScrollerPadding = FMargin(0, 20, 80, 20);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		FScrollBoxStyle ScrollerStyle;

	//The padding of each individual cell.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		FMargin Cellpadding = FMargin(20);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		float MinDesiredCellHeight = 175;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		float MinDesiredCellWidth = 175;

	//The maximum number of columns allowed for this content window
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		int32 MaxColumnSize = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = EHorizontalAlignment::HAlign_Center;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Scroller")
		TEnumAsByte<EVerticalAlignment> VerticalAlignment = EVerticalAlignment::VAlign_Center;


	//--------------------------------------------------------------------------------------------
	//    CONTENT FEEDBACK FUNCTIONALITY
	//--------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content Feedback")
		TArray<FMenuContentFeedback> Feedback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Feedback")
		FSlateFontInfo FeedbackFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Feedback")
		FLinearColor FeedbackColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Feedback")
		FVector2D FeedbackShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content Feedback")
		FLinearColor FeedbackShadowColorAndOpacity;	
	
};
