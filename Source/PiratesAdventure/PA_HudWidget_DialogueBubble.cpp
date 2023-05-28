// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.


#include "PA_HudWidget_DialogueBubble.h"
#include "GameFramework/Character.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UPA_HudWidget_DialogueBubble::SetupText(float TextSpeed)
{
	UWorld* World = GetWorld();
	if (World)
	{
		Fulltext = DialogueNode.TextProcessed;
		TextIndex = 0;
		CurrentTextSpeed = FMath::Clamp(TextSpeed, 0.01f, 5.f); //let's hardcode in an absolute limit.

		World->GetTimerManager().SetTimer(TextIncrementTimer, this, &UPA_HudWidget_DialogueBubble::IncrementText, CurrentTextSpeed, true);
		//TODO: add a quick check to speed events here, so we can deal with instant text speed
	}
}

void UPA_HudWidget_DialogueBubble::IncrementText()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentText = Fulltext.ToString();
		TextIndex++;

		//Evaluate text event before we do anything
		EvaluateTextEvents();

		if (TextIndex > CurrentText.Len())
		{
			World->GetTimerManager().ClearTimer(TextIncrementTimer);
			return;
		}
		else
		{
			FString NextLetter = CurrentText.Mid(TextIndex, 1);
			if (NextLetter == " " && bSkipSpaces)
			{
				TextIndex++;
				EvaluateTextEvents();
			}

			OnReceiveIncrementText(CurrentText.Mid(0, TextIndex));
		}
	}
}

void UPA_HudWidget_DialogueBubble::EvaluateTextEvents()
{
	if (DialogueNode.TextSpeedLinkers.Contains(TextIndex))
	{
		float NewSpeed = DialogueNode.TextSpeedLinkers[TextIndex];

		if (NewSpeed > 0.f)
		{
			OldTextSpeed = CurrentTextSpeed;
			SetTextSpeed(NewSpeed);
			bProcessingSpeedEvent = true;
		}
		else
		{
			bProcessingSpeedEvent = false;
			CurrentTextSpeed = OldTextSpeed;
			SetTextSpeed(CurrentTextSpeed);
		}
	}

	if (DialogueNode.TextEventLinkers.Contains(TextIndex))
	{
		StartTextEvent(DialogueNode.TextEventLinkers[TextIndex]);
	}
}

void UPA_HudWidget_DialogueBubble::SetTextSpeed(float NewSpeed)
{
	UWorld* World = GetWorld();
	if (World)
	{
		//If the timer has been deactivated, we don't want to active it again.
		if (!World->GetTimerManager().IsTimerActive(TextIncrementTimer))
			return;

		NewSpeed = FMath::Clamp(NewSpeed, 0.01f, 5.f);

		if (bProcessingSpeedEvent)
		{
			OldTextSpeed = NewSpeed;
		}
		else
		{
			if (bTextHasAdvanced)
			{
				CurrentTextSpeed = 0.01f;
			}
			else
				CurrentTextSpeed = NewSpeed;

			World->GetTimerManager().SetTimer(TextIncrementTimer, this, &UPA_HudWidget_DialogueBubble::IncrementText, CurrentTextSpeed, true);
		}
	}
}

void UPA_HudWidget_DialogueBubble::AdvanceText()
{
	//This technically doesn't advance the text. Rather, it just speeds things up.
	UWorld* World = GetWorld();
	if (World && !bTextHasAdvanced)
	{
		if (World->GetTimerManager().IsTimerActive(TextIncrementTimer))
		{
			if (DialogueNode.TextSpeedLinkers.Num() == 0)
			{
				//We likely don't need to set the index, but let's do it just in case
				FString TempString = Fulltext.ToString();
				TextIndex = TempString.Len();

				World->GetTimerManager().ClearTimer(TextIncrementTimer);
				OnReceiveIncrementText(Fulltext.ToString());

				//Call all remaining text events
				TArray<FName> TextEvents;
				DialogueNode.TextEventLinkers.GenerateValueArray(TextEvents);
				for (FName Name : TextEvents)
				{
					StartTextEvent(Name);
				}
			}
			else
			{
				OldTextSpeed = 0.01;
				CurrentTextSpeed = 0.01;
				SetTextSpeed(CurrentTextSpeed);
			}
		}

		bTextHasAdvanced = true;
	}
}

bool UPA_HudWidget_DialogueBubble::IsTextComplete()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString TempString = Fulltext.ToString();
		int32 TextLength = TempString.Len();

		return World->GetTimerManager().IsTimerActive(TextIncrementTimer) || TextLength >= TextIndex;
	}

	return true;
}

FVector UPA_HudWidget_DialogueBubble::GetDialoguePosition(bool bApplyZCorrection, bool bOutputCamDistance)
{
	FVector NewPos = FVector::ZeroVector;
	FVector WorldLocation;
	UWorld* World = GetWorld();

	if (FollowActor
		&& World)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);

		if (PC)
		{
			//First get the actual screenpositions
			ACharacter* Character = Cast<ACharacter>(FollowActor);
			if (Character)
			{
				WorldLocation = Character->GetActorLocation();
				if(bApplyZCorrection)
					WorldLocation.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			}
			else
			{
				FVector Origin, BoxExtent;
				FollowActor->GetActorBounds(false, Origin, BoxExtent);

				WorldLocation = Origin;
				if(bApplyZCorrection)
					WorldLocation.Z += BoxExtent.Z;
			}

			if (bOutputCamDistance)
			{
				UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPositionWithDistance(PC, WorldLocation, NewPos);
				NewPos.X += WindowOffset.X;
				NewPos.Y += WindowOffset.Y;
			}
			else
			{
				FVector2D ScreenPos; 
				UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldLocation, ScreenPos);
				ScreenPos += WindowOffset;
				NewPos.X = ScreenPos.X;
				NewPos.Y = ScreenPos.Y;
			}

			//Clamp the positions based on screen resolution
			FVector2D ScreenRes = UWidgetLayoutLibrary::GetViewportSize(World) / UWidgetLayoutLibrary::GetViewportScale(World);
			NewPos = FVector(
				FMath::Clamp(NewPos.X, ViewportMargins.Left, ScreenRes.X - ViewportMargins.Right - WindowSize.X),
				FMath::Clamp(NewPos.Y, ViewportMargins.Top, ScreenRes.Y - ViewportMargins.Bottom - WindowSize.Y),
				NewPos.Z
			);
		}
	}

	return NewPos;
}

//Choice Nodes functionality
void UPA_HudWidget_DialogueBubble::SetupChoiceNodes(TArray<FDialogueNode> NewChoiceNodes)
{
	ChoiceNodes = NewChoiceNodes;
	ChoiceIndex = 0;

	OnReceiveSetupChoiceNodes(ChoiceNodes);
}

void UPA_HudWidget_DialogueBubble::MoveChoiceUp()
{
	ChoiceIndex--;
	if (ChoiceIndex < 0)
	{
		ChoiceIndex = ChoiceNodes.Num() - 1;
	}

	OnReceiveFocusChoice(ChoiceIndex);
}

void UPA_HudWidget_DialogueBubble::MoveChoiceDown()
{
	ChoiceIndex++;
	if (ChoiceIndex >= ChoiceNodes.Num())
	{
		ChoiceIndex = 0;
	}

	OnReceiveFocusChoice(ChoiceIndex);
}

void UPA_HudWidget_DialogueBubble::FocusChoice()
{
	OnReceiveFocusChoice(ChoiceIndex);
}

void UPA_HudWidget_DialogueBubble::SelectChoice()
{
	OnReceiveSelectChoice(ChoiceIndex);
}