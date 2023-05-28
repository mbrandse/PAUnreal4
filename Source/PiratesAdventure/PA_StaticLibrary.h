// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "UObject/NoExportTypes.h"
#include "PA_StaticLibrary.generated.h"

UENUM(BlueprintType)
enum class EDamageWeight : uint8
{
	DW_LIGHT UMETA(DisplayName = "Light"),
	DW_HEAVY UMETA(DisplayName = "Heavy")
};

UENUM(BlueprintType)
enum class EDamageCategory : uint8
{
	DC_GROUND UMETA(DisplayName = "Ground"),
	DC_AIR UMETA(DisplayName = "Air"),
	DC_VOLUME UMETA(DisplayName = "Volume"),
	DC_SPIKES UMETA(DisplayName = "Spikes")
};

USTRUCT(Blueprintable)
struct FDamageAnimation
{
	GENERATED_USTRUCT_BODY()

public:

	//the number of the item in the HUD inventory list.  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		EDamageWeight DamageWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		EDamageCategory DamageCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		FVector AirImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		FVector GroundImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float RecoveryTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		int32 HitCount;
};


UENUM(BlueprintType)
enum class EPlayerWeaponType : uint8
{
	//This type should not be assigned to anything. It's so we can return NULLs.
	NONE UMETA(DisplayName = "None"), 	
	WEAPON_FIRE UMETA(DisplayName = "FireSkill"),
	WEAPON_ICE UMETA(DisplayName = "IceSkill"),
	WEAPON_WIND UMETA(DisplayName = "WindSkill"),
	WEAPON_WOOD UMETA(DisplayName = "WoodSkill"),
	WEAPON_DEFAULT UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EPlayerToolType : uint8
{
	//This type should not be assigned to anything. It's so we can return NULLs.
	NONE UMETA(DisplayName = "None"), 
	//The flintlock allows Morgan to shoot things from a distance.
	TOOL_GUN UMETA(DisplayName = "FlintLock"),
	//The lantarn allows Morgan to see in areas that are dark, or even to influence his surroundings based on the equipped element.
	TOOL_LANTARN UMETA(DisplayName = "Lantarn"),
	//The all seeing eye. Element is always default. Makes the invisible visible.
	TOOL_EYE UMETA(DisplayName = "Eye"), 
	//The grenade allows Morgan to create concentrated elemental bursts, but with limited range
	TOOL_GRENADE UMETA(DisplayName = "Grenade"),
	//The Spiral cane. Final boss item only, will cause massive burst of magic.
	TOOL_WAND UMETA(DisplayName = "Wand")

	/**
	//Normal consumables you can stockpile are also considered to be tools. Element is always default.
	TOOL_ITEM UMETA(DisplayName = "Item"), 
	//Necessary for making an HUD entry for the sword as well. The sword will actually never be called, so the template field can be left empty.
	TOOL_SWORD UMETA(DisplayName = "Sword") 
	**/
};

UENUM(BlueprintType)
enum class EISMAState : uint8
{
	//ISMA state has a number of neutral states that can be implemented in interactive meshes, as a replacement for AI callbacks. Not clean, but shit happens.
	ISMASTATE_None UMETA(DisplayName = "State None"),
	ISMASTATE_StateA UMETA(DisplayName = "State A"),
	ISMASTATE_StateB UMETA(DisplayName = "State B"),
	ISMASTATE_StateC UMETA(DisplayName = "State C"),
	ISMASTATE_StateD UMETA(DisplayName = "State D"),
	ISMASTATE_StateE UMETA(DisplayName = "State E"),
	ISMASTATE_StateF UMETA(Displayname = "State F")
};

UENUM(BlueprintType)
enum class EWaterMaterialType : uint8
{
	MT_UNLIT_Transparant_Tesselated UMETA(DisplayName = "UNLIT Transparant Tesselated"),
	MT_UNLIT_Transparant_NonTesselated UMETA(DisplayName = "UNLIT Transparant Non-Tesselated"),
	MT_UNLIT_NonTransparant_Tesselated UMETA(DisplayName = "UNLIT Non-Transparant Tesselated"),
	MT_UNLIT_NonTransparant_NonTesselated UMETA(DisplayName = "UNLIT Non-Transparant Non-Tesselated"),
	MT_PBR_Transparant_Tesselated UMETA(DisplayName = "PBR Transparant Tesselated"),
	MT_PBR_Transparant_NonTesselated UMETA(DisplayName = "PBR Transparant Non-Tesselated"),
	MT_PBR_NonTransparant_Tesselated UMETA(DisplayName = "PBR Non-Transparant Tesselated"),
	MT_PBR_NonTransparant_NonTesselated UMETA(DisplayName = "PBR Non-Transparant Non-Tesselated"),
	MT_MOBILE_Transparant UMETA(DisplayName = "Mobile Transparant"),
	MT_MOBILE_NonTransparant UMETA(DisplayName = "Mobile Non-Transparant")
};

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API UPA_StaticLibrary : public UObject
{
	GENERATED_BODY()
	
};
