#include "QuakeWadAssetFactory.h"

#include "FileHelpers.h"
#include "Assets/QuakeWadAsset.h"

UClass* FQWadAssetTypeAction::GetSupportedClass() const
{
	return UQuakeWadAsset::StaticClass();
}
 
FText FQWadAssetTypeAction::GetName() const
{
	return INVTEXT("Quake WAD");
}
 
FColor FQWadAssetTypeAction::GetTypeColor() const
{
	return FColor::Purple;
}
 
uint32 FQWadAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

UQuakeWadAssetFactory::UQuakeWadAssetFactory()
{
	Formats.Add("wad;Quake WAD FIle");
	SupportedClass = UQuakeWadAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject* UQuakeWadAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UQuakeWadAsset* newWadObj = NewObject<UQuakeWadAsset>(InParent, InClass, InName, Flags);
	newWadObj->LoadWadFromFile(Filename);
	UEditorLoadingAndSavingUtils::SavePackages({newWadObj->GetPackage()}, false);
	return newWadObj;
}