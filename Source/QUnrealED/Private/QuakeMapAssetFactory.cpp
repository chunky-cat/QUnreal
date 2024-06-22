#include "QuakeMapAssetFactory.h"

#include "FileHelpers.h"
#include "QUnrealSettings.h"
#include "Assets/QuakeMapAsset.h"
#include "EditorFramework/AssetImportData.h"

UClass* FQMapAssetTypeAction::GetSupportedClass() const
{
	return UQuakeMapAsset::StaticClass();
}
 
FText FQMapAssetTypeAction::GetName() const
{
	return INVTEXT("Quake Map");
}
 
FColor FQMapAssetTypeAction::GetTypeColor() const
{
	return FColor::Red;
}
 
uint32 FQMapAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

bool FQMapAssetTypeAction::IsImportedAsset() const
{
	return true;
}

void FQMapAssetTypeAction::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets,
	TArray<FString>& OutSourceFilePaths) const
{
		FAssetTypeActions_Base::GetResolvedSourceFilePaths(TypeAssets, OutSourceFilePaths);
    
    	for (auto& Asset : TypeAssets)
    	{
    		const auto QuakeMap = CastChecked<UQuakeMapAsset>(Asset);
    		if (QuakeMap->AssetImportData)
    		{
    			QuakeMap->AssetImportData->ExtractFilenames(OutSourceFilePaths);
    		}
    	}
}

UQuakeMapAssetFactory::UQuakeMapAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Formats.Add("map;Quake Map FIle");
	SupportedClass = UQuakeMapAsset::StaticClass();
	
	bCreateNew = false;
	bEditorImport = true;
	bAutomatedReimport = true;
	bEditAfterNew = true;
	ImportPriority = 1;
}

UObject* UQuakeMapAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UQuakeMapAsset* newMapObj = NewObject<UQuakeMapAsset>(InParent, InClass, InName, Flags);
	newMapObj->SourceQMapFile = Filename;
	newMapObj->LoadMapFromFile(Filename);
	newMapObj->PostEditChange();

	newMapObj->AssetImportData->Update(Filename);
	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, newMapObj);
	return newMapObj;
}

bool UQuakeMapAssetFactory::ConfigureProperties()
{
	return Super::ConfigureProperties();
}

bool UQuakeMapAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	return true;
}

void UQuakeMapAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	auto ReimportMap = static_cast<UQuakeMapAsset*>(Obj);
	if (ReimportMap != nullptr)
	{
		ReimportMap->SourceQMapFile = NewReimportPaths[0];
		ReimportMap->Modify();
		ReimportMap->AssetImportData->Update(NewReimportPaths[0]);
	}
}

int32 UQuakeMapAssetFactory::GetPriority() const
{
	return ImportPriority;
}

EReimportResult::Type UQuakeMapAssetFactory::Reimport(UObject* Obj)
{
	auto ReimportMap = static_cast<UQuakeMapAsset*>(Obj);
	//ReimportMap->LoadMapFromFile(ReimportMap->SourceQMapFile);
	//GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetReimport(ReimportMap);
	//const FString Filename = ReimportMap->AssetImportData->GetFirstFilename();
	//const FString FileExtension = FPaths::GetExtension(Filename);

	ReimportMap->LoadMapFromFile(ReimportMap->SourceQMapFile);
	// Mark the package dirty after the successful import
	ReimportMap->MarkPackageDirty();
	ReimportMap->QuakeMapUpdated.Broadcast();
	return EReimportResult::Succeeded;
}
