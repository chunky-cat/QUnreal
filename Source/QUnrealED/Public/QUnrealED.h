#pragma once

#include "CoreMinimal.h"
#include "QuakeMapAssetFactory.h"

#include "Modules/ModuleManager.h"


class FQUnrealEDModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual bool SupportsDynamicReloading() override
    {
        return true;
    }
private:
    TSharedPtr<FQMapAssetTypeAction> FQMapAssetTypeActions;
};
