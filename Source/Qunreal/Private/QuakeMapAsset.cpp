#include "QuakeMapAsset.h"
#include "QuakeWadAsset.h"
#include "RawMesh.h"
#include "UObject/SavePackage.h"
#include "AssetHelper.h"
#include "EditorFramework/AssetImportData.h"

qformats::textures::ITexture *UQuakeMapAsset::onTextureRequest(std::string name, UMaterial* BaseMaterial)
{
	auto texture = FWadManager::GetInstance()->FindTexture(name);
	if (texture == nullptr)
	{
		return nullptr;
	}
	auto MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	MaterialInstance->SetTextureParameterValue("BaseTexture", texture);
	auto metatex = new qformats::textures::Texture<UMaterialInstanceDynamic*>();
	metatex->SetWidth(texture->GetSizeX());
	metatex->SetHeight(texture->GetSizeY());
	metatex->SetData(MaterialInstance);
	Materials.Emplace(MaterialInstance);
	return metatex;
}

void UQuakeMapAsset::LoadMapFromFile(FString fileName)
{
	if (NativeMap != nullptr) {
		delete NativeMap;
		NativeMap = nullptr;
	}

	NativeMap = new qformats::map::QMap();
	NativeMap->LoadFile(std::string(TCHAR_TO_UTF8(*fileName)));
	NativeMap->GenerateGeometry();
	UMaterial* BaseMaterial = static_cast<UMaterial*>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/QUnreal/Materials/M_WadMaster"), nullptr,
	                                                                   LOAD_None, nullptr));

	FWadManager::GetInstance()->Refresh();
	NativeMap->LoadTextures([this,BaseMaterial](std::string name) -> qformats::textures::ITexture *
								 { return this->onTextureRequest(name,BaseMaterial); });
	
	if (NativeMap->solidEntities.size() == 0)
		return;
	
	
	WorldSpawnMesh = ConvertEntityToModel(NativeMap->solidEntities[0]);
	EntityMeshes.Empty();
	EntityClassCount.clear();
	
	for (int i = 1; i < NativeMap->solidEntities.size()-1;i++)
	{
		EntityMeshes.Emplace(ConvertEntityToModel(NativeMap->solidEntities[i]));
	}
}

void UQuakeMapAsset::RemoveOnUpdatedCallback(int id)
{
	for (int i = 0; i < onUpdatedCallbacks.size(); i++)
	{
		if (onUpdatedCallbacks[i].first == id)
		{
			onUpdatedCallbacks.erase(onUpdatedCallbacks.begin()+i);
		}
	}
}

int UQuakeMapAsset::RegisterOnUpdatedCallback(std::function<void()> cb)
{
	int id = onUpdateCallbackIDSeed++;
	onUpdatedCallbacks.push_back({id,cb});
	return onUpdateCallbackIDSeed++;
}

FString UQuakeMapAsset::GetClassName(int idx) const
{
	idx++;
	if ( idx >= NativeMap->solidEntities.size() || idx < 0) return "none";
	if (NativeMap->solidEntities[idx].entityRef == nullptr) return "none";
	auto ClassName = NativeMap->solidEntities[idx].entityRef->classname.c_str();
	return FString(ClassName);
}

void UQuakeMapAsset::PostInitProperties()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}

	UObject::PostInitProperties();
}

void UQuakeMapAsset::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	if (AssetImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
	}

	UObject::GetAssetRegistryTags(OutTags);
}

void UQuakeMapAsset::Serialize(FArchive& Ar)
{
	UObject::Serialize(Ar);
	if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON && !AssetImportData)
	{
		// AssetImportData should always be valid
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
}

UStaticMesh* UQuakeMapAsset::ConvertEntityToModel(const qformats::map::SolidEntity& Entity)
{
	auto MapName = FPaths::GetBaseFilename(SourceQMapFile);
	FString MeshName = GetUniqueEntityName(Entity.entityRef);
	FString PackagePath = this->GetPackage()->GetPathName();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	UPackage* Pkg = CreatePackage(*PackagePath);
	UStaticMesh *Mesh = NewObject<UStaticMesh>(Pkg, *MeshName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	FAssetRegistryModule::AssetCreated(Mesh);
	Pkg->FullyLoad();

	UMaterial* BaseMaterial = (UMaterial*)StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/QUnreal/Materials/M_WadMaster"), NULL, LOAD_None, NULL);
	FVector3f EmptyVector = FVector3f(0, 0, 0);
	FColor WhiteVertex = FColor(255, 255, 255, 255);
	FRawMesh rawMesh;

	for (auto m : Materials)
	{
		Mesh->AddMaterial(m);
	}
	
	int offset_idx = 0;
	
	for (const auto &b : Entity.geoBrushes)
	{
		for (const auto p : b.polygons)
		{
			//
			for (int i = p->vertices.size()-1; i >= 0 ; i--)
			{
				const auto &pt = p->vertices[i];
				rawMesh.VertexPositions.Add(FVector3f(-pt.point.x,pt.point.y,pt.point.z));
			}

			for (int i = 0; i < p->indices.size()-2; i+= 3)
			{
				auto endv = p->vertices.size()-1;
				const auto &pt = p->vertices[endv-p->indices[i]];
				const auto &pt2 = p->vertices[endv-p->indices[i+1]];
				const auto &pt3 = p->vertices[endv-p->indices[i+2]];

				rawMesh.WedgeIndices.Add(p->indices[i]+offset_idx);
				rawMesh.WedgeIndices.Add(p->indices[i+1]+offset_idx);
				rawMesh.WedgeIndices.Add(p->indices[i+2]+offset_idx);
				
				rawMesh.WedgeColors.Add(WhiteVertex);
				rawMesh.WedgeColors.Add(WhiteVertex);
				rawMesh.WedgeColors.Add(WhiteVertex);

				rawMesh.WedgeTexCoords[0].Add(FVector2f(pt.uv.x, pt.uv.y));
				rawMesh.WedgeTexCoords[0].Add(FVector2f(pt2.uv.x, pt2.uv.y));
				rawMesh.WedgeTexCoords[0].Add(FVector2f(pt3.uv.x, pt3.uv.y));

				rawMesh.WedgeTangentX.Add(EmptyVector);
				rawMesh.WedgeTangentX.Add(EmptyVector);
				rawMesh.WedgeTangentX.Add(EmptyVector);

				rawMesh.WedgeTangentY.Add(EmptyVector);
				rawMesh.WedgeTangentY.Add(EmptyVector);
				rawMesh.WedgeTangentY.Add(EmptyVector);

				rawMesh.WedgeTangentZ.Add(FVector3f(pt.normal.x, pt.normal.y,pt.normal.z));
				rawMesh.WedgeTangentZ.Add(FVector3f(pt2.normal.x, pt2.normal.y,pt2.normal.z));
				rawMesh.WedgeTangentZ.Add(FVector3f(pt3.normal.x, pt3.normal.y,pt3.normal.z));
				
				rawMesh.FaceMaterialIndices.Add(p->faceRef.textureID);
				rawMesh.FaceSmoothingMasks.Add(0);
			}
			offset_idx += p->vertices.size();
		}
	}
	
	Mesh->AddSourceModel();
	auto &SrcModel = Mesh->GetSourceModel(0);
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
	SrcModel.BuildSettings.bRecomputeNormals = true;
	SrcModel.BuildSettings.bRecomputeTangents = true;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = true;
	SrcModel.BuildSettings.bGenerateLightmapUVs = true;
	SrcModel.BuildSettings.MinLightmapResolution = 128;
	
	SrcModel.RawMeshBulkData->SaveRawMesh(rawMesh);
	Mesh->PostEditChange();
	Mesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
	Mesh->SetLightingGuid();
	Mesh->Build(false);

	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Public | RF_Standalone;
	Args.SaveFlags = SAVE_NoError;

	return Mesh;
}

FString UQuakeMapAsset::GetUniqueEntityName(qformats::map::QEntity *Ent)
{
	auto Name = FString(Ent->classname.c_str())+"_";
	if(!EntityClassCount.contains(Ent->classname))
	{
		EntityClassCount[Ent->classname] = 0;
		Name.AppendInt(0);
		return Name;
	}
	Name.AppendInt(EntityClassCount[Ent->classname] += 1);
	return Name;
}

