// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PA_HudWidget_DialogueContainer.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudWidget_DialogueContainer : public UUserWidget
{
	GENERATED_BODY()

public:


	// Conditions
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
		bool IsConditionsMetForNode(FDialogueNode Node);

	// Events
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
		void RunEventsForNode(FDialogueNode Node);

	////Adding and managing dialogue bubbles
	//UFUNCTION(BlueprintCallable, Category = "Dialogue")
	//	void AddDialogueInstance(class UPA_HudWidget_DialogueBubble* Instance);

	//UFUNCTION(BlueprintPure, Category = "Dialogue")
	//	class UPA_HudWidget_DialogueBubble* GetDialogueInstanceByNode(FDialogueNode Node);

	//UFUNCTION(BlueprintPure, Category = "Dialogue")
	//	class UPA_HudWidget_DialogueBubble* GetDialogueInstanceByOwner(class AActor* Owner);

	//UFUNCTION(BlueprintPure, Category = "Dialogue")
	//	class AActor* GetDialogueInstanceOwnerByNode(FDialogueNode Node);	
};
