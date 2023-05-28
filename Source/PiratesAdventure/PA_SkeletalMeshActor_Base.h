// The base skeletal mesh actor class for Pirates Adventure. Provides additional functionality for animation notifies and the likes.

#pragma once

#include "Animation/SkeletalMeshActor.h"
//#include "PA_GlobalCharacterInterface.h"
#include "PA_SkeletalMeshActor_Base.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_SkeletalMeshActor_Base : public ASkeletalMeshActor//, public IPA_Interface_GlobalCharacter
{
public:
	GENERATED_BODY()
	
	////moved to interface
	//virtual void UpdateScalarMaterialParameterSet(UMaterialParameterCollection* Collection, FName ParameterName, float Value);	

	////for testing
	//virtual FString ToString();
};
