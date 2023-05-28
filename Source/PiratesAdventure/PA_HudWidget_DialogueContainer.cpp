// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.


#include "PA_HudWidget_DialogueContainer.h"
#include "Dialogue.h"

bool UPA_HudWidget_DialogueContainer::IsConditionsMetForNode_Implementation(FDialogueNode Node)
{
	for (UDialogueConditions* Condition : Node.Conditions)
	{
		if (IsValid(Condition))
		{
			//if (!Condition->IsConditionMet(GetOwningPlayer(), NPCActor))
			//{
			//	return false;
			//}
		}
	}
	return true;
}

void UPA_HudWidget_DialogueContainer::RunEventsForNode_Implementation(FDialogueNode Node)
{
	for (UDialogueEvents* Event : Node.Events)
	{
		//if (IsValid(Event))
		//{
		//	Event->RecieveEventTriggered(GetOwningPlayer(), NPCActor);
		//}
	}
}

