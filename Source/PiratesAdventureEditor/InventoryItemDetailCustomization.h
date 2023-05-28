// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "PA_Asset_Item.h"
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

//////////////////////////////////////////////////////////////////////////
// FInventoryItemDetailCustomization

class FInventoryItemDetailCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	void NameTextCommited(const FText& NewText, ETextCommit::Type InCommitType, class UPA_Asset_Item* ItemToCommit);
	void SupportTextCommited(const FText& NewText, ETextCommit::Type InCommitType, class UPA_Asset_Item* ItemToCommit);
	void SmallSupportTextCommited(const FText& NewText, ETextCommit::Type InCommitType, class UPA_Asset_Item* ItemToCommit);
	// End of IDetailCustomization interface

private:
	TArray<TSharedPtr<FString>> PropertyOptions;
	FString GetEnumAsString(EItemCategory EnumValue);
	void OnChangeProperty(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);
	//FReply RefreshOnClicked();

	/** Associated detail layout builder */
	IDetailLayoutBuilder* DetailLayoutBuilder;
	UPA_Asset_Item* Item;
};