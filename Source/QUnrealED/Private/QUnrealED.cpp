#include "QUnrealED.h"

#include "Runtime/AssetRegistry/Private/AssetRegistryImpl.h"
#
#define LOCTEXT_NAMESPACE "FQUnrealEDModule"

void FQUnrealEDModule::StartupModule()
{
	FQMapAssetTypeActions = MakeShared<FQMapAssetTypeAction>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(FQMapAssetTypeActions.ToSharedRef());
	
}

void FQUnrealEDModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FQUnrealEDModule, QUnrealED)