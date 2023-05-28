// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "PA_Tool_Base.h"
#include "PA_Tool_Lantarn.generated.h"

USTRUCT(Blueprintable)
struct FLantarnLightSettings
{
	GENERATED_USTRUCT_BODY()

public:
	//the material of the fake volumetric effects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Light")
		UMaterialInterface* LantarnFireMaterial;

	//the strength of the main light. The main light causes hard realtime shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Light")
		float MainLightStrength = 30000.f;

	//the radius of the main light. The main light causes hard realtime shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Light")
		float MainLightRadius = 1000.f;

	//the color of the main light. The main light causes hard realtime shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Light")
		FColor MainLightColor;

	//the falloff of the main light. The main light causes hard realtime shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Light")
		float MainLightFallOff = 8.f;

	//the strength of the sub light. The sub light is only to soften the hardness of the light, and doesnt cast any shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sub Light")
		float SubLightStrength = 10.f;

	//the radius of the sub light. The sub light is only to soften the hardness of the light, and doesnt cast any shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sub Light")
		float SubLightRadius = 500.f;

	//the color of the sub light. The sub light is only to soften the hardness of the light, and doesnt cast any shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sub Light")
		FColor SubLightColor;

	//the falloff of the sub light. The sub light is only to soften the hardness of the light, and doesnt cast any shadows. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sub Light")
		float SubLightFallOff = 2.f;

	//the size of the light sphere for the lantern. Used for fake volumetric effects. Also doubles as range for enemies.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volumetric Sphere")
		float VolumetricSize = 500.f;

	//the material of the fake volumetric effects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volumetric Sphere")
		UMaterialInterface* VolumetricMaterial;
};

UCLASS()
class PIRATESADVENTURE_API UPA_Tool_Lantarn : public UPA_Tool_Base
{
	GENERATED_BODY()
	
public:

	UPA_Tool_Lantarn(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void StartTool() override;
	virtual void StopTool() override;

	virtual void InitializeTool() override;
	virtual void DeinitializeTool() override;
};
