
#include "InventoryItemDetailCustomization.h"
#include "PiratesAdventureEditor.h"
#include "PA_Asset_Item.h"
#include "PropertyEditing.h"
#include "SlateBasics.h"
#include "SlateFontInfo.h"
#include "STextComboBox.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"
#include "DetailLayoutBuilder.h"

#define LOCTEXT_NAMESPACE "InventoryItemDetailCustomization"

//////////////////////////////////////////////////////////////////////////
// FInventoryItemDetailCustomization

TSharedRef<IDetailCustomization> FInventoryItemDetailCustomization::MakeInstance()
{
	return MakeShareable(new FInventoryItemDetailCustomization);
}

void FInventoryItemDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	//static int Tester;
	//DetailBuilder.EditCategory("Sprite", FText::GetEmpty(), ECategoryPriority::Important);

	//TODO FOR TOMORROW: MAKE AND POPULATE ALL CATEGORIES HERE (MAKE SURE TO SAVE THE CATEGORIES AS SEPERATE VARIABLES IN THE .H), AND USE THE CHANGEPROPERTY TO TURN THEM (IN)VISIBLE
	//MAYBE ONLY USE THE CHANGEPROPERTY TO CALL ForceRefreshDetails()

	DetailLayoutBuilder = &DetailLayout;

	const TSharedPtr<IPropertyHandle> ItemProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Item, ItemData));
	DetailLayout.HideProperty(ItemProperty);

	IDetailCategoryBuilder& CurrentItemCategoryBasic = DetailLayout.EditCategory("Basic Data", LOCTEXT("InventoryItem", "Basic Data"), ECategoryPriority::Important);	
	IDetailCategoryBuilder& CurrentItemCategoryHUD = DetailLayout.EditCategory("HUD Data", LOCTEXT("InventoryItemHUD", "HUD Data"), ECategoryPriority::Important);

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;

	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	TArray<UObject*> StrongObjects;
	CopyFromWeakArray(StrongObjects, ObjectsBeingCustomized);

	if (StrongObjects.Num() == 0) return;

	//UE_LOG(PAEditor, Warning, TEXT("Updated the Inventory Item Details a total of %d times"), Tester++);

	Item = Cast<UPA_Asset_Item>(StrongObjects[0]);
	if (Item)
	{
		const TSharedPtr<IPropertyHandle> IDField = ItemProperty->GetChildHandle("ID");
	
		//Add later together, possibly in a separate category
		const TSharedPtr<IPropertyHandle> IconField = ItemProperty->GetChildHandle("Icon");
		//const TSharedPtr<IPropertyHandle> IconUnlockedField = ItemProperty->GetChildHandle("bUnlocked");
		//const TSharedPtr<IPropertyHandle> IconInactiveField = ItemProperty->GetChildHandle("bInactive");
		const TSharedPtr<IPropertyHandle> IconButtonAssociationField = ItemProperty->GetChildHandle("RingMenuButtonAssociation");

		//Weapon fields
		const TSharedPtr<IPropertyHandle> WeaponTypeField = ItemProperty->GetChildHandle("WeaponType");

		//Tool fields
		const TSharedPtr<IPropertyHandle> ToolTypeField = ItemProperty->GetChildHandle("ToolType");
		const TSharedPtr<IPropertyHandle> ToolTemplateField = ItemProperty->GetChildHandle("ToolTemplate");

		//Item fields
		const TSharedPtr<IPropertyHandle> ItemClassField = ItemProperty->GetChildHandle("ItemClass");
		const TSharedPtr<IPropertyHandle> ItemEmptyField = ItemProperty->GetChildHandle("bIsEmpty");

		//Equipment fields
		const TSharedPtr<IPropertyHandle> MeshField = ItemProperty->GetChildHandle("Mesh");

		//have the ID field on top.
		CurrentItemCategoryBasic.AddProperty(IDField);

		//after this we have our text parameters
		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("NameText", "NameText")).NameContent()
			[
				SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(LOCTEXT("Name Text", "Name Text"))
			];

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("NameTextValue", "NameTextValue"))
			[
				SNew(SBox)
				//.HeightOverride(100)
				[
					SNew(SEditableTextBox).Text(Item->ItemData.NameText)
					//.AutoWrapText(false)
					.OnTextCommitted(this, &FInventoryItemDetailCustomization::NameTextCommited, Item)
					//.ModiferKeyForNewLine(EModifierKey::Shift)
				]

			];

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("SupportTextSmall", "SupportTextSmall")).NameContent()
			[
				SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(LOCTEXT("Support Text Small", "Support Text Small"))
			];

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("SupportTextSmallValue", "SupportTextSmallValue"))
			[
				SNew(SBox)
				.HeightOverride(50)
				[
					SNew(SMultiLineEditableTextBox).Text(Item->ItemData.SupportTextSmall)
					.AutoWrapText(true)
					.OnTextCommitted(this, &FInventoryItemDetailCustomization::SmallSupportTextCommited, Item)
					.ModiferKeyForNewLine(EModifierKey::Shift)
				]

			];

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("SupportText", "SupportText")).NameContent()
			[
				SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(LOCTEXT("Support Text", "Support Text"))
			];

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("SupportTextValue", "SupportTextValue"))
			[
				SNew(SBox)
				.HeightOverride(100)
				[
					SNew(SMultiLineEditableTextBox).Text(Item->ItemData.SupportText)
					.AutoWrapText(true)
					.OnTextCommitted(this, &FInventoryItemDetailCustomization::SupportTextCommited, Item)
					.ModiferKeyForNewLine(EModifierKey::Shift)
				]

			];

		PropertyOptions.Empty();
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Consumable))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Key))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Collectible))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Equipment))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Weapon))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Tool))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EItemCategory::IC_Item))));

		uint8 DataValue = (uint8)Item->ItemData.Category;

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("CategoryText", "CategoryText")).NameContent()
			[
				SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(LOCTEXT("Category", "Category"))
			];

		CurrentItemCategoryBasic.AddCustomRow(LOCTEXT("CategoryValue", "CategoryValue"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.Padding(0.f, 0.f, 2.f, 0.f)
				[
					SNew(STextComboBox)
					.OptionsSource(&PropertyOptions)
					.InitiallySelectedItem(PropertyOptions[DataValue])
					.OnSelectionChanged(this, &FInventoryItemDetailCustomization::OnChangeProperty)
				]
			];

		//Add HUD elements related things
		CurrentItemCategoryHUD.AddProperty(IconField);
		//CurrentItemCategoryHUD.AddProperty(IconUnlockedField);
		//CurrentItemCategoryHUD.AddProperty(IconInactiveField);

		IDetailCategoryBuilder* CurrentItemCategoryExtended; 

		switch (Item->ItemData.Category)
		{
		case EItemCategory::IC_Collectible:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Collectible Data", LOCTEXT("InventoryItemCollectible", "Collectible Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(ItemClassField); 
			break;

		case EItemCategory::IC_Consumable:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Consumable Data", LOCTEXT("InventoryItemCollectible", "Consumable Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(ItemClassField);
			CurrentItemCategoryExtended->AddProperty(ItemEmptyField);
			break;

		case EItemCategory::IC_Equipment:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Equipment Data", LOCTEXT("InventoryItemCollectible", "Equipment Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(MeshField);
			break;

		case EItemCategory::IC_Key:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Key Item Data", LOCTEXT("InventoryItemCollectible", "Keyitem Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(ItemClassField);
			break;

		case EItemCategory::IC_Weapon:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Weapon Data", LOCTEXT("InventoryItemCollectible", "Weapon Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(WeaponTypeField);
			CurrentItemCategoryExtended->AddProperty(IconButtonAssociationField);
			break;

		case EItemCategory::IC_Tool:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Tool Data", LOCTEXT("InventoryItemCollectible", "Tool Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(ToolTypeField);
			CurrentItemCategoryExtended->AddProperty(ToolTemplateField);
			CurrentItemCategoryExtended->AddProperty(IconButtonAssociationField);
			break;

		case EItemCategory::IC_Item:
			CurrentItemCategoryExtended = &DetailLayout.EditCategory("Flask Data", LOCTEXT("InventoryItemCollectible", "Item Data"), ECategoryPriority::Default);
			CurrentItemCategoryExtended->AddProperty(ItemClassField);
			CurrentItemCategoryExtended->AddProperty(ItemEmptyField);
		}
	}
}

void FInventoryItemDetailCustomization::OnChangeProperty(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	for (uint8 Index = 0; Index < PropertyOptions.Num(); Index++)
	{
		if (ItemSelected == PropertyOptions[Index])
		{
			Item->ItemData.Category = (EItemCategory)Index;
			break;
		}
	}

	if (DetailLayoutBuilder)
		DetailLayoutBuilder->ForceRefreshDetails();
}

FString FInventoryItemDetailCustomization::GetEnumAsString(EItemCategory EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EItemCategory"), true);
	if (!EnumPtr) return TEXT("Invalid");

	return EnumPtr->GetDisplayNameTextByIndex((uint8)EnumValue).ToString();

	//return EnumPtr->GetDisplayNameText((uint8)EnumValue).ToString();
}

void FInventoryItemDetailCustomization::NameTextCommited(const FText& NewText, ETextCommit::Type InCommitType, class UPA_Asset_Item* ItemToCommit)
{
	// we don't commit text if it hasn't changed
	if (ItemToCommit->ItemData.NameText.EqualTo(NewText))
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("NameTextCommited", "Edited Name Text"));
	ItemToCommit->Modify();

	FString TextKey = ItemToCommit->GetName();
	TextKey += " Name Text";

	FText FinalText = FText::ChangeKey(TEXT("Inventory Item"), TextKey, NewText);

	ItemToCommit->ItemData.NameText = FinalText;
}

void FInventoryItemDetailCustomization::SupportTextCommited(const FText& NewText, ETextCommit::Type InCommitType, class UPA_Asset_Item* ItemToCommit)
{
	// we don't commit text if it hasn't changed
	if (ItemToCommit->ItemData.SupportText.EqualTo(NewText))
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("SupportTextCommited", "Edited Support Text"));
	ItemToCommit->Modify();

	FString TextKey = ItemToCommit->GetName();
	TextKey += " Support Text";

	FText FinalText = FText::ChangeKey(TEXT("Inventory Item"), TextKey, NewText);

	ItemToCommit->ItemData.SupportText = FinalText;
}

void FInventoryItemDetailCustomization::SmallSupportTextCommited(const FText& NewText, ETextCommit::Type InCommitType, class UPA_Asset_Item* ItemToCommit)
{
	// we don't commit text if it hasn't changed
	if (ItemToCommit->ItemData.SupportText.EqualTo(NewText))
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("SmallSupportTextCommited", "Edited Support Text Small"));
	ItemToCommit->Modify();

	FString TextKey = ItemToCommit->GetName();
	TextKey += " Support Text Small";

	FText FinalText = FText::ChangeKey(TEXT("Inventory Item"), TextKey, NewText);

	ItemToCommit->ItemData.SupportTextSmall = FinalText;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE