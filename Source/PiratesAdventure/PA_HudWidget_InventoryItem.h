// Copyright belongs to Digital Hollywood and Michael Brandse

/*
TODO:
Inventory Item base will handle all the various actions associated with a normal Inventory Item. That includes:
-Item Presence in the menu itself
--Item icon
--Item states (normal, rollover, active (and/or equipped), inactive)
--Item name (translatable)
--Item ammo (if applicable)

-Item Popup Small
--small extended description floater

-Item Popup Large
--New popup window
---3D Model Viewer
---Description

CONSIDER MAKING THIS A BASE CLASS AND MAKE VARIATIONS ON ITEMS FROM HERE


*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PA_Asset_Item.h"
#include "PA_HudWidget_InventoryItemStyle.h"
#include "SPA_HudWidget_InventoryItemSlate.h"
#include "PA_HudWidget_InventoryItem.generated.h"

/**
* Creates an item icon container.
* - No Child
* - Has 3 states; normal,selected and inactive
* - Can have a separate ammo counter
*/
UCLASS(meta = (DisplayName = "Inventory Item"))
class PIRATESADVENTURE_API UPA_HudWidget_InventoryItem : public UWidget
{
	GENERATED_BODY()

public:

	UPA_HudWidget_InventoryItem(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
		UPA_Asset_Item* ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Appearance", meta = (sRGB = "true"))
		FLinearColor WidgetColorAndOpacity = FLinearColor::White;

	/** The button style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Appearance", meta = (DisplayName = "Inventory Item Style"))
		FInventoryItemStyle InventoryItemStyle;

	UPROPERTY()
		FGetLinearColor WidgetColorAndOpacityDelegate;

	UPROPERTY()
		FSlateBrush BackgroundBrush;

	UPROPERTY()
		FGetSlateBrush BackgroundBrushDelegate;

	UPROPERTY()
		FSlateBrush AmmoBrush;

	UPROPERTY()
		FGetSlateBrush AmmoBrushDelegate;

	UPROPERTY()
		FText TitleText = FText::FromString("");

	UPROPERTY()
		int32 AmmoCount = INDEX_NONE;

	//UPROPERTY()
	//	FText NewText = FText::FromString("");

	//Itembox related functions for determining shape
	FMargin GetAmmoMargin(FMargin MainMargin, FVector2D MainSize, FMargin CurrentMargin, FVector2D CurrentSize);
	void SetIcons();

	//Itembox related functions for blueprint
	//UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Item State"))
	//	void SetItemState(EItemIconState NewState) { WidgetStyle.ItemState = NewState; RebuildWidget(); };

	//UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Get Item State"))
	//	EItemIconState GetItemState() { return WidgetStyle.ItemState; };

	//UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Has Ammo"))
	//	void SetHasAmmo(bool bNewState) { WidgetStyle.bHasAmmo = bNewState; };

	//UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Background Tile"))
	//	UMaterialInstanceDynamic* SetBackgroundIconTile(UMaterialInterface* TilingMaterial, UTexture2D* TilingTexture, FVector2D TileCoordinates);

	//UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Ammo Tile"))
	//	UMaterialInstanceDynamic* SetAmmoIconTile(UMaterialInterface* TilingMaterial, UTexture2D* TilingTexture, FVector2D TileCoordinates);

	UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Widget Color and Opacity"))
		void SetWidgetColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Widget Opacity"))
		void SetWidgetOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Background Icon Color and Opacity"))
		void SetBackgroundColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Ammo Icon Color and Opacity"))
		void SetAmmoColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "Set Ammo Count"))
		void SetAmmoCount(int32 NewValue);

	UFUNCTION(BlueprintPure, Category = "Item", meta = (DisplayName = "Get Ammo Count"))
		int32 GetAmmoCount();

	UFUNCTION(BlueprintCallable, Category = "Item")
		void SetTitleText(FText NewText);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void SetInventoryItemState(EInventoryItemState NewState);

	UFUNCTION(BlueprintCallable, Category = "Item")
		EInventoryItemState GetInventoryItemState();

	void SetInventoryStyle(FInventoryItemStyle NewStyle);
	void SetSize(FVector2D NewSize);
	void SetScale(FVector2D NewScale);

protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	//virtual void PostLoad() override;
	const FSlateBrush* ConvertBackgroundImage(TAttribute<FSlateBrush> InImageAsset) const;
	const FSlateBrush* ConvertAmmoImage(TAttribute<FSlateBrush> InImageAsset) const;
	bool BackgroundCanTile();
	bool AmmoCanTile();

	TSharedPtr<class SPA_HudWidget_InventoryItemSlate> MyInventoryItem;
};