// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_StaticLibrary.h"
#include "PA_Asset_Water.generated.h"

UENUM(BlueprintType)
enum class EWaterType : uint8
{
	WT_Lake UMETA(DisplayName = "Lake"),
	WT_Ocean UMETA(DisplayName = "Ocean"),
	WT_River UMETA(DisplayName = "River")
};

USTRUCT(BlueprintType)
struct FWaterSettingsGlobal
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EWaterType WaterType = EWaterType::WT_Lake;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EWaterMaterialType MaterialType = EWaterMaterialType::MT_PBR_Transparant_Tesselated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) //R
		float RiverFlowDirection = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) //R
		float RiverFlowDisplacement = 30.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WaveSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float GlobalOpacity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ShoreDistance = 150.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float DayIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WaterDepth = 500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SunReflectionIntensity = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SunReflectionTightness = 4000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TesselationFactor = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool UpdateWatermeshRuntime = true;
};

USTRUCT(BlueprintType)
struct FWaterSettingsGerstnerWaves
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WaveLength = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Amplitude = 0.8f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Steepness = 0.8f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Speed = 150.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) //S
		float WindDirection = 0.651376f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float NumberOfSummedWaves = 12.f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsDistances
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ColorDistanceFade = 6000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ColorDistanceFalloff = 3000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WaterDistanceFade = 3000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WaterDistanceFalloff = 1500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FoamDistanceFade = 6000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FoamDistanceFalloff = 3000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float NormalDistanceFade = 6000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float NormalDistanceFalloff = 3000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RefractionDistanceFade = 500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RefractionDistanceFalloff = 500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SSSDistanceFade = 6000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SSSDistanceFalloff = 3000.f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsNormals
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SmallNormalSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SmallNormalIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SmallNormalTiling = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float LargeNormalSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float LargeNormalIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float LargeNormalTiling = 2.f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsFoam
{
	GENERATED_USTRUCT_BODY()

public:

	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FoamIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FoamSpeed = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FoamXTile = 3.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FoamYTile = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float IntersectionSize = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float IntersectionSoftness = 50.f;

	//Sea and Lake
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SeafoamIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SeafoamBaseHeight = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SeafoamSoftness = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SeafoamDistortion = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SeafoamTiling = 2.f;

	//River
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RiverfoamSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RiverfoamIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RiverfoamDistortion = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RiverfoamTiling = 3.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ObjectIntersectionMaskSize = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ObjectIntersectionMaskFalloff = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ObjectIntersectionSoftness = 200.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ObjectIntersectionIntensity = 0.6f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsReflectionRefraction
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float CubemapIntensity = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ReflectionAngle = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SpecularIntensity = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RefractionAmount = 1.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RefractionViewAngle = 10.f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsHighlight
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Intensity = 30.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Baseheight = -5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Softness = 20.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Density = 0.6f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsThrough
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Intensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Baseheight = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Softness = 100.f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsSSS
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Intensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Baseheight = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Softness = 50.f;
};

USTRUCT(BlueprintType)
struct FWaterSettingsColor
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor BaseColor = FLinearColor(0.000000, 0.078604, 0.260000, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor FarColor = FLinearColor(0.000000, 0.238608, 0.740000, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor ShallowColor = FLinearColor(0.171441, 0.887923, 0.412543, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor ThroughColor = FLinearColor(0.000000, 0.035000, 0.150000, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor SSSColor = FLinearColor(0.000000, 0.885000, 0.489376, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor FoamColor = FLinearColor(1.000000, 1.000000, 1.000000, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere) //S+L
		FLinearColor SeafoamColor = FLinearColor(1.000000, 1.000000, 1.000000, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditAnywhere) //R
		FLinearColor RiverfoamColor = FLinearColor(1.000000, 1.000000, 1.000000, 1.000000);
};

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_Asset_Water : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsGlobal SettingsGlobal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsGerstnerWaves SettingsGerstnerWaves;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsDistances SettingsDistances;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsNormals SettingsNormals;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsFoam SettingsFoam;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsReflectionRefraction SettingsReflectionRefraction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsHighlight SettingsHighlight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsThrough SettingsThrough;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsSSS SettingsSSS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaterSettingsColor SettingsColor;	
};
