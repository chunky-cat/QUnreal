#pragma once

#include "CoreMinimal.h"
#include "QunrealAssetFactories.generated.h"

UCLASS()
class UQuakeMapAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UQuakeMapAssetFactory();
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};

UCLASS()
class UWadAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UWadAssetFactory();
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};