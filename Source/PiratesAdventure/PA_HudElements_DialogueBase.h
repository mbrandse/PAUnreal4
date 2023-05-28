// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "Blueprint/UserWidget.h"
#include "Dialogue.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_DialogueBase.generated.h"

/**
 * 
 */

USTRUCT(Blueprintable)
struct FDialogueWindowData
{
	GENERATED_USTRUCT_BODY()

public:
	//The maximum number of text that can be present in this window at once. Used to determine the window size type.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxCharacters;

	//the size of the dialogue window
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D WindowSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMargin WindowMargin;

	//the frontal material for the dialogue window. By default this should be present for the dialogue window to show.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInterface* MaterialTemplate;

	//the dynamic instance of the material for animation
	UPROPERTY()
		class UMaterialInstanceDynamic* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D TextBlockPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D TextBlockSize;
};

USTRUCT()
struct FChoiceLine
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
		class UVerticalBoxSlot* PanelSlot;

	UPROPERTY()
		class UTextBlock* Text;

	UPROPERTY()
		class UMaterialInstanceDynamic* ChoiceBG;
};


UCLASS(abstract)
class PIRATESADVENTURE_API UPA_HudElements_DialogueBase : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
public:
	//Initialize dialogue functionality. This is because we start out without nodes, and the first one is slightly different as it also needs to initiate animation.
	void InitializeDialogue();

	//For times when we spawn dialogue that doesnt progress as default.
	void FinalizeDialogue();

	UPROPERTY()
		bool bFinalizeDialogueCalled = false;
	
	//Proceed conversation functionality
	void NextDialogueNode(FDialogueNode Node);

	//checks whether the next part of the dialogue is spoken by someone else. Technically also returns the final node. 
	bool ShouldChangeSpeaker(FDialogueNode Node);

	//checks whether we have reached the final node in our tree and the next one will close it.
	bool IsFinalNode(FDialogueNode Node);

	//Add a character to text, for JRPG dialogue progression style;
	void IncrementText();

	//If the text is still incrementing, and the player presses the button again, fill it instantly.
	void FinalizeText();

	//In case we have a choice window, we don't want the player to accidentally press it away (since choices don't increment). Instead, we will force the animation to 100%
	void FinalizeChoice();

	FTimerHandle TickDialogueTextHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<AActor*> NPCs;

	//Our current window data.
	UPROPERTY()
		FDialogueWindowData WindowData;

	//Our current dialogue data.
	UPROPERTY()
		class UDialogue* Dialogue;

	//Our current dialogue node.
	UPROPERTY()
		TArray<FDialogueNode> CurrentDialogueNodes;

	//-----------------------------------------------------------------------------------------------------------------------------
	// NODE DURATION DATA
	//-----------------------------------------------------------------------------------------------------------------------------

	FTimerHandle DialogueNodeDurationHandle;

	void SetNextNodeAfterDuration();

	//-----------------------------------------------------------------------------------------------------------------------------
	// MAIN WINDOW DATA
	//-----------------------------------------------------------------------------------------------------------------------------

	//We will use a separate canvas panel so we can alter the orientation as well as position of the dialogue window as a whole.
	UPROPERTY()
		class UCanvasPanel* MainPanel;

	UPROPERTY()
		class UCanvasPanelSlot* MainPanelSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		TArray<FDialogueWindowData> DialogueWindows;

	//Influences how the appearance of the window is animated. If no curve is used, the animation will be a sample scaling from 0 - 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		class UCurveFloat* AnimationIntroCurve;

	//Influences how the disappearance of the window is animated. If no curve is used, the animation will be a sample scaling from 1 - 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		class UCurveFloat* AnimationOuttroCurve;

	UPROPERTY()
		EHudElementState HudElementState = EHudElementState::HES_NoLoop;

	//If we use this variable, we can also use curve floats.
	UPROPERTY()
		float HudElementStateProgress;

	//if our window is delayed, we should use this to make sure our Tick doesn't start just yet.
	UPROPERTY()
		bool bIsDelayed = false;

	UPROPERTY()
		FTimerHandle DelayHandle;

	UFUNCTION()
		void StartDelayedDialogue();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FVector2D UpdateTailPosition;

	//To reverse the tail's facing (either right or left)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FName TailIsReversedParameter;

	//To set whether the tail is top or bottom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FName TailIsBottomParameter;

	//To set the position of the tail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FName PositionTailParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FVector2D WindowScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FVector2D MinimumPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		FVector2D MaximumPivot;

	//Determines the angle at which we wish to offset the dialogue window from the character's position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		float ScreenPositionOffsetAngle;

	//Determines the distance at which we wish to offset the dialogue window from the character's position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Window")
		float ScreenPositionOffsetDistance;

	//Using the Min and Max pivot, we will calculate this base offset
	UFUNCTION()
		FVector2D CalculateBaseOffset();

	//-----------------------------------------------------------------------------------------------------------------------------
	// TEXT BLOCK
	//-----------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		class UBorder* TextPanel;

	UPROPERTY()
		class UPA_UMG_WrappedTextBlock* TextBlock;

	UPROPERTY()
		class UCanvasPanelSlot* TextBlockSlot;

	UPROPERTY()
		TArray<FText> Text; //consider changing this at a later point, if we happen to be able to integrate a dialogue editor into the engine.

	UPROPERTY()
		int32 TextIndex; //The index at which the text is. Used to determine the size of the text box as well.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Text")
		FSlateFontInfo TextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Text")
		FLinearColor TextColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Text")
		FVector2D TextShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Text")
		FLinearColor TextShadowColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Text")
		TEnumAsByte<enum ETextJustify::Type> TextJustification;

	//-----------------------------------------------------------------------------------------------------------------------------
	// CHOICE WINDOW DATA
	//-----------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		TArray<FChoiceLine> Choices;

	UPROPERTY()
		class UVerticalBox* ChoicePanel;

	//The position of the choice box in relation to the text balloon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FVector2D ChoicePosition;

	//The size of a single choice button; the total size depends on the number of choices.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FVector2D ChoiceSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		class UMaterialInterface* ChoiceBGMaterialTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FName ChoiceBGActiveParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		TEnumAsByte<enum ETextJustify::Type> ChoiceTextJustification;

	//The text for the currently active chosen answer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FSlateFontInfo ChoiceFocusTextFont;

	//The text for the currently active chosen answer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FLinearColor ChoiceFocusTextColorAndOpacity;

	//The text for the currently active chosen answer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FVector2D ChoiceFocusTextShadowOffset;

	//The text for the currently active chosen answer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FLinearColor ChoiceFocusTextShadowColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FVector2D ChoiceTextPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FVector2D ChoiceTextSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Choice")
		FMargin ChoiceTextMargin;

	UFUNCTION()
		void GenerateChoices();

	UFUNCTION()
		void ClearChoices();

	UFUNCTION()
		void FocusChoice(uint32 IndexToFocus);

	UPROPERTY()
		int32 CurrentChoiceIndex;

	//-----------------------------------------------------------------------------------------------------------------------------
	// BACK AND FOREGROUND WINDOW
	//-----------------------------------------------------------------------------------------------------------------------------
	
	UPROPERTY()
		class UImage* Window;

	UPROPERTY()
		class UCanvasPanelSlot* WindowSlot;

	//-----------------------------------------------------------------------------------------------------------------------------
	// CONTINUE ICON
	//-----------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		class UImage* ContinueImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Continue Icon")
		FVector2D ContinueSize;

	//The position of the continue image, taken from the bottom right of the window.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Continue Icon")
		FVector2D ContinuePosition;

	//The icon shown when there is still text left, and a button press will initiate the next set of dialogue(only for the owner)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Continue Icon")
		class UMaterialInterface* ContinueMaterialTemplate;

	UPROPERTY()
		class UCanvasPanelSlot* ContinueImageSlot;

	//-----------------------------------------------------------------------------------------------------------------------------
	// POSITION SETTINGS
	//-----------------------------------------------------------------------------------------------------------------------------

	//This is to reduce needless repositioning of the window. 
	UPROPERTY()
		FVector2D OldScreenPosition;

	//If set to true, the dialogue window will follow the owner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Position")
		bool bFollowOwner;

	//if set to true, the dialogue window will follow the owner even if the owner is not onscreen anymore. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Position")
		bool bFollowOwnerOffscreen;

	//If bFollowOwner is set to true, the OwnerTextOffset will offset the window position, using the owner as center. X is distance, Y is radius.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Position")
		FVector2D OwnerTextOffset;

	//Scales the dialogue based on distance to the main character. If exceeding the max scale, the dialogue is terminated.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue - Position")
		FVector2D MinMaxScaleDistance;

	//Sets the speed at which dialogue is filled into the dialogue window. 0 means instantaneous.
	UPROPERTY()
		float DialogueSpeed;

	//UFUNCTION()
	//	void UpdateTailPosition(float Quadrant);

	UFUNCTION()
		FDialogueWindowData FetchWindowSize(int32 CharacterCount);

	UFUNCTION()
		bool IsInteractiveDialogue() { return Dialogue != NULL; };

	//-----------------------------------------------------------------------------------------------------------------------------
	// DIALOGUE EVENTS
	//-----------------------------------------------------------------------------------------------------------------------------

	UFUNCTION()
		void CallEventsAtStartNode(FDialogueNode Node);

	UFUNCTION()
		void CallEventsAtEndNode(FDialogueNode Node);

	//-----------------------------------------------------------------------------------------------------------------------------
	// DIALOGUE CONDITIONS
	//-----------------------------------------------------------------------------------------------------------------------------

	//UFUNCTION()
		bool ConditionsAreMet(class UDialogueConditions* Conditions);

	//-----------------------------------------------------------------------------------------------------------------------------
	// MISC
	//-----------------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		class APA_PlayerHUD* OwnerHUD;
};
