// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "PiratesAdventure.h"
#include "SlateBasics.h"
#include "PA_HudWidget_InventoryItemStyle.h"
#include "PA_SlateStyles.h"

/** Type of check box */
//UENUM()
//namespace EInventoryItemState
//{
//	enum Type
//	{
//		/** Basic icon for inventory item. */
//		Normal,
//
//		/** When player highlights the inventory icon. Overrides normal. */
//		Rollover,
//
//		/** When player has activated the inventory icon. Overrides normal and rollover. */
//		Active,
//
//		/** When the player cannot select the inventory icon. Overrides everything else. */
//		Inactive
//	};
//}

UENUM()
enum class EInventoryItemState : uint8
{
	Normal UMETA(DisplayName = "Normal"), 
	Rollover UMETA(DisplayName = "Rollover"), 
	Active UMETA(DisplayName = "Active"),
	Inactive UMETA(DisplayName = "Inactive")
};

class PIRATESADVENTURE_API SPA_HudWidget_InventoryItemSlate : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SPA_HudWidget_InventoryItemSlate)
		: _Content()
		, _InventoryItemStyle(&FPA_SlateStyles::Get().GetWidgetStyle< FInventoryItemStyle >("InventoryItemStyle"))
		, _HasAmmo(true)
		, _BackgroundNormal(nullptr)
		, _BackgroundRollover(nullptr)
		, _BackgroundActive(nullptr)
		, _BackgroundInactive(nullptr)
		, _TitleText(FText::FromString("Test"))
		, _AmmoText(FText::FromString("6"))
		, _NewText(FText::FromString("New"))
		, _Size(FVector2D(175.f,175.f))
		, _Scale(FVector2D(1.f,1.f))
		{}

		/** Slot for this item's content (optional) */
		SLATE_DEFAULT_SLOT(FArguments, Content)

		/** The visual style of the Inventory item */
		SLATE_STYLE_ARGUMENT(FInventoryItemStyle, InventoryItemStyle)

		/** For if we should draw an ammo container as well */	
		SLATE_ARGUMENT(bool, HasAmmo)

		/** Normal background for inventory icon */
		SLATE_ARGUMENT(const FSlateBrush*, BackgroundNormal)

		/** Rollover background for inventory icon */
		SLATE_ARGUMENT(const FSlateBrush*, BackgroundRollover)

		/** Active background for inventory icon */
		SLATE_ARGUMENT(const FSlateBrush*, BackgroundActive)

		/** Inactive background for inventory icon */
		SLATE_ARGUMENT(const FSlateBrush*, BackgroundInactive)

		/** Initial Title Text */
		SLATE_ARGUMENT(FText, TitleText)

		/** Initial Ammo Text */
		SLATE_ARGUMENT(FText, AmmoText)

		/** Initial New Text */
		SLATE_ARGUMENT(FText, NewText)

		/** Initial Icon Size */
		SLATE_ARGUMENT(FVector2D, Size)

		/** Initial Icon Scale */
		SLATE_ARGUMENT(FVector2D, Scale)

		SLATE_END_ARGS()

public:

	//widget overrides
	void Construct(const FArguments& InArgs);
	//for rebuilding the inventory icon when its state changes
	void BuildInventoryIcon(/*TSharedRef<SWidget> InContent*/);
	virtual FVector2D ComputeDesiredSize(float) const override;

	FText GetAmmoText() const;
	FText GetTitleText() const;
	FText GetNewText() const;

	/** Set the inventory item state */
	void SetInventoryIconState(TAttribute<EInventoryItemState> InNewState);
	EInventoryItemState GetInventoryItemState();

public:
	//Whether to show ammo text or not
	bool bHasAmmo;

	TAttribute<EInventoryItemState> InventoryIconState;
	
	//Total size of the icon
	FVector2D Size;
	//Total scale of the icon
	FVector2D Scale;

	/** Image to use for any situation not involving the following three. */
	const FSlateBrush* BackgroundNormal;
	/** Image to use when the selector hovers over the icon. */
	const FSlateBrush* BackgroundRollover;
	/** Image to use when the player has selected this icon, until the next one is checked. */
	const FSlateBrush* BackgroundActive;
	/** Image to use when the icon cannot be selected for any reason. */
	const FSlateBrush* BackgroundInactive;

	void SetBackgroundNormal(const FSlateBrush* Brush);
	void SetBackgroundRollover(const FSlateBrush* Brush);
	void SetBackgroundActive(const FSlateBrush* Brush);
	void SetBackgroundInactive(const FSlateBrush* Brush);

	void SetTitleText(FText Text);
	void SetAmmoCount(int32 Ammo);
	void SetNewText(FText Text);

	void SetTitleScale(float NewScale);
	void SetAmmoScale(float NewScale);
	void SetNewTextScale(float NewScale);

	void SetInventoryStyle(FInventoryItemStyle* NewStyle);

	void SetSize(FVector2D NewSize) { Size = NewSize; };
	void SetScale(FVector2D NewScale) { Scale = NewScale; };

	//If we have ammo, the text contents for the item box.
	FText AmmoText = FText::FromString("0");
	//The name of the item
	FText TitleText = FText::FromString("Name");
	//The text shown when an item is new
	FText NewText = FText::FromString("New");

	///** The wrapped layout for the ammo text block */
	//TSharedPtr< FTextBlockLayout > AmmoTextLayoutCache;
	///** The wrapped layout for the title text block */
	//TSharedPtr< FTextBlockLayout > TitleTextLayoutCache;
	///** The wrapped layout for the new text block */
	//TSharedPtr< FTextBlockLayout > NewTextLayoutCache;

	const FInventoryItemStyle* InventoryItemStyle;

protected:

	///** Rebuilds the InventoryIcon based on the current ESlateCheckBoxType */
	//void BuildInventoryIcon(TSharedRef<SWidget> InContent);

	const FSlateBrush* OnGetBackground() const;

	const FSlateBrush* GetBackgroundNormal() const;
	const FSlateBrush* GetBackgroundRollover() const;
	const FSlateBrush* GetBackgroundActive() const;
	const FSlateBrush* GetBackgroundInactive() const;

	const FTextBlockStyle* OnGetTitleStyle() const;
	const FTextBlockStyle* OnGetAmmoStyle() const;

protected:
	//These variables will be saved so we can update the actual text when we want to. 
	///** This will hold the reference to our Inventory Icon */
	TSharedPtr<STextBlock> TitleTextContainer;
	TSharedPtr<STextBlock> AmmoTextContainer;
	TSharedPtr<STextBlock> NewTextContainer;

};