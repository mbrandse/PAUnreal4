// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_PlayerHUD.h"
#include "PiratesAdventure.h"
#include "PiratesAdventureCharacter.h"
#include "PA_HudElements_RingMenu.h"
#include "PA_HudElements_ContextPopup.h"
#include "PA_HudElements_DialogueBase.h"
#include "PA_HudElements_Status.h"
#include "PA_HudElements_IngameMenu.h"
#include "../Classes/Kismet/DataTableFunctionLibrary.h"
#include "PiratesAdventureCharacter.h"

void APA_PlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APiratesAdventureCharacter>(PlayerOwner->GetPawn());

	if (ElementRingTemplate)
	{
		ElementRingInstance = CreateWidget<UPA_HudElements_RingMenu>(PlayerOwner, ElementRingTemplate);
		if (ElementRingInstance)
		{
			ElementRingInstance->OwnerHUD = this;
			ElementRingInstance->AddToViewport();
			ElementRingInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (ToolRingTemplate)
	{
		ToolRingInstance = CreateWidget<UPA_HudElements_RingMenu>(PlayerOwner, ToolRingTemplate);
		if (ToolRingInstance)
		{
			ToolRingInstance->OwnerHUD = this;
			ToolRingInstance->AddToViewport();
			ToolRingInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//if (StatusIndicatorTemplate)
	//{
	//	StatusIndicatorInstance = CreateWidget<UPA_HudElements_Status>(PlayerOwner, StatusIndicatorTemplate);
	//	if (StatusIndicatorInstance)
	//	{
	//		StatusIndicatorInstance->OwnerHUD = this;
	//		StatusIndicatorInstance->AddToViewport();
	//		StatusIndicatorInstance->SetVisibility(ESlateVisibility::Visible);
	//	}
	//}

	if (ContextPopupTemplate)
	{
		ContextPopupInstance = CreateWidget<UPA_HudElements_ContextPopup>(PlayerOwner, ContextPopupTemplate);
		if (ContextPopupInstance)
		{
			ContextPopupInstance->OwnerHUD = this;
			ContextPopupInstance->AddToViewport();
			ContextPopupInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}

	//if (IngameMenuTemplate)
	//{
	//	IngameMenuInstance = CreateWidget<UPA_HudElements_IngameMenu>(PlayerOwner, IngameMenuTemplate);
	//	if (IngameMenuInstance)
	//	{
	//		IngameMenuInstance->OwnerHUD = this;
	//		IngameMenuInstance->AddToViewport();
	//		IngameMenuInstance->SetVisibility(ESlateVisibility::Hidden); 
	//		IngameMenuInstance->SetIsEnabled(false); 
	//	}
	//}
}

void APA_PlayerHUD::EnableHUDInput(class APlayerController* PlayerController)
{
	if (PlayerController)
	{
		// If it doesn't exist create it and bind delegates. We should also rebind everything in case we load from a save for instance
		if (!InputComponent)
		{
			InputComponent = NewObject<UInputComponent>(this);
			InputComponent->RegisterComponent();
			InputComponent->bBlockInput = true; //to make sure we can completely override other inputcomponents for now.
			InputComponent->Priority = InputPriority;

			//Let's bind all HUD related options here
			InputComponent->BindAction("InterfaceAccept", IE_Pressed, this, &APA_PlayerHUD::InterfaceAcceptPress).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceAccept", IE_Released, this, &APA_PlayerHUD::InterfaceAcceptRelease).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceCancel", IE_Pressed, this, &APA_PlayerHUD::InterfaceCancelPress).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceCancel", IE_Released, this, &APA_PlayerHUD::InterfaceCancelRelease).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceUp", IE_Pressed, this, &APA_PlayerHUD::InterfaceUpPress).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceUp", IE_Released, this, &APA_PlayerHUD::InterfaceUpRelease).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceLeft", IE_Pressed, this, &APA_PlayerHUD::InterfaceLeftPress).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceLeft", IE_Released, this, &APA_PlayerHUD::InterfaceLeftRelease).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceDown", IE_Pressed, this, &APA_PlayerHUD::InterfaceDownPress).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceDown", IE_Released, this, &APA_PlayerHUD::InterfaceDownRelease).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceRight", IE_Pressed, this, &APA_PlayerHUD::InterfaceRightPress).bExecuteWhenPaused = true;
			InputComponent->BindAction("InterfaceRight", IE_Released, this, &APA_PlayerHUD::InterfaceRightRelease).bExecuteWhenPaused = true;

			if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
			{
				UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent);
			}
		}
		else
		{
			// Make sure we only have one instance of the InputComponent on the stack
			PlayerController->PopInputComponent(InputComponent);
		}

		PlayerController->PushInputComponent(InputComponent);
	}
}

void APA_PlayerHUD::RebindHUDInput()
{
	if (InputComponent)
	{
		//clear all bindings for rebinding
		InputComponent->ClearActionBindings();
		InputComponent->AxisBindings.Empty();

		//Let's bind all HUD related options here
		InputComponent->BindAction("InterfaceAccept", IE_Pressed, this, &APA_PlayerHUD::InterfaceAcceptPress).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceAccept", IE_Released, this, &APA_PlayerHUD::InterfaceAcceptRelease).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceCancel", IE_Pressed, this, &APA_PlayerHUD::InterfaceCancelPress).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceCancel", IE_Released, this, &APA_PlayerHUD::InterfaceCancelRelease).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceUp", IE_Pressed, this, &APA_PlayerHUD::InterfaceUpPress).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceUp", IE_Released, this, &APA_PlayerHUD::InterfaceUpRelease).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceLeft", IE_Pressed, this, &APA_PlayerHUD::InterfaceLeftPress).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceLeft", IE_Released, this, &APA_PlayerHUD::InterfaceLeftRelease).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceDown", IE_Pressed, this, &APA_PlayerHUD::InterfaceDownPress).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceDown", IE_Released, this, &APA_PlayerHUD::InterfaceDownRelease).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceRight", IE_Pressed, this, &APA_PlayerHUD::InterfaceRightPress).bExecuteWhenPaused = true;
		InputComponent->BindAction("InterfaceRight", IE_Released, this, &APA_PlayerHUD::InterfaceRightRelease).bExecuteWhenPaused = true;

		if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
		{
			UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent);
		}
	}
}

void APA_PlayerHUD::SpawnItemGetWindow(class UPA_Asset_Item* Item, TSubclassOf<class UPA_HudElements_ItemGet> Template)
{
	if (Template
		&& Item)
	{
		ItemPopupInstance = CreateWidget<UPA_HudElements_ItemGet>(PlayerOwner, Template);
		if (ItemPopupInstance)
		{
			ItemPopupInstance->ConstructItemPopup(Item);
			ItemPopupInstance->AddToViewport();
		}
	}
}

void APA_PlayerHUD::SpawnDialogue(TArray<AActor*> NPCs, class UDialogue* Dialogue)
{
	if (OwnerCharacter)
	{
		Dialogue->Player = OwnerCharacter;
		Dialogue->NPCs = NPCs;
		//TODO: if necessary, build in some support that makes a quick search of the dialogue data to see if there's any nodes with Duration == 0 (which means we require player input)
		//Perhaps have a TMap with all Dialogues (and a bool for player input) so we can check for duplicates?
		OnReceiveSpawnDialogue(Dialogue);
	}
}

bool APA_PlayerHUD::ShouldFocusDialogue(class UDialogue* Dialogue)
{
	if (Dialogue)
	{
		for (int32 NodeIndex = 0; NodeIndex < Dialogue->Data.Num(); NodeIndex++)
		{
			if (Dialogue->Data[NodeIndex].id != 0)
			{
				if (Dialogue->Data[NodeIndex].NodeOwner == EDialogueNodeOwner::DNO_CHOICE
					|| Dialogue->Data[NodeIndex].Duration != 0.f)
					return false;
			}

			return true;
		}
	}

	return false;
}

//Input functions
void APA_PlayerHUD::InterfaceAcceptPress()
{
	ReceiveInterfaceAcceptPress();

	//UE_LOG(AnyLog, Log, TEXT("PRESSED INTERFACE ACCEPT BUTTON"));

	//if (PlayerOwner
	//	&& GetWorld())
	//{
	//	//Since our dialogue cannot be progressed while paused, let's put that functionality in here. 
	//	//We will assume for the sake of our interface that as long as we use any in-game menu, the game will become paused.
	//	if (!UGameplayStatics::IsGamePaused(PlayerOwner->GetWorld()))
	//	{
	//		//temporary. Doesn't take into account choice
	//		if (PlayerDialogueInstance && PlayerDialogueInstance->IsInteractiveDialogue()) //for now do nothing with a choice. We want to see it rendered first
	//		{
	//			FDialogueNode CurrentNode = PlayerDialogueInstance->CurrentDialogueNodes[PlayerDialogueInstance->CurrentChoiceIndex];

	//			if (PlayerDialogueInstance->GetWorld()->GetTimerManager().IsTimerActive(PlayerDialogueInstance->TickDialogueTextHandle))
	//			{
	//				PlayerDialogueInstance->FinalizeText();
	//			}
	//			else if (CurrentNode.isChoice && PlayerDialogueInstance->HudElementState == EHudElementState::HES_StartLoop)
	//			{
	//				PlayerDialogueInstance->FinalizeChoice();
	//			}
	//			else
	//			{
	//				if (!CurrentNode.isChoice)
	//					PlayerDialogueInstance->CallEventsAtEndNode(CurrentNode);
	//				
	//				//if we have a delay, apply it now
	//				if (CurrentNode.Delay != 0.f)
	//				{
	//					PlayerDialogueInstance->GetWorld()->GetTimerManager().SetTimer(PlayerDialogueInstance->DelayHandle,
	//						PlayerDialogueInstance,
	//						&UPA_HudElements_DialogueBase::StartDelayedDialogue,
	//						CurrentNode.Delay,
	//						false);

	//					PlayerDialogueInstance->bIsDelayed = true;
	//				}

	//				//do we need to switch the balloon to another person or force a close window? Then leave the setting of the next node to the animation.
	//				if (PlayerDialogueInstance->ShouldChangeSpeaker(CurrentNode)
	//					|| CurrentNode.bForceWindowClose)
	//				{
	//					PlayerDialogueInstance->HudElementState = EHudElementState::HES_StopLoop;
	//				}
	//				else
	//				{
	//					PlayerDialogueInstance->NextDialogueNode(CurrentNode);
	//					PlayerDialogueInstance->CurrentChoiceIndex = 0;
	//				}
	//			}
	//		}
	//	}

	//	//TODO: make dialogue fade out or disappear when main menu is called.
	//	if (IngameMenuInstance->bIsEnabled)
	//	{
	//		IngameMenuInstance->ProcessConfirm();
	//	}
	//}
}

void APA_PlayerHUD::InterfaceAcceptRelease()
{
	ReceiveInterfaceAcceptRelease();
}

void APA_PlayerHUD::InterfaceCancelPress()
{
	ReceiveInterfaceCancelPress();

	//if (PlayerOwner
	//	&& GetWorld())
	//{
	//	//TEMP SUB LEVEL FUNCTION
	//	if (IngameMenuInstance->bIsEnabled)
	//	{
	//		IngameMenuInstance->ProcessCancel();
	//	}
	//}
}

void APA_PlayerHUD::InterfaceCancelRelease()
{
	ReceiveInterfaceCancelRelease();
}

void APA_PlayerHUD::InterfaceUpPress()
{
	ReceiveInterfaceUpPress();

	//if (PlayerOwner
	//	&& GetWorld())
	//{
	//	if (!UGameplayStatics::IsGamePaused(PlayerOwner->GetWorld()))
	//	{
	//		//handles the up for the dialogue choice index;
	//		if (PlayerDialogueInstance 
	//			&& PlayerDialogueInstance->IsInteractiveDialogue()
	//			&& PlayerDialogueInstance->CurrentDialogueNodes[0].isChoice) 
	//		{
	//			PlayerDialogueInstance->CurrentChoiceIndex--;
	//			if (PlayerDialogueInstance->CurrentChoiceIndex < 0)
	//				PlayerDialogueInstance->CurrentChoiceIndex = PlayerDialogueInstance->CurrentDialogueNodes.Num() - 1;

	//			PlayerDialogueInstance->FocusChoice(PlayerDialogueInstance->CurrentChoiceIndex);
	//		}
	//	}

	//	if (IngameMenuInstance->bIsEnabled)
	//	{
	//		IngameMenuInstance->ProcessVerticalNavigation(-1);
	//	}
	//}
}

void APA_PlayerHUD::InterfaceUpRelease()
{
	ReceiveInterfaceUpRelease();
}

void APA_PlayerHUD::InterfaceLeftPress()
{
	ReceiveInterfaceLeftPress();

	//if (PlayerOwner
	//	&& GetWorld())
	//{
	//	if (IngameMenuInstance->bIsEnabled)
	//	{
	//		IngameMenuInstance->ProcessHorizontalNavigation(-1);
	//	}
	//}
}

void APA_PlayerHUD::InterfaceLeftRelease()
{
	ReceiveInterfaceLeftRelease();
}

void APA_PlayerHUD::InterfaceDownPress()
{
	ReceiveInterfaceDownPress();

	//if (PlayerOwner
	//	&& GetWorld())
	//{
	//	if (!UGameplayStatics::IsGamePaused(PlayerOwner->GetWorld()))
	//	{
	//		//handles the up for the dialogue choice index;
	//		if (PlayerDialogueInstance 
	//			&& PlayerDialogueInstance->IsInteractiveDialogue()
	//			&& PlayerDialogueInstance->CurrentDialogueNodes[0].isChoice)
	//		{
	//			PlayerDialogueInstance->CurrentChoiceIndex++;
	//			if (PlayerDialogueInstance->CurrentChoiceIndex >= PlayerDialogueInstance->CurrentDialogueNodes.Num())
	//				PlayerDialogueInstance->CurrentChoiceIndex = 0;

	//			PlayerDialogueInstance->FocusChoice(PlayerDialogueInstance->CurrentChoiceIndex);
	//		}
	//	}

	//	if (IngameMenuInstance->bIsEnabled)
	//	{
	//		IngameMenuInstance->ProcessVerticalNavigation(1);
	//	}
	//}
}

void APA_PlayerHUD::InterfaceDownRelease()
{
	ReceiveInterfaceDownRelease();
}

void APA_PlayerHUD::InterfaceRightPress()
{
	ReceiveInterfaceRightPress();

	//if (PlayerOwner
	//	&& GetWorld())
	//{
	//	if (IngameMenuInstance->bIsEnabled)
	//	{
	//		IngameMenuInstance->ProcessHorizontalNavigation(1);
	//	}
	//}
}

void APA_PlayerHUD::InterfaceRightRelease()
{
	ReceiveInterfaceRightRelease();
}

////HUD Transition Related functions
//void APA_PlayerHUD::SetFade(float NewValue)
//{
//	if (StatusIndicatorInstance)
//		StatusIndicatorInstance->SetFade(NewValue);
//}
//
//void APA_PlayerHUD::StartFade()
//{
//	if (StatusIndicatorInstance)
//		StatusIndicatorInstance->StartOpenFadeIn();
//}