// Global interface that will take care of character specific functions that need to be present over all
// custom character classes.

#pragma once

#include "PA_StaticLibrary.h"
#include "PA_Interface_GlobalCharacter.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI)
class UPA_Interface_GlobalCharacter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IPA_Interface_GlobalCharacter
{
	GENERATED_IINTERFACE_BODY()

	virtual void UpdateScalarMaterialParameterSet(UMaterialParameterCollection* Collection, FName ParameterName, float Value);

public: 

	//Null Element is passed by the volume to indicate what element can ignore the damage from this volume
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
		void OnProcessTimedDamage(const class UDamageType* DamageType, const float& Damage, EPlayerWeaponType NulledElement);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
		bool CanTakeDamage();
};
