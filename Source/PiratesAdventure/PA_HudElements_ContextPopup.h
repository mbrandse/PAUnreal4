// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "Blueprint/UserWidget.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_ContextPopup.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudElements_ContextPopup : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	//for animating the popup
	UPROPERTY()
		EHudElementState HudElementState = EHudElementState::HES_NoLoop;

	UPROPERTY()
		class APA_PlayerHUD* OwnerHUD;
	
	UPROPERTY()
		class UImage* PopupImage;

	UPROPERTY()
		class UCanvasPanelSlot* PopupImageSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Misc")
		float FadeSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Button")
		FVector2D PopupImageSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Button")
		class UMaterialInterface* PopupImageMaterialTemplate;

	UPROPERTY()
		class UMaterialInstanceDynamic* PopupImageMaterial;

	//The parameter name in the ButtonImageMaterial for fading it in and out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Button")
		FName ParameterNameFade;

	UPROPERTY()
		class UTextBlock* PopupText;

	UPROPERTY()
		class UCanvasPanelSlot* PopupTextSlot;

	//Change text is only used when we switch from one context event to another. 
	UPROPERTY()
		class UTextBlock* ChangeText;

	UPROPERTY()
		class UCanvasPanelSlot* ChangeTextSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Text")
		FLinearColor TextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Text")
		FSlateFontInfo TextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Text")
		FVector2D TextShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Text")
		FLinearColor TextShadowColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Text")
		TEnumAsByte<enum ETextJustify::Type> TextJustification;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Context Popup - Text")
		FMargin TextMargin;

	UFUNCTION()
		void PopulateText(FText NewText, FLinearColor NewColor, bool bShowButton);

	UFUNCTION()
		void ReplaceText(FText NewText, FLinearColor NewColor, bool bShowButton);

	UFUNCTION()
		void DePopulateText();
	
};
