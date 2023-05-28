// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_LevelScriptActor_Base.h"
#include "PiratesAdventure.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_IngameMenu.h"
#include "PA_HudElements_Status.h"

APA_LevelScriptActor_Base::APA_LevelScriptActor_Base()
{
	//LevelName = NAME_None;
}

void APA_LevelScriptActor_Base::ReceiveBeginPlay()
{
	Super::ReceiveBeginPlay();
}

void APA_LevelScriptActor_Base::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		//if (!PC) return;
		//APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());
		//if (HUD 
		//	&& HUD->IngameMenuInstance 
		//	&& HUD->StatusIndicatorInstance)
		//{
		//	if (HUD->IngameMenuInstance->CurrentLoadType == ELoadBookUIType::BOOK_CloseIngameMenu
		//		|| HUD->IngameMenuInstance->CurrentLoadType == ELoadBookUIType::BOOK_CloseMainMenu)
		//	{
		//		HUD->StatusIndicatorInstance->StartCloseFadeOut();
		//	}
		//}
	}
}

void APA_LevelScriptActor_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APA_LevelScriptActor_Base::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}