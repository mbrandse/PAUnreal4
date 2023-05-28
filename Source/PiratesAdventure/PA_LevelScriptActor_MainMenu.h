// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "PA_LevelScriptActor_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_LevelScriptActor_MainMenu : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

	APA_LevelScriptActor_MainMenu();

protected:

	virtual void BeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

};