#pragma once


#include <CoreMinimal.h>
#include "AssetTypeActions_Base.h"
#include "QuakeWadAssetFactory.generated.h"

class QUNREALED_API FQWadAssetTypeAction final : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
};

UCLASS()
class QUNREALED_API UQuakeWadAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UQuakeWadAssetFactory();
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

};
