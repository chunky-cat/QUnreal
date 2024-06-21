#include "Assets/QuakeMapAsset.h"
#include "Assets/QuakeWadAsset.h"
#include "RawMesh.h"
#include "UObject/SavePackage.h"
#include "Assets/AssetHelper.h"
#include "Entities/QSolidEntityActor.h"
#include "EditorFramework/AssetImportData.h"

qformats::textures::ITexture* UQuakeMapAsset::onTextureRequest(std::string name)
{
	UTexture2D* texture = nullptr;
	UMaterial* Material = BaseMaterial;

	if (MaterialOverrideCache.Contains(name.c_str()))
	{
		Material = MaterialOverrideCache[name.c_str()].Get();
	}

	if (TextureCache.Contains(name.c_str()))
	{
		texture = TextureCache[name.c_str()].Get();
	}

	auto MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	auto metatex = new qformats::textures::Texture<TWeakObjectPtr<UMaterialInstanceDynamic>>();
	metatex->SetWidth(128);
	metatex->SetHeight(128);
	if (texture != nullptr)
	{
		MaterialInstance->SetTextureParameterValue("BaseTexture", texture);
		metatex->SetWidth(texture->GetImportedSize().X);
		metatex->SetHeight(texture->GetImportedSize().Y);
		MaterialInstance->PostEditChange();
	}
	auto wp = MakeWeakObjectPtr<UMaterialInstanceDynamic>(MaterialInstance);
	metatex->SetData(wp);
	Materials.Push(MaterialInstance);
	return metatex;
}

inline void CreateEntityFromNative(FEntity* InEnt, qformats::map::BaseEntityPtr Nent, float InverseScale)
{
	InEnt->ClassName = FString(Nent->classname.c_str());
	InEnt->Origin = FVector3d(-Nent->origin.x(), Nent->origin.y(), Nent->origin.z()) / InverseScale;
	InEnt->Angle = Nent->angle;

	for (auto Prop : Nent->attributes)
	{
		InEnt->Properties.Add(FString(Prop.first.c_str()), FString(Prop.second.c_str()));
	}
}

void UQuakeMapAsset::Reset()
{
	if (LightMapDivider == 0)
	{
		LightMapDivider = 1024;
	}

	if (InverseScale == 0)
	{
		InverseScale = 1;
	}

	TextureCache.Empty();
	MaterialOverrideCache.Empty();
	Materials.Empty();
}

void UQuakeMapAsset::LoadMapFromFile(FString fileName)
{
	auto NativeMap = new qformats::map::QMap();
	NativeMap->LoadFile(std::string(TCHAR_TO_UTF8(*fileName)));

	if (NativeMap == nullptr)
	{
		return;
	}

	if (!BaseMaterial->IsValidLowLevel())
	{
		BaseMaterial = static_cast<UMaterial*>(StaticLoadObject(UMaterial::StaticClass(), nullptr,
																		   TEXT("/QUnreal/Materials/M_WadMaster"), nullptr,
																		   LOAD_None, nullptr));
	}

	MarkTexture(NativeMap, SkyTexture, qformats::map::Face::NODRAW);
	MarkTexture(NativeMap, SkipTexture, qformats::map::Face::SKIP);
	MarkTexture(NativeMap, ClipTexture, qformats::map::Face::CLIP);

	NativeMap->GenerateGeometry();
	FWadManager::GetInstance()->Refresh();

	FillCacheFromTextures(NativeMap);
	
	NativeMap->LoadTextures([this](std::string name) -> qformats::textures::ITexture* {
		return this->onTextureRequest(name);
	});

	if (NativeMap->GetSolidEntities().size() == 0)
		return;


	FVector3d Center{};
	WorldSpawnMesh = ConvertEntityToModel(NativeMap->GetSolidEntities()[0],Center);
	SolidEntities.Empty();
	EntityClassCount.clear();

	for (int i = 1; i < NativeMap->GetSolidEntities().size(); i++)
	{
		auto NativeSolidEntity = NativeMap->GetSolidEntities()[i];
		FSolidEntity NewSolidEntity{};
		CreateEntityFromNative(&NewSolidEntity, NativeSolidEntity, InverseScale);
		auto EntityMesh = ConvertEntityToModel(NativeSolidEntity,Center);
		if (EntityMesh == nullptr)
		{
			continue;
		}
		NewSolidEntity.Center = Center / InverseScale;
		NewSolidEntity.Mesh = EntityMesh,
			NewSolidEntity.UniqueClassName = FString(EntityMesh->GetName()),
			NewSolidEntity.Type = EntityType_Solid;
		NewSolidEntity.ClassTemplate = AQSolidEntityActor::StaticClass();
		if (EntityClassOverrides != nullptr && EntityClassOverrides->Classes.Contains(NewSolidEntity.ClassName))
		{
			NewSolidEntity.ClassTemplate = EntityClassOverrides->Classes[NewSolidEntity.ClassName];
		}

		SolidEntities.Emplace(NewSolidEntity);
	}

	for (const auto& NativePointEntity : NativeMap->GetPointEntities())
	{
		FEntity NewPointEntity{};
		CreateEntityFromNative(&NewPointEntity, NativePointEntity, InverseScale);
		NewPointEntity.UniqueClassName = GetUniqueEntityName(NativePointEntity.get());
		PointEntities.Emplace(NewPointEntity);
		NewPointEntity.ClassTemplate = AQEntityActor::StaticClass();
		if (EntityClassOverrides != nullptr && EntityClassOverrides->Classes.Contains(NewPointEntity.ClassName))
		{
			NewPointEntity.ClassTemplate = EntityClassOverrides->Classes[NewPointEntity.ClassName];
		}
	}

	for (auto it : NativeMap->GetTextures())
	{
		auto t = static_cast<qformats::textures::Texture<TWeakObjectPtr<UMaterialInstanceDynamic>>*>(it);
		t->SetData(nullptr);
		delete t;
	}

	if (NativeMap != nullptr)
	{
		delete NativeMap;
	}
}

void UQuakeMapAsset::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	UObject::PostEditChangeProperty(e);

	FName PropertyName = (e.Property != nullptr) ? e.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UQuakeMapAsset, EntityClassOverrides))
	{
		for (auto& PointEntity : PointEntities)
		{
			PointEntity.ClassTemplate = AQEntityActor::StaticClass();
			if (EntityClassOverrides != nullptr && EntityClassOverrides->Classes.Contains(PointEntity.ClassName))
			{
				PointEntity.ClassTemplate = EntityClassOverrides->Classes[PointEntity.ClassName];
			}
		}

		for (auto& SolidEntity : SolidEntities)
		{
			SolidEntity.ClassTemplate = AQSolidEntityActor::StaticClass();
			if (EntityClassOverrides != nullptr && EntityClassOverrides->Classes.Contains(SolidEntity.ClassName))
			{
				SolidEntity.ClassTemplate = EntityClassOverrides->Classes[SolidEntity.ClassName];
			}
		}

		QuakeMapUpdated.Broadcast();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UQuakeMapAsset, InverseScale))
	{
		LoadMapFromFile(SourceQMapFile);
		QuakeMapUpdated.Broadcast();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UQuakeMapAsset, TextureFolder))
	{
		LoadMapFromFile(SourceQMapFile);
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UQuakeMapAsset, MaterialOverrideFolder))
	{
		LoadMapFromFile(SourceQMapFile);
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UQuakeMapAsset, bImportLights))
	{
		QuakeMapUpdated.Broadcast();
	}
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
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(),
		                              FAssetRegistryTag::TT_Hidden));
	}
	UObject::GetAssetRegistryTags(OutTags);
}

void UQuakeMapAsset::Serialize(FArchive& Ar)
{
	UObject::Serialize(Ar);
}

UStaticMesh* UQuakeMapAsset::ConvertEntityToModel(const qformats::map::SolidEntityPtr& Entity, FVector3d &OutCenter)
{
	FVector3f EmptyVector = FVector3f(0, 0, 0);
	FColor WhiteVertex = FColor(255, 255, 255, 255);
	FRawMesh rawMesh;
	TArray<UMaterialInstanceDynamic*> TempMaterials;
	int offset_idx = 0, material_idx = 0;
	TMap<int32_t, int32_t> MatIDMap;
	auto b0 = Entity.get()->GetClippedBrushes()[0];
	FVector3d Min(b0.min.x(),b0.min.y(),b0.min.z());
	FVector3d Max(b0.max.x(),b0.max.y(),b0.max.z());

	
	
	double AreaEstimate = 0;
	
	for (const auto& b : Entity.get()->GetClippedBrushes())
	{
		for (const auto p : b.GetFaces())
		{
			const auto& vertices = p->GetVertices();
			const auto& indices = p->GetIndices();

			if (p->GetVertices().size() < 3 || p->Type() != qformats::map::Face::SOLID)
			{
				continue;
			}
			for (int i = p->GetVertices().size() - 1; i >= 0; i--)
			{
				const auto& pt = vertices[i];
				rawMesh.VertexPositions.Add(FVector3f(
					-pt.point[0],
					pt.point[1] ,
					pt.point[2]
					) / InverseScale);
			}

			for (int i = 0; i < indices.size() - 2; i += 3)
			{
				auto endv = vertices.size() - 1;
				const auto& pt = vertices[endv - indices[i]];
				const auto& pt2 = vertices[endv - indices[i + 1]];
				const auto& pt3 = vertices[endv - indices[i + 2]];

				rawMesh.WedgeIndices.Add(indices[i] + offset_idx);
				rawMesh.WedgeIndices.Add(indices[i + 1] + offset_idx);
				rawMesh.WedgeIndices.Add(indices[i + 2] + offset_idx);

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

				rawMesh.WedgeTangentZ.Add(FVector3f(pt.normal[0], pt.normal[1], pt.normal[2]));
				rawMesh.WedgeTangentZ.Add(FVector3f(pt2.normal[0], pt2.normal[1], pt2.normal[2]));
				rawMesh.WedgeTangentZ.Add(FVector3f(pt3.normal[0], pt3.normal[1], pt3.normal[2]));

				auto TexID = p->TextureID();
				if (!MatIDMap.Contains(TexID))
				{
					if (TexID < Materials.Num())
					{
						TempMaterials.Emplace(Materials[TexID]);
						MatIDMap.Add(TexID, material_idx++);
					}
				}
				rawMesh.FaceMaterialIndices.Add(MatIDMap[TexID]);
				rawMesh.FaceSmoothingMasks.Add(0);
			}
			offset_idx += vertices.size();
		}
		auto width = b.max.x() - b.min.x();
		auto depht = b.max.z() - b.min.z();
		auto height = b.max.y() - b.min.y();
		AreaEstimate += (2 * (width * height + width * depht + height * depht));
	}

	if (rawMesh.VertexPositions.Num() == 0)
	{
		return nullptr;
	}

	rawMesh.CompactMaterialIndices();
	auto entCenter = Entity->GetCenter();
	OutCenter = FVector3d(-entCenter.x(),entCenter.y(),entCenter.z());
	auto MapName = FPaths::GetBaseFilename(SourceQMapFile);
	FString MeshName = GetUniqueEntityName(Entity.get());
	FString PackagePath = this->GetPackage()->GetPathName();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath,
																	  FPackageName::GetAssetPackageExtension());
	UPackage* Pkg = CreatePackage(*PackagePath);
	UStaticMesh* Mesh = NewObject<UStaticMesh>(Pkg, *MeshName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	FAssetRegistryModule::AssetCreated(Mesh);
	for (auto Mat : TempMaterials)
	{
		Mesh->AddMaterial(Mat);
	}
	Pkg->FullyLoad();

	Mesh->AddSourceModel();
	auto& SrcModel = Mesh->GetSourceModel(0);
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = true;
	SrcModel.BuildSettings.bRecomputeNormals = true;
	SrcModel.BuildSettings.bRecomputeTangents = true;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = true;
	SrcModel.BuildSettings.bGenerateLightmapUVs = true;

	auto LightMapEstimate = AreaEstimate / LightMapDivider;
	if (LightMapEstimate > MaxLightmapSize)
	{
		LightMapEstimate = MaxLightmapSize;
	}

	if (LightMapEstimate < 32)
	{
		LightMapEstimate = 32;
	}
	SrcModel.BuildSettings.MinLightmapResolution = LightMapEstimate;
	SrcModel.BuildSettings.DstLightmapIndex = 0;
	SrcModel.BuildSettings.DstLightmapIndex = 1;
	SrcModel.BuildSettings.bComputeWeightedNormals = true;
	
	// Get LODGroup info
	ITargetPlatformManagerModule& TargetPlatformManager = GetTargetPlatformManagerRef();
	ITargetPlatform* RunningPlatform = TargetPlatformManager.GetRunningTargetPlatform();
	check(RunningPlatform);

	const FStaticMeshLODSettings& LODSettings = RunningPlatform->GetStaticMeshLODSettings();
	const auto& NewGroup = LODSettings.GetLODGroup(NAME_None);
	SrcModel.ReductionSettings = NewGroup.GetDefaultSettings(0);
	SrcModel.ReductionSettings.bKeepSymmetry = true;
	SrcModel.ReductionSettings.bCullOccluded = true;

	SrcModel.RawMeshBulkData->SaveRawMesh(rawMesh);
	SrcModel.CreateSubObjects(Mesh);

	Mesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
	Mesh->SetLightMapCoordinateIndex(1);
	Mesh->SetLightMapResolution(LightMapEstimate);
	Mesh->bCustomizedCollision = false;
	Mesh->ComplexCollisionMesh = Mesh;
	Mesh->Build(true);
	Mesh->PostEditChange();
	Mesh->SetLightingGuid();

	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Public | RF_Standalone;
	Args.SaveFlags = SAVE_NoError;

	return Mesh;
}

FString UQuakeMapAsset::GetUniqueEntityName(qformats::map::BaseEntity* Ent)
{
	auto Name = FString(Ent->classname.c_str()) + "_";
	if (!EntityClassCount.contains(Ent->classname))
	{
		EntityClassCount[Ent->classname] = 0;
		Name.AppendInt(0);
		return Name;
	}
	Name.AppendInt(EntityClassCount[Ent->classname] += 1);
	return Name;
}

void UQuakeMapAsset::MarkTexture(qformats::map::QMap *NativeMap, const FString& TextureName, qformats::map::Face::eFaceType t)
{
	if (!TextureName.IsEmpty())
	{
		NativeMap->SetFaceTypeByTextureID(TCHAR_TO_UTF8(*TextureName), t);
		NativeMap->SetFaceTypeByTextureID(TCHAR_TO_UTF8(*TextureName.ToUpper()), t);	
	}
}

void UQuakeMapAsset::FillCacheFromTextures(qformats::map::QMap* NativeMap)
{
	for (const auto &TexName : NativeMap->GetTexturesNames())
	{
		UTexture2D* texture = nullptr;
		UMaterial* Material = BaseMaterial;
		TArray<FAssetData> ObjectList;

		if (MaterialOverrideFolder != NAME_None) {
			AssetRegistryModule.Get().GetAssetsByPaths({MaterialOverrideFolder}, ObjectList, true);
			FHashedMaterialParameterInfo paramInfo{};
			paramInfo.Name = FScriptName("BaseTexture");
			for (const auto &Obj : ObjectList)
			{
				if (Obj.GetClass() == UMaterial::StaticClass() && FString(TexName.c_str()) == FPaths::GetBaseFilename(Obj.GetFullName()))
				{
					Material = Cast<UMaterial>(Obj.GetAsset());
					UTexture* TmpTex = nullptr;
					Material->GetTextureParameterValue(paramInfo, TmpTex);
					texture = Cast<UTexture2D>(TmpTex);
					break;
				}
			}
		}
		
		if (TextureFolder != NAME_None)
		{
			AssetRegistryModule.Get().GetAssetsByPaths({TextureFolder}, ObjectList, true);
			if (texture == nullptr)
			{
				for (const auto &Obj : ObjectList)
				{
					if (Obj.GetClass() == UTexture2D::StaticClass() && FString(TexName.c_str()) == FPaths::GetBaseFilename(Obj.GetFullName()))
					{
						texture = Cast<UTexture2D>(Obj.GetAsset());
						break;
					}
				}
			}
		}

		MaterialOverrideCache.Add(TexName.c_str(), Material);
		if (texture == nullptr)
		{
			texture = FWadManager::GetInstance()->FindTexture(TexName);
		}

		if (texture->IsValidLowLevel())
		{
				TextureCache.Add(FString(TexName.c_str()), texture);
		}
	}
}