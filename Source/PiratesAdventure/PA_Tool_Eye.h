// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "PA_Tool_Base.h"
#include "PA_Tool_Eye.generated.h"

UENUM()
enum class EEyeToolAnimationState : uint8
{
	STATE_IDLE UMETA(DisplayName = "Idle"), //no animation
	STATE_OPEN UMETA(DisplayName = "Open"),
	STATE_CLOSE UMETA(DisplayName = "Close"),
	STATE_COUNTDOWN UMETA(DisplayName = "CountDown"),
	STATE_FINALCOUNTDOWN UMETA(DisplayName = "FinalCountdown")
};

UCLASS()
class PIRATESADVENTURE_API UPA_Tool_Eye : public UPA_Tool_Base
{
	GENERATED_BODY()

public:
	
	UPA_Tool_Eye(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void StartTool();
	virtual void StopTool();

	UFUNCTION(BlueprintImplementableEvent, Category = "Tool")
		void OnStartEyeAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Tool")
		void OnEndEyeAnimation();
};
