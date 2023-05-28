//#include "PiratesAdventureEditorModule.h"
#include "PiratesAdventureEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "InventoryItemDetailCustomization.h"
#include "WaterDataDetailCustomization.h"
#include "PropertyEditorModule.h"

IMPLEMENT_GAME_MODULE(FPiratesAdventureEditorModule, PiratesAdventureEditor);

DEFINE_LOG_CATEGORY(PAEditor)

#define LOCTEXT_NAMESPACE "PiratesAdventureEditorModule"

void FPiratesAdventureEditorModule::StartupModule()
{
	UE_LOG(PAEditor, Warning, TEXT("PiratesAdventureEditor: Log Started"));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom detail views
	PropertyModule.RegisterCustomClassLayout("PA_Asset_Item", FOnGetDetailCustomizationInstance::CreateStatic(&FInventoryItemDetailCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("PA_Asset_Water", FOnGetDetailCustomizationInstance::CreateStatic(&FWaterDataDetailCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FPiratesAdventureEditorModule::ShutdownModule()
{
	UE_LOG(PAEditor, Warning, TEXT("PiratesAdventureEditor: Log Ended"));

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout("PA_Asset_Item");
		PropertyModule.UnregisterCustomClassLayout("PA_Asset_Water");
	}
}

#undef LOCTEXT_NAMESPACE