// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PiratesAdventureHUD.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHudType : uint8
{
	HT_RINGMENU UMETA(DisplayName = "Ring Menu"),
	HT_MAPMENU UMETA(DisplayName = "Map Menu"),
	HT_ITEMMENU UMETA(DisplayName = "Item Menu"),
	HT_INGAMEMENU UMETA(DisplayName = "Ingame Menu")
};

UCLASS()
class PIRATESADVENTURE_API APiratesAdventureHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	//TODO: HAVE SEPERATE ACTIVATION FUNCTIONS FOR DIALOGUE AND CONTEXT, SINCE THEIR INPUT IS DIFFERENT
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
		void OpenMenu(EHudType HUDType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
		void CloseMenu();

	//TODO: DO WE NEED SOMETHING TO SWAP MENUS? FOR THE TIME BEING WE DON'T. 
	
	
	
};
