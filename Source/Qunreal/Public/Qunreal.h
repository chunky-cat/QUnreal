// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "QuakeMap.h"
#include "Modules/ModuleManager.h"

class FQunrealModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	TSharedPtr<FQMapAssetTypeAction> FQMapAssetTypeActions;

};
