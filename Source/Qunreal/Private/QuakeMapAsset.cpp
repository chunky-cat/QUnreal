#include "QuakeMapAsset.h"
#include "QuakeWadAsset.h"
#include "RawMesh.h"
#include "UObject/SavePackage.h"
#include "AssetHelper.h"
#include "EditorFramework/AssetImportData.h"

qformats::textures::ITexture *UQuakeMapAsset::onTextureRequest(std::string name, UMaterial* BaseMaterial)
{
	auto texture = FWadManager::GetInstance()->FindTexture(name);
	auto MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	auto metatex = new qformats::textures::Texture<TWeakObjectPtr<UMaterialInstanceDynamic>>();
	metatex->SetWidth(64);
	metatex->SetHeight(64);
	if (texture != nullptr)
	{
		MaterialInstance->SetTextureParameterValue("BaseTexture", texture);
		metatex->SetWidth(texture->GetImportedSize().X);
		metatex->SetHeight(texture->GetImportedSize().Y);
	}
	auto wp = MakeWeakObjectPtr<UMaterialInstanceDynamic>(MaterialInstance);
	metatex->SetData(wp);
	Materials.Push(MaterialInstance);
	return metatex;
}

void UQuakeMapAsset::LoadMapFromFile(FString fileName)
{

	auto NativeMap = new qformats::map::QMap();
	NativeMap->LoadFile(std::string(TCHAR_TO_UTF8(*fileName)));

	NativeMap->ExcludeTextureSurface("SKY1");
	NativeMap->ExcludeTextureSurface("CLIP");
	NativeMap->ExcludeTextureSurface("SKIP");
	
	NativeMap->GenerateGeometry();
	UMaterial* BaseMaterial = static_cast<UMaterial*>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/QUnreal/Materials/M_WadMaster"), nullptr,
	                                                                   LOAD_None, nullptr));

	Materials.Empty();
	EntityClassNames.Empty();
	FWadManager::GetInstance()->Refresh();
	NativeMap->LoadTextures([this,BaseMaterial](std::string name) -> qformats::textures::ITexture *
								 { return this->onTextureRequest(name,BaseMaterial); });
	
	if (NativeMap->GetSolidEntities().size() == 0)
		return;


	EntityClassNames.Add(FString("worldspawn"));
	WorldSpawnMesh = ConvertEntityToModel(NativeMap->GetSolidEntities()[0], 512);
	EntityMeshes.Empty();
	EntityClassCount.clear();
	
	for (int i = 1; i < NativeMap->GetSolidEntities().size()-1;i++)
	{
		auto ClassName = NativeMap->GetSolidEntities()[i]->ClassName().c_str();
		EntityClassNames.Add(FString(ClassName));
		EntityMeshes.Emplace(ConvertEntityToModel(NativeMap->GetSolidEntities()[i],128));
	}

	for (auto it : NativeMap->GetTextures())
	{
		auto t = static_cast<qformats::textures::Texture<TWeakObjectPtr<UMaterialInstanceDynamic>>*>(it);
		t->SetData(nullptr);
		delete t;
	}
	
	delete NativeMap;
}

FString UQuakeMapAsset::GetClassName(int idx) const
{
	idx++;
	if ( idx >= EntityClassNames.Num() || idx < 0) return "none";
	return FString(EntityClassNames[idx]);
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
}

UStaticMesh* UQuakeMapAsset::ConvertEntityToModel(const qformats::map::SolidEntityPtr& Entity, int LMSize=128)
{
	auto MapName = FPaths::GetBaseFilename(SourceQMapFile);
	FString MeshName = GetUniqueEntityName(Entity.get());
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
	
	for (const auto &b : Entity.get()->GetClippedBrushes())
	{
		for (const auto p : b.faces)
		{
			if (p->vertices.size() < 3 || p->noDraw) continue;
			
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

				auto TexID = p->TextureID();
				if (!MatIDMap.Contains(TexID))
				{
					if (TexID < Materials.Num())
					{
						Mesh->AddMaterial(Materials[TexID]);
						MatIDMap.Add(TexID,material_idx++);
					}
				}
				rawMesh.FaceMaterialIndices.Add(MatIDMap[TexID]);
				rawMesh.FaceSmoothingMasks.Add(0);
			}
			
			offset_idx += p->vertices.size();
		}
	}

	
	rawMesh.CompactMaterialIndices();
	
	Mesh->AddSourceModel();
	auto &SrcModel = Mesh->GetSourceModel(0);
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = true;
	SrcModel.BuildSettings.bRecomputeNormals = true;
	SrcModel.BuildSettings.bRecomputeTangents = true;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = true;
	SrcModel.BuildSettings.bGenerateLightmapUVs = true;
	SrcModel.BuildSettings.MinLightmapResolution = LMSize;
	SrcModel.BuildSettings.DstLightmapIndex = 0;
	SrcModel.BuildSettings.DstLightmapIndex = 1;
	SrcModel.BuildSettings.bComputeWeightedNormals = true;
	
	SrcModel.ReductionSettings.bKeepSymmetry = true;
	SrcModel.ReductionSettings.bCullOccluded = true;
	SrcModel.RawMeshBulkData->SaveRawMesh(rawMesh);
	
	SrcModel.CreateSubObjects(Mesh);
	
	Mesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
	Mesh->SetLightMapCoordinateIndex(1);
	Mesh->SetLightMapResolution(LMSize);
	Mesh->bCustomizedCollision = false;
	Mesh->ComplexCollisionMesh = Mesh;
	Mesh->PostEditChange();
	Mesh->Build(false);
	Mesh->GenerateLodsInPackage();
	Mesh->SetLightingGuid();
	

	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Public | RF_Standalone;
	Args.SaveFlags = SAVE_NoError;

	return Mesh;
}

FString UQuakeMapAsset::GetUniqueEntityName(qformats::map::BaseEntity *Ent)
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

