// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_HudWidget_InventoryItemStyle.h"
#include "PiratesAdventure.h"

FInventoryItemStyle::FInventoryItemStyle()
	//: Background()
	: BackgroundNormal()
	, BackgroundRollover()
	, BackgroundActive()
	, BackgroundInactive()
	//, AmmoNormal()
	//, AmmoInactive()
{
}

const FName FInventoryItemStyle::TypeName(TEXT("FInventoryItemStyle"));

void FInventoryItemStyle::GetResources(TArray< const FSlateBrush* >& OutBrushes) const
{
	//OutBrushes.Add(&Background);
	OutBrushes.Add(&BackgroundNormal);
	OutBrushes.Add(&BackgroundRollover);
	OutBrushes.Add(&BackgroundActive);
	OutBrushes.Add(&BackgroundInactive);
	//OutBrushes.Add(&AmmoNormal);
	//OutBrushes.Add(&AmmoInactive);
}

const FInventoryItemStyle& FInventoryItemStyle::GetDefault()
{
	static FInventoryItemStyle Default;

	return Default;
}

const FName FInventoryItemStyle::GetTypeName() const
{
	static const FName NewTypeName = TEXT("FInventoryItemStyle");
	return NewTypeName;
}