// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "PA_LevelScriptActor_Base.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_LevelScriptActor_Base : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

	APA_LevelScriptActor_Base();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pirates Adventure Basic Level Data")
	//	FName LevelName;

	//OVERRIDES
protected:

	void ReceiveBeginPlay();

	virtual void BeginPlay() override;

	//Called when removed from world. Likely necessary when transition to the main menu.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//ON REMOVE FROM WORLD - USE INSTEAD OF BEGIN DESTROY
	/** Called when this actor is in a level which is being removed from the world (e.g. my level is getting UWorld::RemoveFromWorld called on it) */
	/*
	virtual void OnRemoveFromWorld() override; //look into this function.
	*/

protected:

	virtual void Tick(float DeltaSeconds) override;
	
};
