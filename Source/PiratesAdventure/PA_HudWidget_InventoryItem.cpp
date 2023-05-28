// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_HudWidget_InventoryItem.h"
#include "PiratesAdventure.h"
#include "PA_HudWidget_InventoryItemStyle.h"
#include "Slate/SlateBrushAsset.h"
#include "SPA_HudWidget_InventoryItemSlate.h"
//#include "PA_ISMA_InteractiveBook.h"

UPA_HudWidget_InventoryItem::UPA_HudWidget_InventoryItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//BackgroundBrush = WidgetStyle.BackgroundNormal;
	//BackgroundBrush.ImageSize = FVector2D(WidgetStyle.BackgroundIcon.IconSize, WidgetStyle.BackgroundIcon.IconSize);
	//AmmoBrush = WidgetStyle.AmmoNormal;
	//AmmoBrush.ImageSize = FVector2D(WidgetStyle.AmmoIcon.IconSize, WidgetStyle.AmmoIcon.IconSize);

	//this won't work for some reason, so let's leave it alone.
	//SBK_UIWidget_ItemboxSimple::FArguments DefaultItemBox;
	//WidgetStyle = *DefaultItemBox._ItemboxStyle;
}

TSharedRef<SWidget> UPA_HudWidget_InventoryItem::RebuildWidget()
{
	//UE_LOG(AnyLog, Log, TEXT("REBUILT THE WIDGET"));

	SetIcons();
	BackgroundBrush.ImageSize = FVector2D(175.f, 175.f);
	AmmoBrush.ImageSize = FVector2D(50.f, 50.f);
	//BackgroundBrush.ImageSize = FVector2D(WidgetStyle.BackgroundIcon.IconSize, WidgetStyle.BackgroundIcon.IconSize);
	//AmmoBrush.ImageSize = FVector2D(WidgetStyle.AmmoIcon.IconSize, WidgetStyle.AmmoIcon.IconSize);

	//if (BackgroundCanTile())
	//{
	//	SetBackgroundIconTile(WidgetStyle.TilingMatInterface, WidgetStyle.BackgroundIcon.IconTexture, WidgetStyle.BackgroundIcon.IconCoordinates);
	//}

	//if (AmmoCanTile())
	//{
	//	SetAmmoIconTile(WidgetStyle.TilingMatInterface, WidgetStyle.AmmoIcon.IconTexture, WidgetStyle.AmmoIcon.IconCoordinates);
	//}

	MyInventoryItem = SNew(SPA_HudWidget_InventoryItemSlate)
		//.BackgroundImage(&BackgroundBrush)
		//.AmmoImage(&AmmoBrush)
		.InventoryItemStyle(&InventoryItemStyle)
		//.TextStyle(&WidgetStyle.AmmoTextStyle)
		//.AmmoText(AmmoText)
		;

	return MyInventoryItem.ToSharedRef();
	//return BuildDesignTimeWidget(MyInventoryItem.ToSharedRef());
}

void UPA_HudWidget_InventoryItem::SetIcons()
{
	BackgroundBrush = InventoryItemStyle.BackgroundNormal;
	//AmmoBrush = WidgetStyle.AmmoNormal;

	//if (MyInventoryItem.IsValid())
	//{
	//	if (!MyInventoryItem->IsEnabled() || WidgetStyle.ItemState == EItemIconState::ITEM_DISABLED)
	//	{
	//		BackgroundBrush = WidgetStyle.BackgroundInactive;
	//		AmmoBrush = WidgetStyle.AmmoInactive;
	//	}
	//	else if (WidgetStyle.ItemState == EItemIconState::ITEM_SELECTED)
	//	{
	//		BackgroundBrush = WidgetStyle.BackgroundSelected;
	//		AmmoBrush = WidgetStyle.AmmoSelected;
	//	}
	//	else
	//	{
	//		BackgroundBrush = WidgetStyle.BackgroundNormal;
	//		AmmoBrush = WidgetStyle.AmmoNormal;
	//	}

	//	MyInventoryItem->BackgroundImage = &BackgroundBrush;
	//	MyInventoryItem->AmmoImage = &AmmoBrush;
	//}
	//else
	//{
	//	BackgroundBrush = WidgetStyle.BackgroundNormal;
	//	AmmoBrush = WidgetStyle.AmmoNormal;
	//}
}

void UPA_HudWidget_InventoryItem::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyInventoryItem.Reset();
}

void UPA_HudWidget_InventoryItem::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyInventoryItem.IsValid())
	{
		if (!TitleText.IsEmpty()) MyInventoryItem->SetTitleText(TitleText);
		if (AmmoCount != INDEX_NONE) MyInventoryItem->SetAmmoCount(AmmoCount);
		//TAttribute<const FSlateBrush*> BackgroundBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, BackgroundBrush, const FSlateBrush*, ConvertBackgroundImage);
		//TAttribute<const FSlateBrush*> AmmoBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, AmmoBrush, const FSlateBrush*, ConvertAmmoImage);

		//MyInventoryItem->SetWidgetColorAndOpacity(WidgetColorAndOpacity);
		//MyInventoryItem->SetBackgroundImage(BackgroundBinding);
		//MyInventoryItem->SetAmmoImage(AmmoBinding);
	}
}

void UPA_HudWidget_InventoryItem::SetInventoryItemState(EInventoryItemState NewState)
{
	if (MyInventoryItem.IsValid())
	{
		MyInventoryItem->SetInventoryIconState(NewState);
	}
}

EInventoryItemState UPA_HudWidget_InventoryItem::GetInventoryItemState()
{
	if (MyInventoryItem.IsValid())
	{
		return MyInventoryItem->GetInventoryItemState();
	}

	return EInventoryItemState::Normal;
}

const FSlateBrush* UPA_HudWidget_InventoryItem::ConvertBackgroundImage(TAttribute<FSlateBrush> InImageAsset) const
{
	UPA_HudWidget_InventoryItem* MutableThis = const_cast<UPA_HudWidget_InventoryItem*>(this);
	MutableThis->BackgroundBrush = InImageAsset.Get();

	return &BackgroundBrush;
}

const FSlateBrush* UPA_HudWidget_InventoryItem::ConvertAmmoImage(TAttribute<FSlateBrush> InImageAsset) const
{
	UPA_HudWidget_InventoryItem* MutableThis = const_cast<UPA_HudWidget_InventoryItem*>(this);
	MutableThis->AmmoBrush = InImageAsset.Get();

	return &AmmoBrush;
}

FMargin UPA_HudWidget_InventoryItem::GetAmmoMargin(FMargin MainMargin, FVector2D MainSize, FMargin CurrentMargin, FVector2D CurrentSize)
{
	FMargin NewMargin = CurrentMargin;

	NewMargin.Left = MainMargin.Left + MainSize.X - (CurrentSize.X * 0.5);
	NewMargin.Left += CurrentMargin.Left;
	NewMargin.Top = MainMargin.Top + MainSize.Y - (CurrentSize.Y * 0.5);
	NewMargin.Top += CurrentMargin.Top;

	return NewMargin;
}

int32 UPA_HudWidget_InventoryItem::GetAmmoCount()
{
	if (MyInventoryItem.IsValid())
	{
		return FCString::Atoi(*MyInventoryItem->AmmoText.ToString());
	}

	return INDEX_NONE;
	//return FCString::Atoi(*AmmoText.ToString());
}

bool UPA_HudWidget_InventoryItem::BackgroundCanTile()
{
	return false;

	//if (WidgetStyle.BackgroundIcon.bIsTile)
	//{
	//	if (WidgetStyle.BackgroundIcon.IconSize <= 0.f)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Background icon tilesize is 0 or less. Check the itemproperties and update the itembox."));
	//		return false;
	//	}

	//	if (!WidgetStyle.TilingMatInterface)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No material has been added to TilingMatInterface. Check the itemproperties and update the itembox."));
	//		return false;
	//	}

	//	if (!WidgetStyle.BackgroundIcon.IconTexture)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Background icon is missing textures. Check the itemproperties and update the itembox."));
	//		return false;
	//	}

	//	return true;
	//}

	//return false;
}

bool UPA_HudWidget_InventoryItem::AmmoCanTile()
{
	/*if (WidgetStyle.AmmoIcon.bIsTile && WidgetStyle.bHasAmmo)
	{
		if (WidgetStyle.AmmoIcon.IconSize <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ammo icon tilesize is 0 or less. Check the itemproperties and update the itembox."));
			return false;
		}

		if (!WidgetStyle.TilingMatInterface)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No material has been added to TilingMatInterface. Check the itemproperties and update the itembox."));
			return false;
		}

		if (!WidgetStyle.AmmoIcon.IconTexture)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ammo icon is missing textures. Check the itemproperties and update the itembox."));
			return false;
		}

		return true;
	}*/

	return false;
}

//UMaterialInstanceDynamic* UBK_UIWidget_ItemboxSimple::SetBackgroundIconTile(UMaterialInterface* TilingMaterial, UTexture2D* TilingTexture, FVector2D TileCoordinates)
//{
//	BackgroundBrush.SetResourceObject(TilingMaterial);
//
//	UMaterialInterface* Material = NULL;
//
//	UObject* Resource = BackgroundBrush.GetResourceObject();
//	Material = Cast<UMaterialInterface>(Resource);
//
//	if (Material || WidgetStyle.BackgroundIcon.IconSize <= 0.f)
//	{
//		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
//
//		if (!DynamicMaterial)
//		{
//			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
//			BackgroundBrush.SetResourceObject(DynamicMaterial);
//
//			if (MyInventoryItem.IsValid())
//			{
//				MyInventoryItem->SetBackgroundImage(&BackgroundBrush);
//			}
//		}
//
//		FVector2D TextureSize = FVector2D(TilingTexture->GetSizeX(), TilingTexture->GetSizeY());
//
//		DynamicMaterial->SetTextureParameterValue("IconTexture", TilingTexture);
//		DynamicMaterial->SetScalarParameterValue("TextureSizeX", TextureSize.X);
//		DynamicMaterial->SetScalarParameterValue("TextureSizeY", TextureSize.Y);
//		DynamicMaterial->SetScalarParameterValue("TileSize", WidgetStyle.BackgroundIcon.IconSize);
//		DynamicMaterial->SetScalarParameterValue("TileX", TileCoordinates.X);
//		DynamicMaterial->SetScalarParameterValue("TileY", TileCoordinates.Y);
//
//		return DynamicMaterial;
//	}
//
//	return NULL;
//}
//
//UMaterialInstanceDynamic* UBK_UIWidget_ItemboxSimple::SetAmmoIconTile(UMaterialInterface* TilingMaterial, UTexture2D* TilingTexture, FVector2D TileCoordinates)
//{
//	AmmoBrush.SetResourceObject(TilingMaterial);
//
//	UMaterialInterface* Material = NULL;
//
//	UObject* Resource = AmmoBrush.GetResourceObject();
//	Material = Cast<UMaterialInterface>(Resource);
//
//	if (Material || WidgetStyle.AmmoIcon.IconSize <= 0.f)
//	{
//		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
//
//		if (!DynamicMaterial)
//		{
//			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
//			AmmoBrush.SetResourceObject(DynamicMaterial);
//
//			if (MyInventoryItem.IsValid())
//			{
//				MyInventoryItem->SetAmmoImage(&AmmoBrush);
//			}
//		}
//
//		FVector2D TextureSize = FVector2D(TilingTexture->GetSizeX(), TilingTexture->GetSizeY());
//
//		DynamicMaterial->SetTextureParameterValue("IconTexture", TilingTexture);
//		DynamicMaterial->SetScalarParameterValue("TextureSizeX", TextureSize.X);
//		DynamicMaterial->SetScalarParameterValue("TextureSizeY", TextureSize.Y);
//		DynamicMaterial->SetScalarParameterValue("TileSize", WidgetStyle.BackgroundIcon.IconSize);
//		DynamicMaterial->SetScalarParameterValue("TileX", TileCoordinates.X);
//		DynamicMaterial->SetScalarParameterValue("TileY", TileCoordinates.Y);
//
//		return DynamicMaterial;
//	}
//
//	return NULL;
//}

void UPA_HudWidget_InventoryItem::SetWidgetOpacity(float InOpacity)
{
	WidgetColorAndOpacity.A = InOpacity;
	if (MyInventoryItem.IsValid())
	{
		//MyInventoryItem->SetWidgetColorAndOpacity(WidgetColorAndOpacity);
	}
}

void UPA_HudWidget_InventoryItem::SetWidgetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	if (MyInventoryItem.IsValid())
	{
		//MyInventoryItem->SetWidgetColorAndOpacity(InColorAndOpacity);
	}
}

void UPA_HudWidget_InventoryItem::SetBackgroundColorAndOpacity(FLinearColor InColorAndOpacity)
{
	BackgroundBrush.TintColor = InColorAndOpacity;
}

void UPA_HudWidget_InventoryItem::SetAmmoColorAndOpacity(FLinearColor InColorAndOpacity)
{
	AmmoBrush.TintColor = InColorAndOpacity;
}

void UPA_HudWidget_InventoryItem::SetTitleText(FText NewText)
{
	TitleText = NewText;

	if (MyInventoryItem.IsValid())
	{
		MyInventoryItem->SetTitleText(NewText);
	}
}

void UPA_HudWidget_InventoryItem::SetAmmoCount(int32 NewValue)
{
	AmmoCount = NewValue;

	if (MyInventoryItem.IsValid())
	{
		MyInventoryItem->SetAmmoCount(NewValue);
	}
}

void UPA_HudWidget_InventoryItem::SetInventoryStyle(FInventoryItemStyle NewStyle)
{
	InventoryItemStyle = NewStyle;

	if (MyInventoryItem.IsValid())
		MyInventoryItem->SetInventoryStyle(&NewStyle);
}

void UPA_HudWidget_InventoryItem::SetSize(FVector2D NewSize)
{
	if (MyInventoryItem.IsValid())
		MyInventoryItem->SetSize(NewSize);
}

void UPA_HudWidget_InventoryItem::SetScale(FVector2D NewScale)
{
	if (MyInventoryItem.IsValid())
		MyInventoryItem->SetScale(NewScale);
}