// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_ISMA_InteractiveBook.h"
#include "PiratesAdventure.h"
#include "PA_PlayerHUD.h"
#include "PA_HudElements_IngameMenu.h"

APA_ISMA_InteractiveBook::APA_ISMA_InteractiveBook()
{
	MenuItemsPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MenuItemsPlane"));
	MenuItemsPlane->SetupAttachment(GetRootComponent());

	MenuContentsLevel1Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MenuContentsLevel1Plane"));
	MenuContentsLevel1Plane->SetupAttachment(GetRootComponent());

	MenuContentsLevel2Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MenuContentsLevel2Plane"));
	MenuContentsLevel2Plane->SetupAttachment(GetRootComponent());

	MenuContentsLevel3Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MenuContentsLevel3Plane"));
	MenuContentsLevel3Plane->SetupAttachment(GetRootComponent());
}

void APA_ISMA_InteractiveBook::BeginPlay()
{
	if (GetWorld())
	{
		APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!PC) return;
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());

		/*if (HUD && HUD->IngameMenuInstance)
		{


			IngameMenuUI = HUD->IngameMenuInstance;
			IngameMenuUI->BookInstance = this;

			switch (IngameMenuUI->CurrentLoadType)
			{
			case ELoadBookUIType::BOOK_OpenMainMenu:
				OpenMainMenu();
				break;

			case ELoadBookUIType::BOOK_OpenIngameMenu:
				OpenIngameMenu();
				break;
			}
		}*/
	}

	Super::BeginPlay();
}

void APA_ISMA_InteractiveBook::OpenIngameMenu()
{
	ReceiveOpenIngameMenu();
}

void APA_ISMA_InteractiveBook::CloseIngameMenu()
{
	ReceiveCloseIngameMenu();
}

void APA_ISMA_InteractiveBook::OpenMainMenu()
{
	ReceiveOpenMainMenu();
}

void APA_ISMA_InteractiveBook::CloseMainMenu()
{
	ReceiveCloseMainMenu();
}

void APA_ISMA_InteractiveBook::OpenSubLevel(int32 SubLevelIndex)
{
	ReceiveOpenSubLevel(SubLevelIndex);
}

UStaticMeshComponent* APA_ISMA_InteractiveBook::GetSubLevelMesh(int32 SubLevelIndex)
{
	switch (SubLevelIndex)
	{
	case 0: return MenuContentsLevel1Plane;
	case 1: return MenuContentsLevel2Plane;
	case 2: return MenuContentsLevel3Plane;
	}

	return NULL;
}

void APA_ISMA_InteractiveBook::FinalizeCloseMenu()
{
	if (GetWorld())
	{
		APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!PC) return;
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());

		//if (HUD && HUD->StatusIndicatorInstance)
		//{
		//	HUD->StatusIndicatorInstance->StartCloseFadeIn();
		//}
	}
}

float APA_ISMA_InteractiveBook::GetFullscreenTransitionDuration()
{
	//TODO: Update this once we have an option of turning off the Fade Transitions
	if (GetWorld())
	{
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

		//if (HUD && HUD->StatusIndicatorInstance)
		//{
		//	return HUD->StatusIndicatorInstance->FadeDuration;
		//}
	}

	return 0.f;
}

void APA_ISMA_InteractiveBook::SetIngameUIElementsVisible()
{
	if (GetWorld())
	{
		APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!PC) return;
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());

		//if (HUD && HUD->IngameMenuInstance)
		//{
		//	HUD->IngameMenuInstance->SetIngameUIVisibility(ESlateVisibility::Visible);
		//}
	}
}

void APA_ISMA_InteractiveBook::SetIngameUIElementsHidden()
{
	if (GetWorld())
	{
		APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!PC) return;
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());

		//if (HUD && HUD->IngameMenuInstance)
		//{
		//	HUD->IngameMenuInstance->SetIngameUIVisibility(ESlateVisibility::Hidden);
		//}
	}
}

void APA_ISMA_InteractiveBook::StartAnimatingSublevelMask()
{
	if (GetWorld())
	{
		APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!PC) return;
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());

		//if (HUD && HUD->IngameMenuInstance)
		//{
		//	HUD->IngameMenuInstance->TurnOnMaskAnimation();
		//}
	}
}

void APA_ISMA_InteractiveBook::StopAnimatingSublevelMask()
{
	if (GetWorld())
	{
		APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!PC) return;
		APA_PlayerHUD* HUD = Cast<APA_PlayerHUD>(PC->GetHUD());

		//if (HUD && HUD->IngameMenuInstance)
		//{
		//	HUD->IngameMenuInstance->TurnOffMaskAnimation();
		//}
	}
}