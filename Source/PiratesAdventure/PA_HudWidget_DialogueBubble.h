// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dialogue.h"
#include "PA_HudWidget_DialogueBubble.generated.h"

/**
 * This class will be a basic groundwork class for all dialogue bubbles. Since we cannot extend Blueprint Widgets, but still need to 
 * have some functionality that is shared between different bubble visualizations, we set this basis up in C++ at least. 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_HudWidget_DialogueBubble : public UUserWidget
{
	GENERATED_BODY()

public:

	//Internal variables
	float CurrentTextSpeed;
	float OldTextSpeed;
	bool bProcessingSpeedEvent = false;

	//Variables with expose on spawn on
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Dialogue", meta = (ExposeOnSpawn = "true"))
		FDialogueNode DialogueNode;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Dialogue", meta = (ExposeOnSpawn = "true"))
		class AActor* FollowActor;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Dialogue", meta = (ExposeOnSpawn = "true"))
		class UUserWidget* DialogueOwnerWidget;

	//Other variables
	//If true, skips spaces when incrementing text, so we only "see" words appearing
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
		bool bSkipSpaces = true;

	UPROPERTY()
		FText Fulltext;

	UPROPERTY()
		int32 TextIndex;

	UPROPERTY()
		FTimerHandle TextIncrementTimer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
		FMargin ViewportMargins = FMargin(25.f);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Dialogue")
		FVector2D WindowSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
		FVector2D WindowOffset;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Dialogue")
		class UMaterialInstanceDynamic* DynMat;

	//Implementable functions
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void SetupText(float TextSpeed = 0.05);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "Setup Text"))
		void OnReceiveSetupText();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialogue")
		void CloseWindow();

	//Advances text to full or the next speed event. Useful for when pressing the button needs to advance text instantly.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void AdvanceText();

	//This boolean is to make sure we go back to max speed after speed events, in the case we have advanced the text. 
	//It can also be used to confirm whether we can click to reach the next node.
	bool bTextHasAdvanced = false;

	//Returns the screenposition of the dialogue, based on FollowActor. If bUseCamDistance is set to true, the Z axis will have Camera Distance. If bApplyZCorrection is true, we try to get the point above the actor instead
	UFUNCTION(BlueprintPure, Category = "Dialogue")
		FVector GetDialoguePosition(bool bApplyZCorrection = true, bool bOutputCamDistance = false);

	void IncrementText();

	void EvaluateTextEvents();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialogue", meta = (DisplayName = "Increment Text"))
		void OnReceiveIncrementText(const FString& NewText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
		void StartTextEvent(FName EventName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void SetTextSpeed(float NewSpeed);

	UFUNCTION(BlueprintPure, Category = "Dialogue")
		bool IsTextComplete();

	//Choice related functionality
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Dialogue", meta = (ExposeOnSpawn = "true"))
		TArray<FDialogueNode> ChoiceNodes;

	UPROPERTY()
		int32 ChoiceIndex;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void SetupChoiceNodes(TArray<FDialogueNode> NewChoiceNodes);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "Setup Choice Nodes"))
		void OnReceiveSetupChoiceNodes(TArray<FDialogueNode>& NewChoiceNodes);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void MoveChoiceUp();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void MoveChoiceDown();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void FocusChoice();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "Focus Choice"))
		void OnReceiveFocusChoice(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void SelectChoice();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "Select Choice"))
		void OnReceiveSelectChoice(int32 Index);
};
