// Copyright 2015 Michael Brandse

#include "PA_SlateStyles.h"
#include "PiratesAdventure.h"
#include "SlateGameResources.h" 

TSharedPtr<FSlateStyleSet> FPA_SlateStyles::PASlateStyleInstance = NULL;

void FPA_SlateStyles::Initialize()
{
	if (!PASlateStyleInstance.IsValid())
	{
		PASlateStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*PASlateStyleInstance);
	}
}

void FPA_SlateStyles::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*PASlateStyleInstance);
	ensure(PASlateStyleInstance.IsUnique());
	PASlateStyleInstance.Reset();
}

FName FPA_SlateStyles::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PASlateStyles"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FPA_SlateStyles::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FPA_SlateStyles::GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");
	return StyleRef;
}

const ISlateStyle& FPA_SlateStyles::Get()
{
	return *PASlateStyleInstance;
}