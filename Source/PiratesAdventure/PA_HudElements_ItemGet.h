// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_ItemGet.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudElements_ItemGet : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void NativeConstruct();
	void ConstructItemPopup(class UPA_Asset_Item* NewItemData);
	void DeconstructItemPopup();
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	//------------------------------------------------------------------------------
	// ITEM POPUP BASIC
	//------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Item Get - Basic")
		class UPA_Asset_Item* ItemData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item Get - Basic")
		FMargin PopupScreenMargin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item Get - Basic")
		FVector2D OverallWindowScale;

	UPROPERTY()
		EHudElementState HudElementState = EHudElementState::HES_NoLoop;

	//If we use this variable, we can also use curve floats.
	UPROPERTY()
		float BGStateProgress;

	UPROPERTY()
		float IconStateProgress;

	//To make sure we don't keep setting the position in the tick
	UPROPERTY()
		bool bInitialized;

	//------------------------------------------------------------------------------
	// ITEM POPUP VISUAL ELEMENTS
	//------------------------------------------------------------------------------

	//Main Panel is just to house all elements so we can easily move it or set it to invisible or something
	class UCanvasPanel* MainPanel;

	class UCanvasPanelSlot* MainPanelSlot;

	//since we need to animate the panels, we add them here.
	class UCanvasPanel* PopupBGPanel;

	class UCanvasPanel* IconBGPanel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item Get - Visual")
		FVector2D PopupBGSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item Get - Visual")
		class UMaterialInterface* PopupBGMaterialTemplate;

	//Influences how the appearance of the window is animated. If no curve is used, the animation will be a sample scaling from 0 - 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Visual")
		class UCurveFloat* PopupBGAnimationIntroCurve;

	//Influences how the disappearance of the window is animated. If no curve is used, the animation will be a sample scaling from 1 - 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Visual")
		class UCurveFloat* PopupBGAnimationOuttroCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item Get - Visual")
		FVector2D IconBGSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item Get - Visual")
		class UMaterialInterface* IconBGMaterialTemplate;

	//Influences how the appearance of the window is animated. If no curve is used, the animation will be a sample scaling from 0 - 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Visual")
		class UCurveFloat* IconBGAnimationIntroCurve;

	//Influences how the disappearance of the window is animated. If no curve is used, the animation will be a sample scaling from 1 - 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Visual")
		class UCurveFloat* IconBGAnimationOuttroCurve;

	//------------------------------------------------------------------------------
	// ITEM POPUP TEXT VARIABLES
	//------------------------------------------------------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FVector2D TitleTextSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FSlateFontInfo TitleTextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FLinearColor TitleTextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FVector2D TitleTextShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FLinearColor TitleTextShadowColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		TEnumAsByte<enum ETextJustify::Type> TitleTextJustification;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FVector2D FlavorTextSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FSlateFontInfo FlavorTextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FLinearColor FlavorTextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FVector2D FlavorTextShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FLinearColor FlavorTextShadowColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		TEnumAsByte<enum ETextJustify::Type> FlavorTextJustification;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		float TextLeftMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		float TitleTextTopMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		FVector2D DividerSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		float DividerTopMargin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Get - Text")
		float FlavorTextTopMargin;
};
