#include "QUnrealED.h"

#include "ISettingsModule.h"
#include "QuakeWadAssetFactory.h"
#include "QUnrealSettings.h"
#include "Runtime/AssetRegistry/Private/AssetRegistryImpl.h"
#
#define LOCTEXT_NAMESPACE "FQUnrealEDModule"

void FQUnrealEDModule::StartupModule()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"QUnrealEditorSettings",
			LOCTEXT("RuntimeSettingsName", "QUnreal Settings"),
			LOCTEXT("RuntimeSettingsDescription", "Configure QUnreal defaults"),
			GetMutableDefault<UQUnrealSettings>()
			);

		auto Options = GetMutableDefault<UQUnrealSettings>();
		
		if (!Options->bIsInitialized) {
			Options->MapAssetOptions.BaseMaterial = static_cast<UMaterial*>(
				StaticLoadObject(UMaterial::StaticClass(),
					nullptr,
					TEXT("/QUnreal/Materials/M_WadMaster"),
					nullptr,
					LOAD_None,
					nullptr));
			Options->MapAssetOptions.EntityClassOverrides = static_cast<UQEntityClassesData*>(
				StaticLoadObject(UQEntityClassesData::StaticClass(),
					nullptr,
					TEXT("/QUnreal/DefaultEntityClasses"),
					nullptr,
					LOAD_None,
					nullptr));
			Options->bIsInitialized = true;
		}
		
	}
	
	FQMapAssetTypeActions = MakeShared<FQMapAssetTypeAction>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(FQMapAssetTypeActions.ToSharedRef());

	FQWadAssetTypeActions = MakeShared<FQWadAssetTypeAction>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(FQWadAssetTypeActions.ToSharedRef());
}

void FQUnrealEDModule::ShutdownModule()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "QUnrealEditorSettings");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FQUnrealEDModule, QUnrealED)
