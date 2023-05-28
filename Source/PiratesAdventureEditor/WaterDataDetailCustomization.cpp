#include "WaterDataDetailCustomization.h"
#include "PiratesAdventureEditor.h"
#include "PA_Asset_Water.h"
#include "PropertyEditing.h"
#include "SlateBasics.h"
#include "SlateFontInfo.h"
#include "STextComboBox.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"
#include "DetailLayoutBuilder.h"

#define LOCTEXT_NAMESPACE "WaterDataDetailCustomization"

//////////////////////////////////////////////////////////////////////////
// FInventoryItemDetailCustomization

TSharedRef<IDetailCustomization> FWaterDataDetailCustomization::MakeInstance()
{
	return MakeShareable(new FWaterDataDetailCustomization);
}

void FWaterDataDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	DetailLayoutBuilder = &DetailLayout;

	//Get all structs from the water data
	const TSharedPtr<IPropertyHandle> GlobalSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsGlobal));
	DetailLayout.HideProperty(GlobalSettings);
	const TSharedPtr<IPropertyHandle> GerstnerSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsGerstnerWaves));
	DetailLayout.HideProperty(GerstnerSettings);
	const TSharedPtr<IPropertyHandle> DistanceSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsDistances));
	DetailLayout.HideProperty(DistanceSettings);
	const TSharedPtr<IPropertyHandle> NormalSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsNormals));
	DetailLayout.HideProperty(NormalSettings);
	const TSharedPtr<IPropertyHandle> FoamSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsFoam));
	DetailLayout.HideProperty(FoamSettings);
	const TSharedPtr<IPropertyHandle> RRSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsReflectionRefraction));
	DetailLayout.HideProperty(RRSettings);
	const TSharedPtr<IPropertyHandle> HighlightSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsHighlight));
	DetailLayout.HideProperty(HighlightSettings);
	const TSharedPtr<IPropertyHandle> ThroughSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsThrough));
	DetailLayout.HideProperty(ThroughSettings);
	const TSharedPtr<IPropertyHandle> SSSSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsSSS));
	DetailLayout.HideProperty(SSSSettings);
	const TSharedPtr<IPropertyHandle> ColorSettings = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPA_Asset_Water, SettingsColor));
	DetailLayout.HideProperty(ColorSettings);

	IDetailCategoryBuilder& CatSelector = DetailLayout.EditCategory("Water Type", LOCTEXT("WaterDataSelector", "Water Type"), ECategoryPriority::Important);
	IDetailCategoryBuilder& CatGlobal = DetailLayout.EditCategory("Global Settings", LOCTEXT("WaterDataGlobal", "Global Settings"), ECategoryPriority::Default);	
	IDetailCategoryBuilder& CatGerstner = DetailLayout.EditCategory("Gerstner Wave Settings", LOCTEXT("WaterDataGerstner", "Gerstner Wave Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatDistance = DetailLayout.EditCategory("Distance Settings", LOCTEXT("WaterDataDistance", "Distance Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatNormal = DetailLayout.EditCategory("Normal Settings", LOCTEXT("WaterDataNormal", "Normal Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatFoam = DetailLayout.EditCategory("Foam Settings", LOCTEXT("WaterDataFoam", "Foam Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatRR = DetailLayout.EditCategory("Reflection and Refraction Settings", LOCTEXT("WaterDataRR", "Reflection and Refraction Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatHiglight = DetailLayout.EditCategory("Highlight Settings", LOCTEXT("WaterDataHighlight", "Highlight Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatThrough = DetailLayout.EditCategory("Through Settings", LOCTEXT("WaterDataThrough", "Through Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatSSS = DetailLayout.EditCategory("Subsurface Scattering Settings", LOCTEXT("WaterDataSSS", "Subsurface Scattering Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& CatColor = DetailLayout.EditCategory("Color Settings", LOCTEXT("WaterDataColor", "Color Settings"), ECategoryPriority::Default);

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;

	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	TArray<UObject*> StrongObjects;
	CopyFromWeakArray(StrongObjects, ObjectsBeingCustomized);

	if (StrongObjects.Num() == 0) return;

	//UE_LOG(PAEditor, Warning, TEXT("Updated the Inventory Item Details a total of %d times"), Tester++);

	Water = Cast<UPA_Asset_Water>(StrongObjects[0]);
	if (Water)
	{
		PropertyOptions.Empty();
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EWaterType::WT_Lake))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EWaterType::WT_Ocean))));
		PropertyOptions.Add(MakeShareable(new FString(GetEnumAsString(EWaterType::WT_River))));

		uint8 DataValue = (uint8)Water->SettingsGlobal.WaterType;

		//Selector
		CatSelector.AddCustomRow(LOCTEXT("WaterType", "WaterType"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.Padding(0.f, 0.f, 2.f, 0.f)
				[
					SNew(STextComboBox)
					.OptionsSource(&PropertyOptions)
					.InitiallySelectedItem(PropertyOptions[DataValue])
					.OnSelectionChanged(this, &FWaterDataDetailCustomization::OnChangeProperty)
				]
			];

		//Global Settings
		const TSharedPtr<IPropertyHandle> MaterialTypeField = GlobalSettings->GetChildHandle("MaterialType");
		CatGlobal.AddProperty(MaterialTypeField);
		if (Water->SettingsGlobal.WaterType == EWaterType::WT_River)
		{
			const TSharedPtr<IPropertyHandle> RiverFlowDirectionField = GlobalSettings->GetChildHandle("RiverFlowDirection");
			CatGlobal.AddProperty(RiverFlowDirectionField);
			const TSharedPtr<IPropertyHandle> RiverFlowDisplacementField = GlobalSettings->GetChildHandle("RiverFlowDisplacement");
			CatGlobal.AddProperty(RiverFlowDisplacementField);
		}
		const TSharedPtr<IPropertyHandle> WaveSpeedField = GlobalSettings->GetChildHandle("WaveSpeed");
		CatGlobal.AddProperty(WaveSpeedField);
		const TSharedPtr<IPropertyHandle> GlobalOpacityField = GlobalSettings->GetChildHandle("GlobalOpacity");
		CatGlobal.AddProperty(GlobalOpacityField);
		const TSharedPtr<IPropertyHandle> ShoreDistanceField = GlobalSettings->GetChildHandle("ShoreDistance");
		CatGlobal.AddProperty(ShoreDistanceField);
		const TSharedPtr<IPropertyHandle> DayIntensityField = GlobalSettings->GetChildHandle("DayIntensity");
		CatGlobal.AddProperty(DayIntensityField);
		const TSharedPtr<IPropertyHandle> WaterDepthField = GlobalSettings->GetChildHandle("WaterDepth");
		CatGlobal.AddProperty(WaterDepthField);
		const TSharedPtr<IPropertyHandle> SunReflectionIntensityField = GlobalSettings->GetChildHandle("SunReflectionIntensity");
		CatGlobal.AddProperty(SunReflectionIntensityField);
		const TSharedPtr<IPropertyHandle> SunReflectionTightnessField = GlobalSettings->GetChildHandle("SunReflectionTightness");
		CatGlobal.AddProperty(SunReflectionTightnessField);
		const TSharedPtr<IPropertyHandle> TesselationFactorField = GlobalSettings->GetChildHandle("TesselationFactor");
		CatGlobal.AddProperty(TesselationFactorField);
		const TSharedPtr<IPropertyHandle> UpdateWatermeshRuntimeField = GlobalSettings->GetChildHandle("UpdateWatermeshRuntime");
		CatGlobal.AddProperty(UpdateWatermeshRuntimeField);

		//GerstnerSettings
		const TSharedPtr<IPropertyHandle> WaveLengthField = GerstnerSettings->GetChildHandle("WaveLength");
		CatGerstner.AddProperty(WaveLengthField);
		const TSharedPtr<IPropertyHandle> AmplitudeField = GerstnerSettings->GetChildHandle("Amplitude");
		CatGerstner.AddProperty(AmplitudeField);
		const TSharedPtr<IPropertyHandle> SteepnessField = GerstnerSettings->GetChildHandle("Steepness");
		CatGerstner.AddProperty(SteepnessField);
		const TSharedPtr<IPropertyHandle> SpeedField = GerstnerSettings->GetChildHandle("Speed");
		CatGerstner.AddProperty(SpeedField);
		if (Water->SettingsGlobal.WaterType == EWaterType::WT_Ocean)
		{
			const TSharedPtr<IPropertyHandle> WindDirectionField = GerstnerSettings->GetChildHandle("WindDirection");
			CatGerstner.AddProperty(WindDirectionField);
		}
		const TSharedPtr<IPropertyHandle> NumberOfSummedWavesField = GerstnerSettings->GetChildHandle("NumberOfSummedWaves");
		CatGerstner.AddProperty(NumberOfSummedWavesField);

		//DistanceSettings
		const TSharedPtr<IPropertyHandle> ColorDistanceFadeField = DistanceSettings->GetChildHandle("ColorDistanceFade");
		CatDistance.AddProperty(ColorDistanceFadeField);
		const TSharedPtr<IPropertyHandle> ColorDistanceFalloffField = DistanceSettings->GetChildHandle("ColorDistanceFalloff");
		CatDistance.AddProperty(ColorDistanceFalloffField);
		const TSharedPtr<IPropertyHandle> WaterDistanceFadeField = DistanceSettings->GetChildHandle("WaterDistanceFade");
		CatDistance.AddProperty(WaterDistanceFadeField);
		const TSharedPtr<IPropertyHandle> WaterDistanceFalloffField = DistanceSettings->GetChildHandle("WaterDistanceFalloff");
		CatDistance.AddProperty(WaterDistanceFalloffField);
		const TSharedPtr<IPropertyHandle> FoamDistanceFadeField = DistanceSettings->GetChildHandle("FoamDistanceFade");
		CatDistance.AddProperty(FoamDistanceFadeField);
		const TSharedPtr<IPropertyHandle> FoamDistanceFalloffField = DistanceSettings->GetChildHandle("FoamDistanceFalloff");
		CatDistance.AddProperty(FoamDistanceFalloffField);
		const TSharedPtr<IPropertyHandle> NormalDistanceFadeField = DistanceSettings->GetChildHandle("NormalDistanceFade");
		CatDistance.AddProperty(NormalDistanceFadeField);
		const TSharedPtr<IPropertyHandle> NormalDistanceFalloffField = DistanceSettings->GetChildHandle("NormalDistanceFalloff");
		CatDistance.AddProperty(NormalDistanceFalloffField);
		const TSharedPtr<IPropertyHandle> RefractionDistanceFadeField = DistanceSettings->GetChildHandle("RefractionDistanceFade");
		CatDistance.AddProperty(RefractionDistanceFadeField);
		const TSharedPtr<IPropertyHandle> RefractionDistanceFalloffField = DistanceSettings->GetChildHandle("RefractionDistanceFalloff");
		CatDistance.AddProperty(RefractionDistanceFalloffField);
		const TSharedPtr<IPropertyHandle> SSSDistanceFadeField = DistanceSettings->GetChildHandle("SSSDistanceFade");
		CatDistance.AddProperty(SSSDistanceFadeField);
		const TSharedPtr<IPropertyHandle> SSSDistanceFalloffField = DistanceSettings->GetChildHandle("SSSDistanceFalloff");
		CatDistance.AddProperty(SSSDistanceFalloffField);

		//Normals
		const TSharedPtr<IPropertyHandle> SmallNormalSpeedField = NormalSettings->GetChildHandle("SmallNormalSpeed");
		CatNormal.AddProperty(SmallNormalSpeedField);
		const TSharedPtr<IPropertyHandle> SmallNormalIntensityField = NormalSettings->GetChildHandle("SmallNormalIntensity");
		CatNormal.AddProperty(SmallNormalIntensityField);
		const TSharedPtr<IPropertyHandle> SmallNormalTilingField = NormalSettings->GetChildHandle("SmallNormalTiling");
		CatNormal.AddProperty(SmallNormalTilingField);
		const TSharedPtr<IPropertyHandle> LargeNormalSpeedField = NormalSettings->GetChildHandle("LargeNormalSpeed");
		CatNormal.AddProperty(LargeNormalSpeedField);
		const TSharedPtr<IPropertyHandle> LargeNormalIntensityField = NormalSettings->GetChildHandle("LargeNormalIntensity");
		CatNormal.AddProperty(LargeNormalIntensityField);
		const TSharedPtr<IPropertyHandle> LargeNormalTilingField = NormalSettings->GetChildHandle("LargeNormalTiling");
		CatNormal.AddProperty(LargeNormalTilingField);

		//Foam
		const TSharedPtr<IPropertyHandle> FoamIntensityField = FoamSettings->GetChildHandle("FoamIntensity");
		CatFoam.AddProperty(FoamIntensityField);
		const TSharedPtr<IPropertyHandle> FoamSpeedField = FoamSettings->GetChildHandle("FoamSpeed");
		CatFoam.AddProperty(FoamSpeedField);
		const TSharedPtr<IPropertyHandle> FoamXTileField = FoamSettings->GetChildHandle("FoamXTile");
		CatFoam.AddProperty(FoamXTileField);
		const TSharedPtr<IPropertyHandle> FoamYTileField = FoamSettings->GetChildHandle("FoamYTile");
		CatFoam.AddProperty(FoamYTileField);
		const TSharedPtr<IPropertyHandle> IntersectionSizeField = FoamSettings->GetChildHandle("IntersectionSize");
		CatFoam.AddProperty(IntersectionSizeField);
		const TSharedPtr<IPropertyHandle> IntersectionSoftnessField = FoamSettings->GetChildHandle("IntersectionSoftness");
		CatFoam.AddProperty(IntersectionSoftnessField);
		if (Water->SettingsGlobal.WaterType != EWaterType::WT_River)
		{
			const TSharedPtr<IPropertyHandle> SeafoamIntensityField = FoamSettings->GetChildHandle("SeafoamIntensity");
			CatFoam.AddProperty(SeafoamIntensityField);
			const TSharedPtr<IPropertyHandle> SeafoamBaseHeightField = FoamSettings->GetChildHandle("SeafoamBaseHeight");
			CatFoam.AddProperty(SeafoamBaseHeightField);
			const TSharedPtr<IPropertyHandle> SeafoamSoftnessField = FoamSettings->GetChildHandle("SeafoamSoftness");
			CatFoam.AddProperty(SeafoamSoftnessField);
			const TSharedPtr<IPropertyHandle> SeafoamDistortionField = FoamSettings->GetChildHandle("SeafoamDistortion");
			CatFoam.AddProperty(SeafoamDistortionField);
			const TSharedPtr<IPropertyHandle> SeafoamTilingField = FoamSettings->GetChildHandle("SeafoamTiling");
			CatFoam.AddProperty(SeafoamTilingField);
		}
		else
		{
			const TSharedPtr<IPropertyHandle> RiverfoamSpeedField = FoamSettings->GetChildHandle("RiverfoamSpeed");
			CatFoam.AddProperty(RiverfoamSpeedField);
			const TSharedPtr<IPropertyHandle> RiverfoamIntensityField = FoamSettings->GetChildHandle("RiverfoamIntensity");
			CatFoam.AddProperty(RiverfoamIntensityField);
			const TSharedPtr<IPropertyHandle> RiverfoamDistortionField = FoamSettings->GetChildHandle("RiverfoamDistortion");
			CatFoam.AddProperty(RiverfoamDistortionField);
			const TSharedPtr<IPropertyHandle> RiverfoamTilingField = FoamSettings->GetChildHandle("RiverfoamTiling");
			CatFoam.AddProperty(RiverfoamTilingField);
			const TSharedPtr<IPropertyHandle> ObjectIntersectionMaskSizeField = FoamSettings->GetChildHandle("ObjectIntersectionMaskSize");
			CatFoam.AddProperty(ObjectIntersectionMaskSizeField);
			const TSharedPtr<IPropertyHandle> ObjectIntersectionMaskFalloffField = FoamSettings->GetChildHandle("ObjectIntersectionMaskFalloff");
			CatFoam.AddProperty(ObjectIntersectionMaskFalloffField);
			const TSharedPtr<IPropertyHandle> ObjectIntersectionSoftnessField = FoamSettings->GetChildHandle("ObjectIntersectionSoftness");
			CatFoam.AddProperty(ObjectIntersectionSoftnessField);
			const TSharedPtr<IPropertyHandle> ObjectIntersectionIntensityField = FoamSettings->GetChildHandle("ObjectIntersectionIntensity");
			CatFoam.AddProperty(ObjectIntersectionIntensityField);
		}

		//Reflection and Refraction
		const TSharedPtr<IPropertyHandle> CubemapIntensityField = RRSettings->GetChildHandle("CubemapIntensity");
		CatRR.AddProperty(CubemapIntensityField);
		const TSharedPtr<IPropertyHandle> ReflectionAngleField = RRSettings->GetChildHandle("ReflectionAngle");
		CatRR.AddProperty(ReflectionAngleField);
		const TSharedPtr<IPropertyHandle> SpecularIntensityField = RRSettings->GetChildHandle("SpecularIntensity");
		CatRR.AddProperty(SpecularIntensityField);
		const TSharedPtr<IPropertyHandle> RefractionAmountField = RRSettings->GetChildHandle("RefractionAmount");
		CatRR.AddProperty(RefractionAmountField);
		const TSharedPtr<IPropertyHandle> RefractionViewAngleField = RRSettings->GetChildHandle("RefractionViewAngle");
		CatRR.AddProperty(RefractionViewAngleField);

		//Highlight
		const TSharedPtr<IPropertyHandle> HLIntensityField = HighlightSettings->GetChildHandle("Intensity");
		CatHiglight.AddProperty(HLIntensityField);
		const TSharedPtr<IPropertyHandle> HLBaseheightField = HighlightSettings->GetChildHandle("Baseheight");
		CatHiglight.AddProperty(HLBaseheightField);
		const TSharedPtr<IPropertyHandle> HLSoftnessField = HighlightSettings->GetChildHandle("Softness");
		CatHiglight.AddProperty(HLSoftnessField);
		const TSharedPtr<IPropertyHandle> HLDensityField = HighlightSettings->GetChildHandle("Density");
		CatHiglight.AddProperty(HLDensityField);

		//Through
		const TSharedPtr<IPropertyHandle> TIntensityField = ThroughSettings->GetChildHandle("Intensity");
		CatThrough.AddProperty(TIntensityField);
		const TSharedPtr<IPropertyHandle> TBaseheightField = ThroughSettings->GetChildHandle("Baseheight");
		CatThrough.AddProperty(TBaseheightField);
		const TSharedPtr<IPropertyHandle> TSoftnessField = ThroughSettings->GetChildHandle("Softness");
		CatThrough.AddProperty(TSoftnessField);

		//SSS
		const TSharedPtr<IPropertyHandle> SSSIntensityField = SSSSettings->GetChildHandle("Intensity");
		CatThrough.AddProperty(SSSIntensityField);
		const TSharedPtr<IPropertyHandle> SSSBaseheightField = SSSSettings->GetChildHandle("Baseheight");
		CatThrough.AddProperty(SSSBaseheightField);
		const TSharedPtr<IPropertyHandle> SSSSoftnessField = SSSSettings->GetChildHandle("Softness");
		CatThrough.AddProperty(SSSSoftnessField);

		//Colors
		const TSharedPtr<IPropertyHandle> BaseColorField = ColorSettings->GetChildHandle("BaseColor");
		CatColor.AddProperty(BaseColorField);
		const TSharedPtr<IPropertyHandle> FarColorField = ColorSettings->GetChildHandle("FarColor");
		CatColor.AddProperty(FarColorField);
		const TSharedPtr<IPropertyHandle> ShallowColorField = ColorSettings->GetChildHandle("ShallowColor");
		CatColor.AddProperty(ShallowColorField);
		const TSharedPtr<IPropertyHandle> ThroughColorField = ColorSettings->GetChildHandle("ThroughColor");
		CatColor.AddProperty(ThroughColorField);
		const TSharedPtr<IPropertyHandle> SSSColorField = ColorSettings->GetChildHandle("SSSColor");
		CatColor.AddProperty(SSSColorField);
		const TSharedPtr<IPropertyHandle> FoamColorField = ColorSettings->GetChildHandle("FoamColor");
		CatColor.AddProperty(FoamColorField);
		if (Water->SettingsGlobal.WaterType != EWaterType::WT_River)
		{
			const TSharedPtr<IPropertyHandle> SeafoamColorField = ColorSettings->GetChildHandle("SeafoamColor");
			CatColor.AddProperty(SeafoamColorField);
		}
		else
		{
			const TSharedPtr<IPropertyHandle> RiverfoamColorField = ColorSettings->GetChildHandle("RiverfoamColor");
			CatColor.AddProperty(RiverfoamColorField);
		}
	}
}

void FWaterDataDetailCustomization::OnChangeProperty(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	for (uint8 Index = 0; Index < PropertyOptions.Num(); Index++)
	{
		if (ItemSelected == PropertyOptions[Index])
		{
			Water->SettingsGlobal.WaterType = (EWaterType)Index;
			break;
		}
	}

	if (DetailLayoutBuilder)
		DetailLayoutBuilder->ForceRefreshDetails();
}

FString FWaterDataDetailCustomization::GetEnumAsString(EWaterType EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWaterType"), true);
	if (!EnumPtr) return TEXT("Invalid");

	return EnumPtr->GetDisplayNameTextByIndex((uint8)EnumValue).ToString();
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE