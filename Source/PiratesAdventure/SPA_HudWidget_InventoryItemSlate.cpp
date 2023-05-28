// Copyright 2015 Michael Brandse

#include "SPA_HudWidget_InventoryItemSlate.h"
#include "PiratesAdventure.h"

#define LOCTEXT_NAMESPACE "SPA_HudWidget_InventoryItemSlate"

void SPA_HudWidget_InventoryItemSlate::Construct(const FArguments& InArgs)
{
	check(InArgs._InventoryItemStyle != nullptr)
		InventoryItemStyle = InArgs._InventoryItemStyle;

	BackgroundNormal = InArgs._BackgroundNormal;
	BackgroundRollover = InArgs._BackgroundRollover;
	BackgroundActive = InArgs._BackgroundActive;
	BackgroundInactive = InArgs._BackgroundInactive;

	bHasAmmo = InArgs._HasAmmo;

	TitleText = InArgs._TitleText;
	AmmoText = InArgs._AmmoText;
	NewText = InArgs._NewText;

	Size = InArgs._Size;
	Scale = InArgs._Scale;

	//this->ChildSlot
	//[
	//		SNew(SHorizontalBox)
	//		+ SHorizontalBox::Slot()
	//	.AutoWidth()
	//	.VAlign(VAlign_Center)
	//	.HAlign(HAlign_Center)
	//	[
	//		SNew(SImage)
	//		.Image(this, &SPA_HudWidget_InventoryItemSlate::OnGetBackground)
	//	]
	//];

	BuildInventoryIcon();
}

void SPA_HudWidget_InventoryItemSlate::BuildInventoryIcon()
{
	this->ChildSlot
	[
		//SNew(SVerticalBox)
		//+ SVerticalBox::Slot()
		//.FillHeight(1.0f)
		SNew(SBox)
		.WidthOverride(Size.X)
		.HeightOverride(Size.Y)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(this, &SPA_HudWidget_InventoryItemSlate::OnGetBackground)
			]

			+ SOverlay::Slot()
			[
				SAssignNew(TitleTextContainer, STextBlock)
				//SNew(STextBlock)
				.TextStyle(OnGetTitleStyle())
				.Margin(InventoryItemStyle->TitleTextMargin)
				.Justification(ETextJustify::Center)
				.Text(TitleText)
			]

			//TODO: make sure this part only gets generated when we have a need for an ammo counter
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			[
				SAssignNew(AmmoTextContainer, STextBlock)
				.TextStyle(OnGetAmmoStyle())
				.Margin(InventoryItemStyle->AmmoTextMargin)
				.Justification(ETextJustify::Center)
				.Text(AmmoText)
			]
		]
		
		//+ SVerticalBox::Slot()
		//.AutoHeight()
		
		/*SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SImage)
			.Image(this, &SPA_HudWidget_InventoryItemSlate::OnGetBackground)
		]*/
	];
}

FVector2D SPA_HudWidget_InventoryItemSlate::ComputeDesiredSize(float) const
{
	return Size * Scale;
}

const FSlateBrush* SPA_HudWidget_InventoryItemSlate::OnGetBackground() const
{
	EInventoryItemState State = InventoryIconState.Get();

	const FSlateBrush* ImageToUse;
	switch (State)
	{
	case EInventoryItemState::Rollover:
		ImageToUse = GetBackgroundRollover();
		break;

	case EInventoryItemState::Active:
		ImageToUse = GetBackgroundActive();
		break;

	case EInventoryItemState::Inactive:
		ImageToUse = GetBackgroundInactive();
		break;

	default:
		ImageToUse = GetBackgroundNormal();
		break;
	}

	return ImageToUse;
}

const FTextBlockStyle* SPA_HudWidget_InventoryItemSlate::OnGetTitleStyle() const
{
	EInventoryItemState State = InventoryIconState.Get();

	const FTextBlockStyle* TitleStyleToUse;
	switch (State)
	{
	case EInventoryItemState::Rollover:
		TitleStyleToUse = &InventoryItemStyle->TitleTextRolloverStyle;
		break;

	case EInventoryItemState::Active:
		TitleStyleToUse = &InventoryItemStyle->TitleTextActiveStyle;
		break;

	case EInventoryItemState::Inactive:
		TitleStyleToUse = &InventoryItemStyle->TitleTextInactiveStyle;
		break;

	default:
		TitleStyleToUse = &InventoryItemStyle->TitleTextNormalStyle;
		break;
	}

	return TitleStyleToUse;
}

const FTextBlockStyle* SPA_HudWidget_InventoryItemSlate::OnGetAmmoStyle() const
{
	EInventoryItemState State = InventoryIconState.Get();
	int32 CurrentAmmoCount;
	if (AmmoTextContainer.IsValid())
	{
		CurrentAmmoCount = FCString::Atoi(*AmmoTextContainer->GetText().ToString());
	}
	else
	{
		CurrentAmmoCount = INDEX_NONE;
	}

	const FTextBlockStyle* AmmoStyleToUse;
	switch (State)
	{
	case EInventoryItemState::Inactive:
		AmmoStyleToUse = &InventoryItemStyle->AmmoTextInactiveStyle;
		break;

	default:
		if (AmmoTextContainer.IsValid()
			&& CurrentAmmoCount <= 0)
		{
			AmmoStyleToUse = &InventoryItemStyle->AmmoTextZeroStyle;
		}
		else
			AmmoStyleToUse = &InventoryItemStyle->AmmoTextNormalStyle;
		break;
	}

	return AmmoStyleToUse;
}

void SPA_HudWidget_InventoryItemSlate::SetInventoryIconState(TAttribute<EInventoryItemState> InNewState)
{
	InventoryIconState = InNewState;

	BuildInventoryIcon();
}

EInventoryItemState SPA_HudWidget_InventoryItemSlate::GetInventoryItemState()
{
	return InventoryIconState.Get();
}

FText SPA_HudWidget_InventoryItemSlate::GetAmmoText() const
{
	return AmmoText;
}

FText SPA_HudWidget_InventoryItemSlate::GetTitleText() const
{
	return TitleText;
}

FText SPA_HudWidget_InventoryItemSlate::GetNewText() const
{
	return NewText;
}

void SPA_HudWidget_InventoryItemSlate::SetBackgroundNormal(const FSlateBrush* Brush)
{
	BackgroundNormal = Brush;
}

void SPA_HudWidget_InventoryItemSlate::SetBackgroundRollover(const FSlateBrush* Brush)
{
	BackgroundRollover = Brush;
}

void SPA_HudWidget_InventoryItemSlate::SetBackgroundActive(const FSlateBrush* Brush)
{
	BackgroundActive = Brush;
}

void SPA_HudWidget_InventoryItemSlate::SetBackgroundInactive(const FSlateBrush* Brush)
{
	BackgroundInactive = Brush;
}

const FSlateBrush* SPA_HudWidget_InventoryItemSlate::GetBackgroundNormal() const
{
	return BackgroundNormal ? BackgroundNormal : &InventoryItemStyle->BackgroundNormal;
}

const FSlateBrush* SPA_HudWidget_InventoryItemSlate::GetBackgroundRollover() const
{
	return BackgroundRollover ? BackgroundRollover : &InventoryItemStyle->BackgroundRollover;
}

const FSlateBrush* SPA_HudWidget_InventoryItemSlate::GetBackgroundActive() const
{
	return BackgroundActive ? BackgroundActive : &InventoryItemStyle->BackgroundActive;
}

const FSlateBrush* SPA_HudWidget_InventoryItemSlate::GetBackgroundInactive() const
{
	return BackgroundInactive ? BackgroundInactive : &InventoryItemStyle->BackgroundInactive;
}

void SPA_HudWidget_InventoryItemSlate::SetTitleText(FText Text)
{
	if (TitleTextContainer.IsValid())
	{
		TitleText = Text;
		TitleTextContainer->SetText(TitleText);
	}
}

void SPA_HudWidget_InventoryItemSlate::SetAmmoCount(int32 Ammo)
{
	if (AmmoTextContainer.IsValid())
	{
		if (Ammo <= 0) Ammo = 0;
		AmmoText = FText::AsNumber(Ammo);
		AmmoTextContainer->SetText(AmmoText);

		AmmoTextContainer->SetTextStyle(OnGetAmmoStyle());
	}
}

void SPA_HudWidget_InventoryItemSlate::SetNewText(FText Text)
{
	if (NewTextContainer.IsValid())
	{
		NewText = Text;
		NewTextContainer->SetText(NewText);
	}
}

void SPA_HudWidget_InventoryItemSlate::SetInventoryStyle(FInventoryItemStyle* NewStyle)
{
	//InventoryItemStyle = NewStyle;
	InventoryItemStyle = NewStyle;

	if (InventoryItemStyle == nullptr)
	{
		FArguments Defaults;
		InventoryItemStyle = Defaults._InventoryItemStyle;
	}

	check(InventoryItemStyle);

	BuildInventoryIcon();
}

#undef LOCTEXT_NAMESPACE