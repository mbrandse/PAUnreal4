// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "PA_HudWidget_InventoryItemStyle.generated.h"

/**
* Represents the appearance of an InventoryItem
*/
USTRUCT(BlueprintType)
struct FInventoryItemStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

		FInventoryItemStyle();

	virtual ~FInventoryItemStyle() {}

	virtual void GetResources(TArray< const FSlateBrush* >& OutBrushes) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override;

	static const FInventoryItemStyle& GetDefault();

	/** Icon Appearance When Normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
		FSlateBrush BackgroundNormal;

	/** Icon Appearance When Rollover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
		FSlateBrush BackgroundRollover;

	/** Icon Appearance When Active (equipped or activated) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
		FSlateBrush BackgroundActive;

	/** Icon Appearance When Inactive */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
		FSlateBrush BackgroundInactive;

	/** Ammo Appearance When Active */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	//	FSlateBrush AmmoNormal;

	/** Ammo Appearance When Inactive */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	//	FSlateBrush AmmoInactive;

	//FInventoryItemStyle& SetBackground(const FSlateBrush& InBackground) { Background = InBackground; return *this; };
	FInventoryItemStyle& SetBackgroundNormal(const FSlateBrush& InNormal) { BackgroundNormal = InNormal; return *this; };
	FInventoryItemStyle& SetBackgroundRollover(const FSlateBrush& InRollover) { BackgroundRollover = InRollover; return *this; };
	FInventoryItemStyle& SetBackgroundActive(const FSlateBrush& InActive) { BackgroundActive = InActive; return *this; };
	FInventoryItemStyle& SetBackgroundInactive(const FSlateBrush& InInactive) { BackgroundInactive = InInactive; return *this; };
	//FInventoryItemStyle& SetBackgroundInactive(const FSlateBrush& InInactive) { BackgroundInactive = InInactive; AmmoInactive = InInactive; return *this; }; //old with ammo switch

	//Size will be ignored for any axis whose alignment is set to fill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FVector2D TitleTextSize;

	//This uses a relative value from 0-1 for calculating the margin (1 = 100%, 0 = 0% of the icon size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FMargin TitleTextMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		TEnumAsByte<EHorizontalAlignment> TitleTextHorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		TEnumAsByte<EVerticalAlignment> TitleTextVerticalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle TitleTextNormalStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle TitleTextRolloverStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle TitleTextActiveStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle TitleTextInactiveStyle;

	//Size will be ignored for any axis whose alignment is set to fill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FVector2D AmmoTextSize;

	//This uses a relative value from 0-1 for calculating the margin (1 = 100%, 0 = 0% of the icon size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FMargin AmmoTextMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		TEnumAsByte<EHorizontalAlignment> AmmoTextHorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		TEnumAsByte<EVerticalAlignment> AmmoTextVerticalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle AmmoTextNormalStyle;

	//This style is shown when the ammo for an item is 0. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle AmmoTextZeroStyle;

	//This is shown when the item cannot be selected. It's NOT the same as the zero style. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle AmmoTextInactiveStyle;	//This uses a relative value from 0-1 for calculating the margin (1 = 100%, 0 = 0% of the icon size)

	//Size will be ignored for any axis whose alignment is set to fill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FVector2D NewTextSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FMargin NewTextMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		TEnumAsByte<EHorizontalAlignment> NewTextHorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		TEnumAsByte<EVerticalAlignment> NewTextVerticalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Default")
		FTextBlockStyle NewTextNormalStyle;
};

/**
* 
*/
UCLASS(hidecategories = Object, MinimalAPI)
class UPA_HudWidget_InventoryItemStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the scoreboard's appearance. */
	UPROPERTY(EditAnywhere, Category = Appearance, meta = (ShowOnlyInnerProperties))
		FInventoryItemStyle InventoryItemStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&InventoryItemStyle);
	}

};