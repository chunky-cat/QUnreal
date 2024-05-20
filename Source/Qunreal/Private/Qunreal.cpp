// Copyright Epic Games, Inc. All Rights Reserved.

#include "Qunreal.h"

#define LOCTEXT_NAMESPACE "FQunrealModule"

void FQunrealModule::StartupModule()
{
	FQMapAssetTypeActions = MakeShared<FQMapAssetTypeAction>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(FQMapAssetTypeActions.ToSharedRef());
}

void FQunrealModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("Qunreal")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(FQMapAssetTypeActions.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQunrealModule, Qunreal)