#include "DialoguePluginEditorSettingsDetails.h"
#include "DialoguePluginEditorPrivatePCH.h"
#include "Dialogue.h"
#include "DialogueViewportWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"


#define LOCTEXT_NAMESPACE "DialoguePluginSettingsDetails"

TSharedRef<IDetailCustomization> FDialoguePluginEditorSettingsDetails::MakeInstance()
{
	return MakeShareable(new FDialoguePluginEditorSettingsDetails());
}

void FDialoguePluginEditorSettingsDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
	DetailLayoutBuilder = &DetailLayout;

	const TSharedPtr<IPropertyHandle> DataProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDialogue, Data));
	DetailLayout.HideProperty(DataProperty);
	const TSharedPtr<IPropertyHandle> NextNodeProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDialogue, NextNodeId));
	DetailLayout.HideProperty(NextNodeProperty);

	// Create a category so this is displayed early in the properties
	IDetailCategoryBuilder& MyCategory = DetailLayout.EditCategory("Dialogue Editor");
	IDetailCategoryBuilder& CurrentNodeCategory = DetailLayout.EditCategory("Current Node", LOCTEXT("CurrentNode", "Current Node"), ECategoryPriority::Important);

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;

	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	TArray<UObject*> StrongObjects;
	CopyFromWeakArray(StrongObjects, ObjectsBeingCustomized);

	if (StrongObjects.Num() == 0) return;

	UDialogue* Dialogue = Cast<UDialogue>(StrongObjects[0]);
	
	if (Dialogue->CurrentNodeId != -1 && Dialogue->CurrentNodeId != 0) //display current node details:
	{
		int32 index;
		FDialogueNode CurrentNode = Dialogue->GetNodeById(Dialogue->CurrentNodeId, index);

		const TSharedPtr<IPropertyHandleArray> Array = DataProperty->AsArray();
		const TSharedPtr<IPropertyHandle> Child = Array->GetElement(index);

		if (CurrentNode.NodeOwner == EDialogueNodeOwner::DNO_EVENT)
		{
			const TSharedPtr<IPropertyHandle> NodeTextEventName = Child->GetChildHandle("TextEventName");
			CurrentNodeCategory.AddProperty(NodeTextEventName);
			return;
		}

		CurrentNodeCategory.AddCustomRow(LOCTEXT("Text", "Text"))
			.WholeRowContent()
			[
				SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(LOCTEXT("Text", "Text"))
			];

		CurrentNodeCategory.AddCustomRow(LOCTEXT("TextValue", "TextValue"))
			.WholeRowContent()
			[
				SNew(SBox)
				.HeightOverride(100)
				[
					SNew(SMultiLineEditableTextBox).Text(CurrentNode.Text)
					.AutoWrapText(true)
					.OnTextCommitted(this, &FDialoguePluginEditorSettingsDetails::TextCommited, Dialogue, Dialogue->CurrentNodeId)
					.ModiferKeyForNewLine(EModifierKey::Shift)
				]
				
			];
			
		//const TSharedPtr<IPropertyHandleArray> Array = DataProperty->AsArray();
		//const TSharedPtr<IPropertyHandle> Child = Array->GetElement(index);
		const TSharedPtr<IPropertyHandle> NodeTypeField = Child->GetChildHandle("NodeType");
		const TSharedPtr<IPropertyHandle> NodeOwnerField = Child->GetChildHandle("NodeOwner");
		const TSharedPtr<IPropertyHandle> NodeOwnerIdx = Child->GetChildHandle("NPCIndex");
		const TSharedPtr<IPropertyHandle> EventsField = Child->GetChildHandle("Events");
		const TSharedPtr<IPropertyHandle> ConditionsField = Child->GetChildHandle("Conditions");
		const TSharedPtr<IPropertyHandle> SoundField = Child->GetChildHandle("Sound");
		const TSharedPtr<IPropertyHandle> DialogueWaveField = Child->GetChildHandle("DialogueWave");
		const TSharedPtr<IPropertyHandle> NodeEventLinkersField = Child->GetChildHandle("TextEventLinkers");
		const TSharedPtr<IPropertyHandle> NodeSpeedLinkersField = Child->GetChildHandle("TextSpeedLinkers");
		const TSharedPtr<IPropertyHandle> DurationField = Child->GetChildHandle("Duration");
		const TSharedPtr<IPropertyHandle> ActivateEventAtStartField = Child->GetChildHandle("bActivateEventAtStartOfNode");
		
		if (CurrentNode.NodeOwner != EDialogueNodeOwner::DNO_CHOICE)
		{
			CurrentNodeCategory.AddProperty(NodeTypeField);
		}

		CurrentNodeCategory.AddProperty(NodeOwnerField);

		switch (CurrentNode.NodeOwner)
		{
		case EDialogueNodeOwner::DNO_NPC:
			CurrentNodeCategory.AddProperty(NodeOwnerIdx);
			break;
		default:
			break;
		}

		CurrentNodeCategory.AddProperty(EventsField);
		CurrentNodeCategory.AddProperty(ActivateEventAtStartField);			
		
		/* 
		* Customizing Conditions Row
		*/
		IDetailPropertyRow * ConditionDetailsRow = &CurrentNodeCategory.AddProperty(ConditionsField);

		//TSharedPtr<SWidget> DefaultNameWidget;
		//TSharedPtr<SWidget> DefaultValueWidget;
		//FDetailWidgetRow DefaultWidgetRow;
		//ConditionDetailsRow->GetDefaultWidgets(DefaultNameWidget, DefaultValueWidget, DefaultWidgetRow);
		//
		//FDetailWidgetRow & CustomRow = ConditionDetailsRow->CustomWidget(false); // erases the default contents of the row
		//CustomRow.NameContent()
		//[
		//	DefaultNameWidget.ToSharedRef()
		//]
		//.ValueContent()
		//.MinDesiredWidth(170.0f)
		//[
		//	DefaultValueWidget.ToSharedRef()
		//];

		//uint32 children = 0;
		//ConditionsField->GetNumChildren(children);
		//for (uint32 i = 0; i < children; i++)
		//{
		//	TSharedPtr<IPropertyHandle> childHandle = ConditionsField->GetChildHandle(i);
		//	IDetailPropertyRow * subConditionRow = &CurrentNodeCategory.AddProperty(childHandle);
		//	subConditionRow->ShouldAutoExpand(true);
		//}
		/*
		* end of conditions customization
		*/

		CurrentNodeCategory.AddProperty(SoundField);
		CurrentNodeCategory.AddProperty(DialogueWaveField);		

		CurrentNodeCategory.AddProperty(NodeEventLinkersField);
		CurrentNodeCategory.AddProperty(NodeSpeedLinkersField);

		if (CurrentNode.NodeOwner != EDialogueNodeOwner::DNO_CHOICE)
		{
			CurrentNodeCategory.AddProperty(DurationField);
		}
	}	
	
}

void FDialoguePluginEditorSettingsDetails::TextCommited(const FText& NewText, ETextCommit::Type CommitInfo, UDialogue* Dialogue, int32 id)
{
	int32 index;
	FDialogueNode CurrentNode = Dialogue->GetNodeById(id, index);

	// we don't commit text if it hasn't changed
	if (Dialogue->Data[index].Text.EqualTo(NewText))
	{
		return;
	}
	
	const FScopedTransaction Transaction(LOCTEXT("TextCommited", "Edited Node Text"));
	Dialogue->Modify();
	
	Dialogue->Data[index].Text = NewText;
}

#undef LOCTEXT_NAMESPACE
