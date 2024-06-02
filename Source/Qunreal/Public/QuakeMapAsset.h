#pragma once

#include "CoreMinimal.h"
#include <qformats/map/map.h>

#include "QuakeMapAsset.generated.h"

UCLASS()
class QUNREAL_API UQuakeMapAsset : public UObject, public IInterface_AssetUserData
{
	GENERATED_BODY()

public:
	void LoadMapFromFile(FString fileName);
	int RegisterOnUpdatedCallback(std::function<void()>);
	void RemoveOnUpdatedCallback(int id);
public:
	UPROPERTY(VisibleAnywhere) UStaticMesh *WorldSpawnMesh;
	UPROPERTY(VisibleAnywhere) TArray<UStaticMesh*> EntityMeshes;
	UPROPERTY(VisibleAnywhere) FString SourceQMapFile;
	UPROPERTY(VisibleAnywhere) TArray<UMaterialInstanceDynamic*> Materials;
public:
	FString GetClassName(int idx) const;

#if WITH_EDITORONLY_DATA
	// Import data for this
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData* AssetImportData;

	// UObject interface
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual void Serialize(FArchive& Ar) override;
	// End of UObject interface
#endif

private:
	UStaticMesh* ConvertEntityToModel(const qformats::map::SolidEntity &ent);
	FString GetUniqueEntityName(qformats::map::QEntity *Ent);

	std::vector<std::pair<int,std::function<void()>>> onUpdatedCallbacks;
	int onUpdateCallbackIDSeed = 0;
	qformats::map::QMap* NativeMap;
	qformats::textures::ITexture *onTextureRequest(std::string name, UMaterial* BaseMaterial);
	std::map<std::string, int> EntityClassCount;
};
