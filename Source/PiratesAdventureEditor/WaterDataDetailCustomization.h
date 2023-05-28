// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "PA_Asset_Water.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

//////////////////////////////////////////////////////////////////////////
// FInventoryItemDetailCustomization

class FWaterDataDetailCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	// End of IDetailCustomization interface

private:
	TArray<TSharedPtr<FString>> PropertyOptions;
	FString GetEnumAsString(EWaterType EnumValue);
	void OnChangeProperty(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);
	//FReply RefreshOnClicked();

	/** Associated detail layout builder */
	IDetailLayoutBuilder* DetailLayoutBuilder;
	class UPA_Asset_Water* Water;
};