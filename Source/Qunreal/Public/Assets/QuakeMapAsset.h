#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include <qformats/map/map.h>

#include "Entities/QEntityClassesData.h"
#include "QuakeMapAsset.generated.h"

UENUM()
enum EEntityType
{
	EntityType_Point = 0,
	EntityType_Solid = 1,
};

USTRUCT()
struct FEntity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere) TEnumAsByte<EEntityType> Type;
	UPROPERTY(EditAnywhere) FString UniqueClassName;
	UPROPERTY(VisibleAnywhere) FString ClassName;
	UPROPERTY(VisibleAnywhere) FVector3d Origin;
	UPROPERTY(VisibleAnywhere) float Angle;
	UPROPERTY(VisibleAnywhere) TMap<FString,FString> Properties;
	TSubclassOf<AQEntityActor> ClassTemplate;
};

USTRUCT()
struct FSolidEntity : public FEntity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere) UStaticMesh *Mesh;
	UPROPERTY(VisibleAnywhere) FVector3d Center;
};


UCLASS()
class QUNREAL_API UQuakeMapAsset : public UObject, public IInterface_AssetUserData
{
	GENERATED_BODY()

public:
	void LoadMapFromFile(FString fileName);
public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UQEntityClassesData> EntityClassOverrides;

	UPROPERTY(EditAnywhere)
	UMaterial *BaseMaterial;
	
	UPROPERTY(EditAnywhere)
	float InverseScale = 1;

	UPROPERTY(EditAnywhere)
	float MaxLightmapSize = 512;

	UPROPERTY(EditAnywhere)
	float LightMapDivider = 2600;
	
	UPROPERTY(EditAnywhere)
	FName TextureFolder = "";

	UPROPERTY(EditAnywhere)
	FName MaterialOverrideFolder = "";

	UPROPERTY(EditAnywhere)
	FString SkipTexture = "skip";

	UPROPERTY(EditAnywhere)
	FString ClipTexture = "clip";

	UPROPERTY(EditAnywhere)
	FString SkyTexture = "sky";
	
	
	UPROPERTY(EditAnywhere)
	bool bImportLights = false;
	
	UPROPERTY(VisibleAnywhere) UStaticMesh *WorldSpawnMesh;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "UniqueClassName"))
	TArray<FSolidEntity> SolidEntities;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "UniqueClassName"))
	TArray<FEntity> PointEntities;
	
	UPROPERTY(VisibleAnywhere) TArray<UMaterialInstanceDynamic*> Materials;
	UPROPERTY() FString SourceQMapFile;
	
	FSimpleMulticastDelegate QuakeMapUpdated;
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
public:

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
	UStaticMesh* ConvertEntityToModel(const qformats::map::SolidEntityPtr &ent, FVector3d &OutCenter);
	FString GetUniqueEntityName(qformats::map::BaseEntity *Ent);
	static void MarkTexture(qformats::map::QMap *NativeMap, const FString &TextureName, qformats::map::Face::eFaceType t);
	void FillCacheFromTextures(qformats::map::QMap* NativeMap);
	qformats::textures::ITexture *onTextureRequest(std::string name);
	std::map<std::string, int> EntityClassCount;
	
	TMap<FString, TWeakObjectPtr<UTexture2D>> TextureCache;
	TMap<FString, TWeakObjectPtr<UMaterial>> MaterialOverrideCache;
	FAssetRegistryModule &AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));;
};
