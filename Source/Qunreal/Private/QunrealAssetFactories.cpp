#include "QunrealAssetFactories.h"
#include "QuakeMap.h"

UQuakeMapAssetFactory::UQuakeMapAssetFactory()
{
	Formats.Add("map;Quake Map FIle");
	SupportedClass = UQMap::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject* UQuakeMapAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UQMap* newMapObj = NewObject<UQMap>(InParent, InClass, InName, Flags);
	newMapObj->SourceQMapFile = Filename;
	return newMapObj;
}

UWadAssetFactory::UWadAssetFactory()
{
	Formats.Add("wad;Quake WAD FIle");
	SupportedClass = UQMap::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject* UWadAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UQMap* newMapObj = NewObject<UQMap>(InParent, InClass, InName, Flags);
	newMapObj->SourceQMapFile = Filename;
	return newMapObj;
}
