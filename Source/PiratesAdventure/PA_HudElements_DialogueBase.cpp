// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_HudElements_DialogueBase.h"
#include "PiratesAdventure.h"
#include "PA_UMG_WrappedTextBlock.h"
#include "WidgetLayoutLibrary.h"
#include "PiratesAdventureCharacter.h"
//#include "CoreMinimal.h"
//#include "UObject/Object.h"
//#include "UObject/WeakObjectPtr.h"
//#include "NameTypes.h"
#include "Dialogue.h"
#include "PA_PlayerHUD.h" //include this for access to the animation ENUM

/* TODOLIST FOR DIALOGUE

1. Make sure dialogue windows change their "shape" depending on their location in the screen (if out of bounds, make sure to alter the shape to fit it into the main viewport)
1b. Make it owner dependent. For testing we use the main character, but everything should be based on the owner.
2. Make sure dialogue window change their "shape" depending on the heading of the owner (after 1 is taken into account); alter tail position based on forward vector as well.
2b. Make sure to adjust the position of the window first before setting it to a different window/position. This means you will have to alter the tail position first before
altering the material.
3. Add dialogue with choice options
4. Intro/outtro animation
5. Automatically remove dialogue that goes out of bounds during the course of the dialogue (animation)
6. Add "auto-fill-in"
7. Possibly add or use existing dialogue editor functionality to create new dialogues (not the array method you used before). If impossible, get yourself the dialogue plugin and 
base it on that.
8. Add dialogue with variable choice option (choose from a list from an array)
9. Add a distance modifier to it, so we can remove the dialogue if the player is too far away from it.

*/

void UPA_HudElements_DialogueBase::NativeConstruct()
{
	Super::NativeConstruct();

	//First set the basic variables
	bFollowOwner = true;

	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());

	if (!RootPanel)
	{
		UE_LOG(AnyLog, Warning, TEXT("NO ROOTPANEL"));
	}

	if (RootPanel)
	{
		//first determine if our window sizes are not zero and get ourselves the biggest window
		//TODO: make sure this calculation is done based on the window size
		//TODO: move the size dependent variables (such as text size and position) into the struct
		if (DialogueWindows.Num() != 0)
		{
			//FVector2D DefaultWindowSize;
			//for (int32 DIndex = 0; DIndex < DialogueWindows.Num(); DIndex++)
			//{
			//	if (DefaultWindowSize.Size() == 0.f || DefaultWindowSize.Size() < DialogueWindows[DIndex].WindowSize.Size())
			//	{
			//		DefaultWindowSize = DialogueWindows[DIndex].WindowSize;
			//		WindowData = DialogueWindows[DIndex];
			//	}
			//}

			//if our biggest window equals zero, we don't need to continue.
			//if (WindowData.WindowSize.Size() == 0.f)
			//	return;

			WindowData = DialogueWindows[0];
		}
		else
		{
			UE_LOG(AnyLog, Log, TEXT("NO WINDOW DATA"));
		}

		MainPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
		TextBlock = NewObject<UPA_UMG_WrappedTextBlock>(UPA_UMG_WrappedTextBlock::StaticClass());
		if (!MainPanel || !TextBlock)
			return;

		//first set up the main panel.
		MainPanelSlot = RootPanel->AddChildToCanvas(MainPanel);
		MainPanelSlot->SetSize(WindowData.WindowSize);
		MainPanelSlot->SetPosition(FVector2D(500, 500));
		MainPanel->SetRenderScale(WindowScale); //initial state for animation

		//Second, set up the dialogue text.
		TextBlock->SetColorAndOpacity(TextColorAndOpacity);
		TextBlock->SetShadowOffset(TextShadowOffset);
		TextBlock->SetShadowColorAndOpacity(TextShadowColorAndOpacity);
		TextBlock->SetJustification(TextJustification);
		TextBlock->Font = TextFont;
		TextBlock->SetVisibility(ESlateVisibility::Visible);
		FText TempText = NSLOCTEXT("Test", "Test", "THIS IS A TEST TEXT.");
		TextBlock->SetText(TempText);

		TextBlockSlot = MainPanel->AddChildToCanvas(TextBlock);
		TextBlockSlot->SetSize(WindowData.TextBlockSize);
		TextBlockSlot->SetPosition(WindowData.TextBlockPosition);
		TextBlockSlot->SetZOrder(1);

		//Generate our window images just in case, but hide them when we don't have a proper material.
		Window = NewObject<UImage>(UImage::StaticClass());
		Window->Brush.DrawAs = ESlateBrushDrawType::Image;
		Window->Brush.ImageSize = WindowData.WindowSize;
		if (WindowData.MaterialTemplate)
		{
			Window->SetBrushFromMaterial(WindowData.MaterialTemplate);
			WindowData.Material = Window->GetDynamicMaterial();

			//Since we have a material, we may as well set the pivot now.
			float CurrentPivotPos;
			WindowData.Material->GetScalarParameterValue(PositionTailParameter, CurrentPivotPos);
			MainPanel->SetRenderTransformPivot(FMath::Lerp(MinimumPivot, MaximumPivot, CurrentPivotPos));
		}
		//else
		//{
		//	Window->SetVisibility(ESlateVisibility::Hidden);
		//}
		WindowSlot = MainPanel->AddChildToCanvas(Window);
		WindowSlot->SetSize(WindowData.WindowSize);
		WindowSlot->SetZOrder(0);

		if (ContinueMaterialTemplate
			&& ContinueSize != FVector2D::ZeroVector)
		{
			ContinueImage = NewObject<UImage>(UImage::StaticClass());
			if (ContinueImage != NULL)
			{
				FAnchors ContinueImageAnchors;
				ContinueImageAnchors.Minimum = FVector2D(1, 1);
				ContinueImageAnchors.Maximum = FVector2D(1, 1);

				ContinueImage->Brush.DrawAs = ESlateBrushDrawType::Image;
				ContinueImage->Brush.ImageSize = ContinueSize;
				ContinueImage->SetBrushFromMaterial(ContinueMaterialTemplate);
				ContinueImage->SetVisibility(ESlateVisibility::Visible);

				ContinueImageSlot = MainPanel->AddChildToCanvas(ContinueImage);
				ContinueImageSlot->SetAutoSize(true);
				ContinueImageSlot->SetAnchors(ContinueImageAnchors);
				ContinueImageSlot->SetZOrder(3);
				ContinueImageSlot->SetPosition(FVector2D::ZeroVector - ContinuePosition - ContinueSize);
			}
		}
	}
}

void UPA_HudElements_DialogueBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	//static float Tester;

	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* World = GetWorld();

	if (!World)
		return;

	//TODO: add a text-fill-up animation somewhere around here.

	//first process any window animations
	float MinTime, MaxTime;
	switch (HudElementState)
	{
	case EHudElementState::HES_StartLoop:
		if(MainPanel)
		{
			if (!bIsDelayed) //just in case. I think we only really need it for the stoploop though.
			{
				if (!AnimationIntroCurve)
				{
					HudElementStateProgress = FMath::Clamp(HudElementStateProgress + InDeltaTime, 0.f, 1.f);
					MainPanel->SetRenderScale(FMath::Lerp(FVector2D::ZeroVector, WindowScale, HudElementStateProgress));
					if (HudElementStateProgress >= 1.f)
					{
						HudElementStateProgress = 0.f;
						HudElementState = EHudElementState::HES_NoLoop;
					}
				}
				else
				{
					AnimationIntroCurve->GetTimeRange(MinTime, MaxTime);
					HudElementStateProgress = FMath::Clamp(HudElementStateProgress + InDeltaTime, MinTime, MaxTime);
					MainPanel->SetRenderScale(FVector2D(AnimationIntroCurve->GetFloatValue(HudElementStateProgress), AnimationIntroCurve->GetFloatValue(HudElementStateProgress)) * WindowScale);
					if (HudElementStateProgress >= MaxTime)
					{
						HudElementStateProgress = 0.f;
						HudElementState = EHudElementState::HES_NoLoop;
					}
				}
			}
		}
		break;

	case EHudElementState::HES_StopLoop:
		if (MainPanel)
		{
			ContinueImage->SetVisibility(ESlateVisibility::Hidden);

			UE_LOG(AnyLog, Log, TEXT("CALLED STOP LOOP ON DIALOGUE"));
			if (!AnimationOuttroCurve)
			{
				HudElementStateProgress = FMath::Clamp(HudElementStateProgress + InDeltaTime, 0.f, 1.f);
				MainPanel->SetRenderScale(FMath::Lerp(FVector2D::ZeroVector, WindowScale, 1.f - HudElementStateProgress));
				if (HudElementStateProgress >= 1.f)
				{
					HudElementStateProgress = 0.f;
					if (!IsFinalNode(CurrentDialogueNodes[CurrentChoiceIndex])
						&& !bFinalizeDialogueCalled)
					{
						if (!bIsDelayed)
						{
							HudElementState = EHudElementState::HES_StartLoop;
							NextDialogueNode(CurrentDialogueNodes[CurrentChoiceIndex]);
							CurrentChoiceIndex = 0;
						}
					}
					else
					{
						HudElementState = EHudElementState::HES_NoLoop;

						if (OwnerHUD)
							OwnerHUD->DisableInput(OwnerHUD->PlayerOwner);
						RemoveFromParent();
					}
				}
			}
			else
			{
				AnimationOuttroCurve->GetTimeRange(MinTime, MaxTime);
				HudElementStateProgress = FMath::Clamp(HudElementStateProgress + InDeltaTime, MinTime, MaxTime);
				MainPanel->SetRenderScale(FVector2D(AnimationOuttroCurve->GetFloatValue(HudElementStateProgress), AnimationOuttroCurve->GetFloatValue(HudElementStateProgress)) * WindowScale);
				if (HudElementStateProgress >= MaxTime)
				{
					HudElementStateProgress = 0.f;
					if (!IsFinalNode(CurrentDialogueNodes[CurrentChoiceIndex])
						&& !bFinalizeDialogueCalled)
					{
						if (!bIsDelayed)
						{
							HudElementState = EHudElementState::HES_StartLoop;
							NextDialogueNode(CurrentDialogueNodes[CurrentChoiceIndex]);
							CurrentChoiceIndex = 0;
						}
					}
					else
					{
						HudElementState = EHudElementState::HES_NoLoop;
						if (OwnerHUD)
							OwnerHUD->DisableInput(OwnerHUD->PlayerOwner);
						RemoveFromParent();
					}
				}
			}
		}
		break;
	}

	if (MainPanelSlot
		&& CurrentDialogueNodes.Num() != 0) //make sure we only tick when we have already called NextDialogueNode at least once
	{
		FVector2D OwnerPosition2D;
		FVector OwnerPosition;

		//TODO: Now that we have vastly simplified tail position calculation, we need to replace it with;
		//1. First we check NPC/player heading for either left or right
		//2. If left or right falls offscreen, we reverse it
		//3. then we check whether we are close to the top of the screen (for bottom window) or otherwise we make it a top window.
		//4. Recalculate pivot and position.
		//5. keep in mind that position goes from 0-1, so in a reverse pivot, we actually have to subtract from 1.

		//first check the first entry, for what it is, and update the OwnerPosition based on it.
		AActor* DialogueOwner = NULL;
		if(CurrentDialogueNodes[0].NodeOwner == EDialogueNodeOwner::DNO_PLAYER)
		{
			DialogueOwner = OwnerHUD->OwnerCharacter;
			ACharacter* Character = Cast<ACharacter>(DialogueOwner);
			if (Character)
			{
				OwnerPosition = Character->GetActorLocation();
				OwnerPosition.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			}

			//TODO, add a separate conditional here for choices!
		}
		else
		{
			//first see if we actually have an NPC
			if (NPCs.IsValidIndex(CurrentDialogueNodes[0].NPCIndex)
				&& NPCs[CurrentDialogueNodes[0].NPCIndex] != NULL)
			{
				DialogueOwner = NPCs[CurrentDialogueNodes[0].NPCIndex];
				ACharacter* Character = Cast<ACharacter>(DialogueOwner);
				if (Character)//if we can cast it to character, we want to use its capsulecomponent, since bounds act weird for characters.
				{
					OwnerPosition = Character->GetActorLocation();
					OwnerPosition.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				}
				else //if it's not a character, we can just use the bounds instead. 
				{
					FVector CBounds, COrigin;

					OwnerPosition = DialogueOwner->GetActorLocation();
					DialogueOwner->GetActorBounds(false, COrigin, CBounds);
					OwnerPosition.Z += CBounds.Z;
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("The dialogue couldn't find an NPC at index %d. Please update the NPC array at the Spawn Dialogue node."), CurrentDialogueNodes[0].NPCIndex));
			}
		}

		//if at this point we don't have a dialogue owner, we have to terminate.
		if (!DialogueOwner)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("An owner for the dialogue window could not be assigned.")));
			return;
		}

		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(UGameplayStatics::GetPlayerController(World, 0), OwnerPosition, OwnerPosition2D);

		//since the player's screen coordinates don't change since the player is the camera's focus, make sure the player is always updated.
		if (OldScreenPosition != OwnerPosition2D || DialogueOwner == OwnerHUD->OwnerCharacter)
		{
			OldScreenPosition = OwnerPosition2D;

			//float Radians, XOffset, YOffset, WindowAngle;
			FVector2D NewAlignment = FVector2D::ZeroVector;

			////first get the initial windowangle angle, either based on following the owner, or simply by placing it overhead.
			//if (bFollowOwner)
			//{
			//	WindowAngle = DialogueOwner->GetActorForwardVector().Y;
			//		
			//	XWeight = FMath::Abs(WindowAngle); //gives us a value of 1-0-1.
			//	YWeight = 1.f - FMath::Abs(WindowAngle); //gives us a value of 0-1-0.

			//	WindowAngle = 2.f - (WindowAngle + 1.f);
			//	WindowAngle = 225.f + (WindowAngle * 45.f); //make sure the window is always on top of the owner, either to the right or left.
			//}
			//else
			//{
			//	WindowAngle = 270.f;
			//	XWeight = 0.f; //no rotation when it's just stationary
			//	YWeight = 0.f;
			//}

			//Radians = FMath::Fmod(WindowAngle, 360.f) * (PI / 180.f);
			//XOffset = (OwnerTextOffset.Y * XWeight) * FMath::Cos(Radians);
			//YOffset = (OwnerTextOffset.Y * YWeight) * FMath::Sin(Radians);

			//OwnerPosition2D.X += XOffset;
			//OwnerPosition2D.Y = OwnerPosition2D.Y + YOffset - OwnerTextOffset.X;

			//float Quadrant = FMath::Fmod(WindowAngle, 360.f) / 90.f; //so we get a value from 0-4. Makes calculation easier.
			//if (Quadrant > 0.5 && Quadrant <= 1.5)
			//{
			//	NewAlignment.Y = 0.f;
			//	NewAlignment.X = Quadrant - 0.5f;
			//}
			//else if (Quadrant > 1.5 && Quadrant <= 2.5)
			//{
			//	NewAlignment.Y = Quadrant - 1.5f;
			//	NewAlignment.X = 1.f;
			//}
			//else if (Quadrant > 2.5 && Quadrant <= 3.5)
			//{
			//	NewAlignment.Y = 1.f;
			//	NewAlignment.X = 1.f - (Quadrant - 2.5);
			//}
			//else
			//{
			//	if (Quadrant > 3.5f && Quadrant <= 4)
			//	{
			//		NewAlignment.Y = 1.f - (Quadrant - 3.5f);
			//	}
			//	else
			//	{
			//		NewAlignment.Y = 1.f - (Quadrant + 0.5f);
			//	}

			//	NewAlignment.X = 0.f;
			//}

			//MainPanelSlot->SetAlignment(NewAlignment);
			//MainPanel->SetRenderTransformPivot(NewAlignment);
			MainPanelSlot->SetPosition(OwnerPosition2D - CalculateBaseOffset());

			//if (Tail)
			//	UpdateTailPosition(Quadrant);

			//Lastly, let's update the scale based on distance
			//See if we should base this around the edge of the screen instead.
			//if (DialogueOwner != UGameplayStatics::GetPlayerCharacter(World, 0) && MinMaxScaleDistance != FVector2D::ZeroVector)
			//{
			//	FVector PlayerPosition = UGameplayStatics::GetPlayerCharacter(World, 0)->GetActorLocation();
			//	FVector OwnerPosition = DialogueOwner->GetActorLocation();
			//	float Distance = FVector::Dist(PlayerPosition, OwnerPosition);

			//	float ScaleMod = 1.f - FMath::Clamp((Distance - MinMaxScaleDistance.X) / (MinMaxScaleDistance.Y - MinMaxScaleDistance.X), 0.f, 1.f);
			//	MainPanel->SetRenderScale(FVector2D(ScaleMod, ScaleMod));
			//}
		}
	}
}

FVector2D UPA_HudElements_DialogueBase::CalculateBaseOffset()
{
	//The current calculations assume a top right position always. Make sure this is quadrant dependent based on window position

	FVector2D WindowSize = MainPanelSlot->GetSize();
	FVector2D Offset;
	FVector2D AdditionalOffset = FVector2D::ZeroVector;
	float MaxXOffset; //since Y can only have roughly two values
	float CurrentPosition;
	float WindowIsFlipped; //Whether the window tail is up or down
	float WindowIsMirrored; //Whether the window is mirrored, which will mirror the max and min pivot as well.

	MaxXOffset = (MaximumPivot.X * WindowSize.X) - (MinimumPivot.X * WindowSize.X);

	if (WindowData.Material)
	{
		//We need the material to determine the offset
		WindowData.Material->GetScalarParameterValue(PositionTailParameter, CurrentPosition);
		WindowData.Material->GetScalarParameterValue(TailIsBottomParameter, WindowIsFlipped);
		WindowData.Material->GetScalarParameterValue(TailIsReversedParameter, WindowIsMirrored);
	}
	else
	{
		//break off the operation if we don't have the proper variables
		return FVector2D::ZeroVector;
	}

	//First calculate pivot based on material variables
	Offset.X = WindowIsMirrored != 0.f ? 
		(WindowSize.X * MinimumPivot.X) + (MaxXOffset * CurrentPosition) : 
		(WindowSize.X * MaximumPivot.X) - (MaxXOffset * (1.f - CurrentPosition));

	Offset.Y = WindowIsFlipped != 1.f ? (WindowSize.Y * (1.f - MaximumPivot.Y)) : (WindowSize.Y * MaximumPivot.Y);
	//Offset.Y = 0;

	if (ScreenPositionOffsetDistance != 0.f)
	{
		float Radians = ScreenPositionOffsetAngle * (PI / 180.f);
		AdditionalOffset.X = ScreenPositionOffsetDistance * FMath::Cos(Radians);
		AdditionalOffset.Y = ScreenPositionOffsetDistance * FMath::Sin(Radians);
	}
 
	//then simply return  the pivot multiplied by window size
	return Offset + AdditionalOffset;
	
}

//void UPA_HudElements_DialogueBase::UpdateTailPosition(float Quadrant)
//{
//	FVector2D NewPosition, NewPositionCornerOffset;
//	float NewAngle;
//
//	//first get the location value
//	if (Quadrant > 0.5 && Quadrant <= 1.5)
//	{
//		NewPosition.Y = 0.f;
//		NewPosition.X = Quadrant - 0.5f;
//	}
//	else if (Quadrant > 1.5 && Quadrant <= 2.5)
//	{
//		NewPosition.Y = Quadrant - 1.5f;
//		NewPosition.X = 1.f;
//	}
//	else if (Quadrant > 2.5 && Quadrant <= 3.5)
//	{
//		NewPosition.Y = 1.f;
//		NewPosition.X = 1.f - (Quadrant - 2.5);
//	}
//	else
//	{
//		if (Quadrant > 3.5f && Quadrant <= 4)
//		{
//			NewPosition.Y = 1.f - (Quadrant - 3.5f);
//		}
//		else
//		{
//			NewPosition.Y = 1.f - (Quadrant + 0.5f);
//		}
//
//		NewPosition.X = 0.f;
//	}
//
//	NewPositionCornerOffset = FVector2D::ZeroVector;
//	if (TailCornerOffset != FVector2D::ZeroVector)
//	{
//		float QuadrantRem;
//		if (Quadrant >= 0.25 && Quadrant <= 0.75)
//		{
//			QuadrantRem = 1.f - (FMath::Abs(-0.25 + (Quadrant - 0.25)) * 4.f);
//			NewPositionCornerOffset.Y = 0.f - QuadrantRem * TailCornerOffset.Y;
//			NewPositionCornerOffset.X = 0.f - QuadrantRem * TailCornerOffset.X;
//		}
//		else if (Quadrant >= 1.25 && Quadrant <= 1.75)
//		{
//			QuadrantRem = 1.f - (FMath::Abs(-0.25 + (Quadrant - 1.25)) * 4.f);
//			NewPositionCornerOffset.Y = 0.f - QuadrantRem * TailCornerOffset.Y;
//			NewPositionCornerOffset.X = QuadrantRem * TailCornerOffset.X;
//		}
//		else if (Quadrant >= 2.25 && Quadrant <= 2.75)
//		{
//			QuadrantRem = 1.f - (FMath::Abs(-0.25 + (Quadrant - 2.25)) * 4.f);
//			NewPositionCornerOffset.Y = QuadrantRem * TailCornerOffset.Y;
//			NewPositionCornerOffset.X = QuadrantRem * TailCornerOffset.X;
//		}
//		else if (Quadrant >= 3.25 && Quadrant <= 3.75)
//		{
//			QuadrantRem = 1.f - (FMath::Abs(-0.25 + (Quadrant - 3.25)) * 4.f);
//			NewPositionCornerOffset.Y = QuadrantRem * TailCornerOffset.Y;
//			NewPositionCornerOffset.X = 0.f - QuadrantRem * TailCornerOffset.X;
//		}
//	}
//
//	FVector2D MainPanelSize = MainPanelSlot->GetSize();
//	NewPosition.Y = (NewPosition.Y * (MainPanelSize.Y - TailSize.Y));
//	NewPosition.X = (NewPosition.X * (MainPanelSize.X - TailSize.X));
//	NewPosition -= NewPositionCornerOffset;
//
//	//Then get the angle value
//	if (Quadrant > 0.25 && Quadrant <= 0.75) //0.5 - 0.125 && 0.5 + 0.125
//	{
//		NewAngle = 1.f + ((Quadrant - 0.25) / 0.5);
//	}
//	else if (Quadrant > 0.75 && Quadrant < 1.25) //steady angle at 0
//	{
//		NewAngle = 2.f;
//	}
//	else if (Quadrant > 1.25 && Quadrant <= 1.75) //1.5 - 0.125 && 1.5 + 0.125
//	{
//		NewAngle = 2.f + (Quadrant - 1.25) / 0.5;
//	}
//	else if (Quadrant > 1.75 && Quadrant < 2.25) //steady angle at 90
//	{
//		NewAngle = 3.f;
//	}
//	else if (Quadrant > 2.25 && Quadrant <= 2.75) //2.5 - 0.125 && 2.5 + 0.125
//	{
//		NewAngle = 3.f + ((Quadrant - 2.25) / 0.5);
//	}
//	else if (Quadrant > 2.75 && Quadrant < 3.25) //steady angle at 180
//	{
//		NewAngle = 0.f;
//	}
//	else if (Quadrant > 3.25 && Quadrant <= 3.75) //3.5 - 0.125 && 3.5 + 0.125
//	{
//		NewAngle = (Quadrant - 3.25) / 0.5;
//	}
//	else  //steady angle at 270
//	{
//		NewAngle = 1.f;
//	}
//
//	TailSlot->SetPosition(NewPosition);
//	Tail->SetRenderAngle(NewAngle * 90.f);
//}

FDialogueWindowData UPA_HudElements_DialogueBase::FetchWindowSize(int32 CharacterCount)
{
	//TODO; actually make this fetch the window size.At the moment it doesnt do anything.

	FDialogueWindowData NewData;
	//NewData.Default();

	return NewData;
}

void UPA_HudElements_DialogueBase::InitializeDialogue()
{
	FDialogueNode FirstNode = Dialogue->GetNodeById(0);

	if (FirstNode.Links.Num() != 0)
	{
		NextDialogueNode(FirstNode);
		HudElementState = EHudElementState::HES_StartLoop;
	}
}

void UPA_HudElements_DialogueBase::FinalizeDialogue()
{
	bFinalizeDialogueCalled = true;
	HudElementState = EHudElementState::HES_StopLoop;
}

void UPA_HudElements_DialogueBase::NextDialogueNode(FDialogueNode Node)
{
	//first see if we actually have an initial node -> MOVE TO INITIALIZE FUNCTION
	//if (CurrentDialogueNodes.Num() == 0)
	//{
	//	FDialogueNode FirstNode = Dialogue->GetFirstNode();
	//	if (FirstNode.id != 0) //first is either empty or start node.
	//	{
	//		CurrentDialogueNodes.Add(FirstNode);
	//		Node = FirstNode; //for the purpose of making sure the ProgressImage is set correctly. 
	//	}
	//}
	//else 

	//first clear the timer when a duration was set
	if (GetWorld()->GetTimerManager().IsTimerActive(DialogueNodeDurationHandle))
		GetWorld()->GetTimerManager().ClearTimer(DialogueNodeDurationHandle);

	if (Node.Links.Num() != 0) //Deal with getting a new node, or deleting the dialogue window.
	{
		CurrentDialogueNodes = Dialogue->GetNextNodes(Node);
		ContinueImage->SetVisibility(ESlateVisibility::Hidden);

		//TODO: MAKE IT SO THAT WE LOOK FORWARD ONE NODE FOR NON-CHOICE SO WE CAN SEE WHETHER IT'S A QUESTION OR NOT

		//first see if we have multiple nodes, indicating either a condition or choice. 

		if (CurrentDialogueNodes.Num() > 0
			&& CurrentDialogueNodes[0].NodeOwner == EDialogueNodeOwner::DNO_CHOICE || CurrentDialogueNodes[0].Conditions.Num() != 0)
		{
			if (CurrentDialogueNodes[0].NodeOwner == EDialogueNodeOwner::DNO_CHOICE)
			{
				//We don't need to clear this one; not clearing it will have the previous text serve as the main text for the answers.
				//TextBlock->SetText(FText::GetEmpty());
				GenerateChoices();
				CurrentChoiceIndex = 0;
				FocusChoice(CurrentChoiceIndex);
				return;
			}

			//Only accept a condition based node if there's another node to pick if the condition returns false
			//consider adding a while loop here, so we can iterate a set of condition based nodes. 
			//TODO:Iterate ALL conditions!
			if (CurrentDialogueNodes[0].Conditions.Num() != 0 && CurrentDialogueNodes.IsValidIndex(1))
			{
				if (!ConditionsAreMet(CurrentDialogueNodes[0].Conditions[0])) CurrentDialogueNodes.RemoveAt(0); //If the condition is false, delete the current node so the next one becomes 0
			}
			else
				return;
		}
		else
		{
			CallEventsAtStartNode(CurrentDialogueNodes[0]);
			ClearChoices();
		}
	}
	else
	{
		return; //if we have an empty array, it means no new node so we need to get out of here. 
	}

	TextBlock->SetText(FText::GetEmpty()); //first clear any previous text, otherwise we will see that for an instant first.
	GetWorld()->GetTimerManager().SetTimer(TickDialogueTextHandle, this, &UPA_HudElements_DialogueBase::IncrementText, 0.1f, true);
}

void UPA_HudElements_DialogueBase::GenerateChoices()
{
	int32 NumberOfChoices = CurrentDialogueNodes.Num();
	if (NumberOfChoices == 0)
		return;

	if (!ChoicePanel)
	{
		UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
		if (!RootPanel) return;

		ChoicePanel = NewObject<UVerticalBox>(UVerticalBox::StaticClass());
		if (ChoicePanel)
		{
			//Match scale to dialogue
			ChoicePanel->SetRenderScale(WindowScale);

			//attach to rootpanel
			UCanvasPanelSlot* ChoicePanelSlot = RootPanel->AddChildToCanvas(ChoicePanel);
			ChoicePanelSlot->SetPosition(MainPanelSlot->GetPosition() + (ChoicePosition * WindowScale)); //TODO; Do this with a variable
			ChoicePanelSlot->SetSize(FVector2D(ChoiceSize.X, (ChoiceSize.Y + 10.f) * NumberOfChoices));
			ChoicePanelSlot->SetZOrder(2); //place it on top of everything
		}
		else
			return;
	}

	//first empty out the current array just in case.
	Choices.Empty();

	for (int32 ChoiceLineIndex = 0; ChoiceLineIndex < NumberOfChoices; ChoiceLineIndex++)
	{
		if (CurrentDialogueNodes[ChoiceLineIndex].NodeOwner == EDialogueNodeOwner::DNO_CHOICE)
		{
			UE_LOG(AnyLog, Warning, TEXT("CREATING CHOICE %d"), ChoiceLineIndex);

			FChoiceLine NewLine;
			UCanvasPanel* Panel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());

			NewLine.PanelSlot = ChoicePanel->AddChildToVerticalBox(Panel);
			NewLine.PanelSlot->Size.SizeRule = ESlateSizeRule::Automatic;
			NewLine.PanelSlot->SetHorizontalAlignment(HAlign_Fill);
			NewLine.PanelSlot->SetVerticalAlignment(VAlign_Fill);
			FSlateChildSize NewSize;
			NewSize.Value = 1.f;
			NewSize.SizeRule = ESlateSizeRule::Fill;
			NewLine.PanelSlot->SetSize(NewSize);
			NewLine.PanelSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f)); //TODO; add this as a variable, since we also need it for calculating the total size of the choicebox

			//Draw all choices as if unfocused to begin with
			NewLine.Text = NewObject<UTextBlock>(UTextBlock::StaticClass());
			NewLine.Text->SetColorAndOpacity(TextColorAndOpacity);
			NewLine.Text->SetShadowOffset(TextShadowOffset);
			NewLine.Text->SetShadowColorAndOpacity(TextShadowColorAndOpacity);
			NewLine.Text->SetJustification(ETextJustify::Left);
			NewLine.Text->Font = TextFont;
			NewLine.Text->SetVisibility(ESlateVisibility::Visible);
			NewLine.Text->SetText(CurrentDialogueNodes[ChoiceLineIndex].Text);

			UCanvasPanelSlot* TextSlot = Panel->AddChildToCanvas(NewLine.Text);
			TextSlot->SetPosition(ChoiceTextPosition);
			TextSlot->SetZOrder(1);


			if (ChoiceBGMaterialTemplate)
			{
				UImage* BGImage = NewObject<UImage>(UImage::StaticClass());
				BGImage->SetBrushFromMaterial(ChoiceBGMaterialTemplate);
				NewLine.ChoiceBG = BGImage->GetDynamicMaterial();

				UCanvasPanelSlot* ImageSlot = Panel->AddChildToCanvas(BGImage);
				ImageSlot->SetPosition(FVector2D::ZeroVector);
				ImageSlot->SetSize(ChoiceSize);
				ImageSlot->SetZOrder(0);
			}

			Choices.Add(NewLine);
		}
	}
}

void UPA_HudElements_DialogueBase::ClearChoices()
{
	if (Choices.Num() != 0)
	{
		for (int32 ChoiceLineIndex = 0; ChoiceLineIndex < Choices.Num(); ChoiceLineIndex++)
		{
			Choices[ChoiceLineIndex].Text->SetVisibility(ESlateVisibility::Hidden);
			Choices[ChoiceLineIndex].Text->RemoveFromParent();
		}

		ChoicePanel->SetVisibility(ESlateVisibility::Hidden);
		ChoicePanel->RemoveFromParent();
		Choices.Empty();
	}
}

void UPA_HudElements_DialogueBase::FocusChoice(uint32 IndexToFocus)
{
	if (Choices.Num() == 0) return;

	for (int32 ChoiceLineIndex = 0; ChoiceLineIndex < Choices.Num(); ChoiceLineIndex++)
	{
		//first unfocus the current batch.
		Choices[ChoiceLineIndex].Text->SetFont(TextFont);
		Choices[ChoiceLineIndex].Text->SetColorAndOpacity(TextColorAndOpacity);
		Choices[ChoiceLineIndex].Text->SetShadowColorAndOpacity(TextShadowColorAndOpacity);
		Choices[ChoiceLineIndex].Text->SetShadowOffset(TextShadowOffset);
		Choices[ChoiceLineIndex].PanelSlot->SetPadding(FMargin(0, 0, 0, 10.f));

		if (Choices[ChoiceLineIndex].ChoiceBG)
			Choices[ChoiceLineIndex].ChoiceBG->SetScalarParameterValue(ChoiceBGActiveParameter, 0.f);
	}

	//and then focus the new one.
	if (Choices.IsValidIndex(IndexToFocus))
	{
		Choices[IndexToFocus].Text->SetFont(ChoiceFocusTextFont);
		Choices[IndexToFocus].Text->SetColorAndOpacity(ChoiceFocusTextColorAndOpacity);
		Choices[IndexToFocus].Text->SetShadowColorAndOpacity(ChoiceFocusTextShadowColorAndOpacity);
		Choices[IndexToFocus].Text->SetShadowOffset(ChoiceFocusTextShadowOffset);
		Choices[IndexToFocus].PanelSlot->SetPadding(FMargin(30, 0, 0, 10.f));

		if (Choices[IndexToFocus].ChoiceBG)
			Choices[IndexToFocus].ChoiceBG->SetScalarParameterValue(ChoiceBGActiveParameter, 1.f);
	}
}

bool UPA_HudElements_DialogueBase::ShouldChangeSpeaker(FDialogueNode Node)
{
	if (Node.Links.Num() != 0)
	{
		for (int32 foundindex : Node.Links)
		{
			//first check whether it's a player node. Since choice nodes are always player, we simply lump them together. 
			//TODO: should we lump choices together?
			if (Dialogue->GetNodeById(Node.id).NodeOwner == EDialogueNodeOwner::DNO_PLAYER)
			{
				return Dialogue->GetNodeById(foundindex).NodeOwner == EDialogueNodeOwner::DNO_PLAYER ? false : true;
			}
			else
			{
				//if it's not a player, it's an NPC
				if (Dialogue->GetNodeById(foundindex).NodeOwner == EDialogueNodeOwner::DNO_PLAYER
					|| Dialogue->GetNodeById(foundindex).NPCIndex != Dialogue->GetNodeById(Node.id).NPCIndex)
				{
					return true;
				}
				else
					return false;
			}
		}
	}

	return true; 
}

bool UPA_HudElements_DialogueBase::IsFinalNode(FDialogueNode Node)
{
	return Node.Links.Num() == 0; //an empty array means there aren't any links.
}

void UPA_HudElements_DialogueBase::IncrementText()
{
	TextIndex++;

	//make sure spaces are skipped over
	FString CheckForSpace = CurrentDialogueNodes[0].Text.ToString().Mid(TextIndex, TextIndex + 1);
	if (CheckForSpace == " ")
		TextIndex++;

	int32 TargetTextLength = CurrentDialogueNodes[0].Text.ToString().Len();
	if (TextIndex > TargetTextLength)
	{
		//stop doing this
		GetWorld()->GetTimerManager().ClearTimer(TickDialogueTextHandle);
		TextIndex = 0;
		ContinueImage->SetVisibility(ESlateVisibility::Visible);

		if (CurrentDialogueNodes[0].Duration != 0.f)
		{
			//if our bubble is full and the bubble has a duration, set a timer
			GetWorld()->GetTimerManager().SetTimer(DialogueNodeDurationHandle, this, &UPA_HudElements_DialogueBase::SetNextNodeAfterDuration, CurrentDialogueNodes[0].Duration, true);
		}
	}
	else
	{
		FString NewString = CurrentDialogueNodes[0].Text.ToString().Left(TextIndex);
		TextBlock->SetText(FText::FromString(NewString));
	}
}

void UPA_HudElements_DialogueBase::SetNextNodeAfterDuration()
{
	if(CurrentDialogueNodes.Num() != 0)
		NextDialogueNode(CurrentDialogueNodes[0]);
}

void UPA_HudElements_DialogueBase::FinalizeText()
{
	GetWorld()->GetTimerManager().ClearTimer(TickDialogueTextHandle);
	ContinueImage->SetVisibility(ESlateVisibility::Visible);
	TextIndex = 0;

	TextBlock->SetText(CurrentDialogueNodes[0].Text);

	if (CurrentDialogueNodes[0].Duration != 0.f)
	{
		//if our bubble is full and the bubble has a duration, set a timer
		GetWorld()->GetTimerManager().SetTimer(DialogueNodeDurationHandle, this, &UPA_HudElements_DialogueBase::SetNextNodeAfterDuration, CurrentDialogueNodes[0].Duration, true);
	}
}

void UPA_HudElements_DialogueBase::FinalizeChoice()
{
	MainPanel->SetRenderScale(FVector2D(1, 1));
	HudElementState = EHudElementState::HES_NoLoop;
}

void UPA_HudElements_DialogueBase::CallEventsAtStartNode(FDialogueNode Node)
{
	if (Node.Events.Num() != 0)
	{
		//for (int32 CallbackIndex = 0; CallbackIndex < Node.Events[0]->EventCallbacks.Num(); CallbackIndex++)
		//{
		//	if (!Node.Events[0]->EventCallbacks[CallbackIndex].bCallAtEndOfNode)
		//	{
		//		//bool bCallFromLevel = Node.Events[0]->EventCallbacks[CallbackIndex].CallbackType != EDialogueEventCallbackType::DECT_ACTOR;
		//		if (Node.isPlayer)
		//		{
		//			Dialogue->CallFunctionByName(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), Node.Events[0]->EventCallbacks[CallbackIndex].FunctionName/*, bCallFromLevel*/);
		//		}
		//		else
		//		{
		//			Dialogue->CallFunctionByName(NPCs[Node.NPCIndex], Node.Events[0]->EventCallbacks[CallbackIndex].FunctionName/*, bCallFromLevel*/);
		//		}
		//	}
		//}
	}
}

void UPA_HudElements_DialogueBase::CallEventsAtEndNode(FDialogueNode Node)
{
	if (Node.Events.Num() != 0)
	{
		//for (int32 CallbackIndex = 0; CallbackIndex < Node.Events[0]->EventCallbacks.Num(); CallbackIndex++)
		//{
		//	if (Node.Events[0]->EventCallbacks[CallbackIndex].bCallAtEndOfNode)
		//	{
		//		//bool bCallFromLevel = Node.Events[0]->EventCallbacks[CallbackIndex].CallbackType != EDialogueEventCallbackType::DECT_ACTOR;
		//		if (Node.isPlayer)
		//		{
		//			Dialogue->CallFunctionByName(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), Node.Events[0]->EventCallbacks[CallbackIndex].FunctionName/*, bCallFromLevel*/);
		//		}
		//		else
		//		{
		//			Dialogue->CallFunctionByName(NPCs[Node.NPCIndex], Node.Events[0]->EventCallbacks[CallbackIndex].FunctionName/*, bCallFromLevel*/);
		//		}
		//	}
		//}
	}
}

bool UPA_HudElements_DialogueBase::ConditionsAreMet(UDialogueConditions* Conditions)
{
	//if (Conditions
	//	&& Conditions->ItemsToCheck.Num() != 0)
	//{
	//	//TODO: Make sure it checks all items. Right now it only checks the first
	//	UPA_Asset_Item* ItemToCheck = Cast<UPA_Asset_Item>(Conditions->ItemsToCheck[0]);
	//	APiratesAdventureCharacter* Character = Cast<APiratesAdventureCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//	if (Character && ItemToCheck)
	//	{
	//		return Character->HasItemInInventory(ItemToCheck);
	//	}
	//	else
	//		return false;
	//}

	return false;
}

void UPA_HudElements_DialogueBase::StartDelayedDialogue()
{
	bIsDelayed = false;
}