#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include <qformats/map/map.h>

#include "QUnrealSettings.h"
#include "QuakeMapData.h"
#include "QuakeMapAsset.generated.h"

UCLASS()
class QUNREALED_API UQuakeMapAsset : public UObject, public IInterface_AssetUserData
{
	GENERATED_BODY()

public:
	UQuakeMapAsset();
	void LoadMapFromFile(FString fileName);

	UPROPERTY(EditAnywhere) bool bOverrideDefaultOptions = false;
	UPROPERTY(EditAnywhere, DisplayName="Import Options", meta = (EditCondition = "bOverrideDefaultOptions"))
	FQuakeMapAssetOptions Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bImportAsStaticMeshLib;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="Map Data")
	UQuakeMapData* MapData;
	
	UPROPERTY(VisibleAnywhere) TArray<UMaterialInstanceDynamic*> Materials;
	UPROPERTY() FString SourceQMapFile;
	
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;

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
	void Reset();
	UStaticMesh* ConvertEntityToModel(const qformats::map::SolidEntityPtr &ent, FVector3d &OutCenter, FVector3d& OutPivot);
	FString GetUniqueEntityName(const std::string &ClassName);
	static void MarkTexture(qformats::map::QMap *NativeMap, const FString &TextureName, qformats::map::Face::eFaceType t);
	void FillCacheFromTextures(qformats::map::QMap* NativeMap);
	qformats::textures::ITexture *onTextureRequest(std::string name);
	std::map<std::string, int> EntityClassCount;
	
	TMap<FString, TWeakObjectPtr<UTexture2D>> TextureCache;
	TMap<FString, TWeakObjectPtr<UMaterial>> MaterialOverrideCache;
	FAssetRegistryModule &AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	static fvec3 CalculateEntityPivot(const qformats::map::SolidEntityPtr &Entity, EQuakeEntityPivot Pivot);
};
