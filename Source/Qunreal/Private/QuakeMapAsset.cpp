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
	metatex->SetWidth(texture->GetImportedSize().X);
	metatex->SetHeight(texture->GetImportedSize().Y);
	metatex->SetData(MaterialInstance);
	Materials.Push(MaterialInstance);
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

	NativeMap->ExcludeTextureSurface("SKY1");
	NativeMap->ExcludeTextureSurface("CLIP");
	NativeMap->ExcludeTextureSurface("SKIP");
	
	NativeMap->GenerateGeometry();
	UMaterial* BaseMaterial = static_cast<UMaterial*>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/QUnreal/Materials/M_WadMaster"), nullptr,
	                                                                   LOAD_None, nullptr));

	FWadManager::GetInstance()->Refresh();
	NativeMap->LoadTextures([this,BaseMaterial](std::string name) -> qformats::textures::ITexture *
								 { return this->onTextureRequest(name,BaseMaterial); });

	/*
	for (auto Textures = NativeMap->GetTextures(); const auto Texture : Textures)
	{
		auto DynMat = static_cast<qformats::textures::Texture<UMaterialInstanceDynamic*>*>(Texture);
		Materials.Push(DynMat->Data());	
	}
	*/
	if (NativeMap->GetSolidEntities().size() == 0)
		return;
	
	
	WorldSpawnMesh = ConvertEntityToModel(NativeMap->GetSolidEntities()[0]);
	EntityMeshes.Empty();
	EntityClassCount.clear();
	
	for (int i = 1; i < NativeMap->GetSolidEntities().size()-1;i++)
	{
		EntityMeshes.Emplace(ConvertEntityToModel(NativeMap->GetSolidEntities()[i]));
	}
}

FString UQuakeMapAsset::GetClassName(int idx) const
{
	idx++;
	if ( idx >= NativeMap->GetSolidEntities().size() || idx < 0) return "none";
	if (NativeMap->GetSolidEntities()[idx].entityRef == nullptr) return "none";
	auto ClassName = NativeMap->GetSolidEntities()[idx].entityRef->classname.c_str();
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

	FVector3f EmptyVector = FVector3f(0, 0, 0);
	FColor WhiteVertex = FColor(255, 255, 255, 255);
	FRawMesh rawMesh;

	
	int offset_idx = 0, material_idx = 0;
	TMap<int32_t, int32_t> MatIDMap;
	
	for (const auto &b : Entity.geoBrushes)
	{
		for (const auto p : b.polygons)
		{
			if (p->vertices.size() == 0 || p->noDraw) continue;
			
			for (int i = p->vertices.size()-1; i >= 0 ; i--)
			{
				const auto &pt = p->vertices[i];
				rawMesh.VertexPositions.Add(FVector3f(-pt.point[0],pt.point[1],pt.point[2]));
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

				rawMesh.WedgeTexCoords[0].Add(FVector2f(pt.uv[0], pt.uv[1]));
				rawMesh.WedgeTexCoords[0].Add(FVector2f(pt2.uv[0], pt2.uv[1]));
				rawMesh.WedgeTexCoords[0].Add(FVector2f(pt3.uv[0], pt3.uv[1]));

				rawMesh.WedgeTangentX.Add(EmptyVector);
				rawMesh.WedgeTangentX.Add(EmptyVector);
				rawMesh.WedgeTangentX.Add(EmptyVector);

				rawMesh.WedgeTangentY.Add(EmptyVector);
				rawMesh.WedgeTangentY.Add(EmptyVector);
				rawMesh.WedgeTangentY.Add(EmptyVector);

				rawMesh.WedgeTangentZ.Add(FVector3f(pt.normal[0], pt.normal[1],pt.normal[2]));
				rawMesh.WedgeTangentZ.Add(FVector3f(pt2.normal[0], pt2.normal[1],pt2.normal[2]));
				rawMesh.WedgeTangentZ.Add(FVector3f(pt3.normal[0], pt3.normal[1],pt3.normal[2]));

				if (!MatIDMap.Contains(p->faceRef.textureID))
				{
					Mesh->AddMaterial(Materials[p->faceRef.textureID]);
					MatIDMap.Add(p->faceRef.textureID,material_idx++);
				}
				rawMesh.FaceMaterialIndices.Add(MatIDMap[p->faceRef.textureID]);
				rawMesh.FaceSmoothingMasks.Add(0);
			}
			
			offset_idx += p->vertices.size();
		}
	}

	
	rawMesh.CompactMaterialIndices();
	
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
	Mesh->bCustomizedCollision = false;
	
	
	Mesh->Build(false);
	Mesh->ComplexCollisionMesh = Mesh;
	

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

