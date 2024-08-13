#include "Assets/QuakeMapAsset.h"

#include "Builders/QMeshBuilder.h"
#include "Assets/QuakeWadAsset.h"
#include "RawMesh.h"
#include "Entities/QSolidEntityActor.h"
#include "EditorFramework/AssetImportData.h"
#include "Entities/QSolidTriggerActor.h"

qformats::textures::ITexture* UQuakeMapAsset::onTextureRequest(std::string name)
{
	UTexture2D* texture = nullptr;
	UMaterial* Material = Options.BaseMaterial;

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

fvec3 UQuakeMapAsset::CalculateEntityPivot(const qformats::map::SolidEntityPtr& Entity, EQuakeEntityPivot Pivot)
{
	if (Entity->attributes.contains("qu_pivot"))
	{
		auto pivotStr = Entity->attributes["qu_pivot"];
		int pivotNum = std::stoi(pivotStr);
		Pivot = static_cast<EQuakeEntityPivot>(pivotNum);
	}

	fvec3 PivotVec = Entity->GetMin();
	
	switch (Pivot)
	{
	case Lower_Left:
		{
			break;
		}
	case Upper_Left:
		{
			PivotVec.set_z(Entity->GetMax().z());
			break;
		}
	case Lower_Right:
		{
			PivotVec.set_x(Entity->GetMax().x());
			break;
		}
	case Upper_Right:
		{
			PivotVec.set_x(Entity->GetMax().x());
			PivotVec.set_z(Entity->GetMax().z());
			break;
		}
	case Center:
		{
			PivotVec = Entity->GetCenter();
			break;
		}
	case Lower_Center:
		{
			PivotVec = Entity->GetCenter();
			PivotVec.set_z(Entity->GetMin().z());
			break;
		}
	case Upper_Center:
		{
			PivotVec = Entity->GetCenter();
			PivotVec.set_z(Entity->GetMax().z());
			break;
		}
	}

	return PivotVec;
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
	if (Options.LightMapDivider == 0)
	{
		Options.LightMapDivider = 1024;
	}

	if (Options.InverseScale == 0)
	{
		Options.InverseScale = 1;
	}

	MapData->InverseScale = Options.InverseScale;
	TextureCache.Empty();
	MaterialOverrideCache.Empty();
	Materials.Empty();
}

UQuakeMapAsset::UQuakeMapAsset()
{
	FString PackagePath = this->GetPackage()->GetPathName();
	UPackage* Pkg = CreatePackage(*PackagePath);
	MapData = NewObject<UQuakeMapData>(Pkg, "MapData", RF_Public | RF_Standalone | RF_MarkAsRootSet);
	FAssetRegistryModule::AssetCreated(MapData);
}

void UQuakeMapAsset::LoadMapFromFile(FString fileName)
{
	Reset();
	auto NativeMap = new qformats::map::QMap();
	NativeMap->LoadFile(std::string(TCHAR_TO_UTF8(*fileName)));

	if (!bOverrideDefaultOptions)
	{
		const UQUnrealSettings* Settings = GetDefault<UQUnrealSettings>();
		Options = Settings->MapAssetOptions;
	}

	if (NativeMap == nullptr)
	{
		return;
	}

	if (!Options.BaseMaterial->IsValidLowLevel())
	{
		Options.BaseMaterial = static_cast<UMaterial*>(StaticLoadObject(UMaterial::StaticClass(), nullptr,
		                                                                TEXT("/QUnreal/Materials/M_WadMaster"), nullptr,
		                                                                LOAD_None, nullptr));
	}

	MarkTexture(NativeMap, Options.SkyTexture, qformats::map::Face::NODRAW);
	MarkTexture(NativeMap, Options.SkipTexture, qformats::map::Face::SKIP);
	MarkTexture(NativeMap, Options.ClipTexture, qformats::map::Face::CLIP);

	NativeMap->GenerateGeometry();
	FWadManager::GetInstance()->Refresh(AssetRegistryModule);

	FillCacheFromTextures(NativeMap);

	NativeMap->LoadTextures([this](std::string name) -> qformats::textures::ITexture* {
		return this->onTextureRequest(name);
	});

	if (NativeMap->GetSolidEntities().size() == 0 || NativeMap->WorldSpawn() == nullptr)
	{
		return;
	}


	if (NativeMap->WorldSpawn()->attributes.contains("qu_meshlib"))
	{
		bImportAsStaticMeshLib = true;
	}

	MapData->SolidEntities.Empty();
	MapData->PointEntities.Empty();
	EntityClassCount.clear();

	ConvertEntityToModel(NativeMap->GetSolidEntities()[0], MapData->WorldSpawn,Center);
	
	for (int i = 1; i < NativeMap->GetSolidEntities().size(); i++)
	{
		auto NativeSolidEntity = NativeMap->GetSolidEntities()[i];
		FSolidEntity NewSolidEntity{};
		CreateEntityFromNative(&NewSolidEntity, NativeSolidEntity, Options.InverseScale);
		ConvertEntityToModel(NativeSolidEntity, NewSolidEntity,Options.DefaultPivot);
		if (NewSolidEntity.Mesh == nullptr)
		{
			continue;
		}

		NewSolidEntity.Type = EntityType_Solid;
		NewSolidEntity.ClassTemplate = AQSolidEntityActor::StaticClass();
		if (Options.EntityClassOverrides != nullptr && Options.EntityClassOverrides->Classes.Contains(
			NewSolidEntity.ClassName))
		{
			NewSolidEntity.ClassTemplate = Options.EntityClassOverrides->Classes[NewSolidEntity.ClassName];
		}
		else
		{
			if (NewSolidEntity.ClassName.Contains("trigger"))
			{
				NewSolidEntity.ClassTemplate = AQSolidTriggerActor::StaticClass();
			}
		}

		MapData->SolidEntities.Emplace(NewSolidEntity);
	}

	if (!bImportAsStaticMeshLib)
	{
		for (const auto& NativePointEntity : NativeMap->GetPointEntities())
		{
			auto ClassName = FString(NativePointEntity->classname.c_str());

			if (Options.EntityClassOverrides != nullptr && Options.EntityClassOverrides->Classes.Contains(ClassName))
			{
				FEntity NewPointEntity{};
				CreateEntityFromNative(&NewPointEntity, NativePointEntity, Options.InverseScale);
				NewPointEntity.UniqueClassName = GetUniqueEntityName(NativePointEntity.get()->classname);
				MapData->PointEntities.Emplace(NewPointEntity);
				NewPointEntity.ClassTemplate = Options.EntityClassOverrides->Classes[NewPointEntity.ClassName];
				MapData->PointEntities.Emplace(NewPointEntity);
			}
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
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FQuakeMapAssetOptions, EntityClassOverrides))
	{
		for (auto& PointEntity : MapData->PointEntities)
		{
			if (Options.EntityClassOverrides != nullptr && Options.EntityClassOverrides->Classes.Contains(
				PointEntity.ClassName))
			{
				PointEntity.ClassTemplate = Options.EntityClassOverrides->Classes[PointEntity.ClassName];
			}
		}

		for (auto& SolidEntity : MapData->SolidEntities)
		{
			SolidEntity.ClassTemplate = AQSolidEntityActor::StaticClass();
			if (Options.EntityClassOverrides != nullptr && Options.EntityClassOverrides->Classes.Contains(
				SolidEntity.ClassName))
			{
				SolidEntity.ClassTemplate = Options.EntityClassOverrides->Classes[SolidEntity.ClassName];
			}
		}

		MapData->QuakeMapUpdated.Broadcast();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FQuakeMapAssetOptions, InverseScale))
	{
		MapData->InverseScale = Options.InverseScale;
		LoadMapFromFile(SourceQMapFile);
		MapData->QuakeMapUpdated.Broadcast();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FQuakeMapAssetOptions, TextureFolder))
	{
		LoadMapFromFile(SourceQMapFile);
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FQuakeMapAssetOptions, MaterialOverrideFolder))
	{
		LoadMapFromFile(SourceQMapFile);
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

void UQuakeMapAsset::ConvertEntityToModel(const qformats::map::SolidEntityPtr& Entity, FSolidEntity& OutEntity, EQuakeEntityPivot PivotOpt)
{
	auto b0 = Entity.get()->GetClippedBrushes()[0];
	FVector3d Min(b0.min.x(), b0.min.y(), b0.min.z());
	FVector3d Max(b0.max.x(), b0.max.y(), b0.max.z());
	fvec3 Pivot = CalculateEntityPivot(Entity, PivotOpt);
	FQMeshBuilder builder(Materials);

	if (bImportAsStaticMeshLib)
	{
		builder.SetVertexOffset(Pivot);
	}

	builder.SetInverseScale(Options.InverseScale);
	builder.ProcessEntity(Entity);

	if (!builder.HasRenderMesh())
	{
		return;
	}
	
	auto entCenter = Entity->GetCenter();
	OutEntity.Center = FVector3d(-entCenter.x(), entCenter.y(), entCenter.z()) / Options.InverseScale;
	OutEntity.Pivot = FVector3d(-Pivot.x(), Pivot.y(), Pivot.z()) / Options.InverseScale;
	
	auto MapName = FPaths::GetBaseFilename(SourceQMapFile);
	
	FString MeshName = GetUniqueEntityName(Entity.get()->ClassName());
	OutEntity.UniqueClassName = FString(MeshName);
	FString PackagePath = this->GetPackage()->GetPathName();
	
	if (bImportAsStaticMeshLib)
	{
		if (!Entity->tbName.empty())
		{
			MeshName = GetUniqueEntityName(Entity->tbName.c_str());
		}

		PackagePath = FPaths::GetPath(this->GetPathName());
		FString BaseName = FPaths::GetBaseFilename(this->GetPathName());
		PackagePath = FPaths::Combine(PackagePath, BaseName + "_Meshes", MeshName);
	}

	UPackage* Pkg = CreatePackage(*PackagePath);
	Pkg->FullyLoad();

	UStaticMesh* Mesh = NewObject<UStaticMesh>(Pkg, *MeshName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	FAssetRegistryModule::AssetCreated(Mesh);
	builder.SetupRenderSourceModel(Mesh, Options.LightMapDivider, Options.MaxLightmapSize);

	if (!bImportAsStaticMeshLib && builder.HasClipMesh()) {

		UStaticMesh* ClipMesh = NewObject<UStaticMesh>(Pkg, *MeshName.Append("_Clip"), RF_Public | RF_Standalone | RF_MarkAsRootSet);
		FAssetRegistryModule::AssetCreated(ClipMesh);
		builder.SetupClippingSourceModel(ClipMesh);
		OutEntity.ClipMesh = ClipMesh;
		OutEntity.ClipMesh->PostEditChange();
	}
	OutEntity.Mesh = Mesh;
}

FString UQuakeMapAsset::GetUniqueEntityName(const std::string& ClassName)
{
	auto Name = FString(ClassName.c_str()) + "_";
	if (!EntityClassCount.contains(ClassName))
	{
		EntityClassCount[ClassName] = 0;
		Name.AppendInt(0);
		return Name;
	}
	Name.AppendInt(EntityClassCount[ClassName] += 1);
	return Name;
}

void UQuakeMapAsset::MarkTexture(qformats::map::QMap* NativeMap, const FString& TextureName,
                                 qformats::map::Face::eFaceType t)
{
	if (!TextureName.IsEmpty())
	{
		NativeMap->SetFaceTypeByTextureID(TCHAR_TO_UTF8(*TextureName), t);
		NativeMap->SetFaceTypeByTextureID(TCHAR_TO_UTF8(*TextureName.ToUpper()), t);
	}
}

void UQuakeMapAsset::FillCacheFromTextures(qformats::map::QMap* NativeMap)
{
	for (const auto& TexName : NativeMap->GetTexturesNames())
	{
		UTexture2D* texture = nullptr;
		UMaterial* Material = Options.BaseMaterial;
		TArray<FAssetData> ObjectList;

		if (Options.MaterialOverrideFolder != NAME_None)
		{
			AssetRegistryModule.Get().GetAssetsByPaths({Options.MaterialOverrideFolder}, ObjectList, true);
			FHashedMaterialParameterInfo paramInfo{};
			paramInfo.Name = FScriptName("BaseTexture");
			for (const auto& Obj : ObjectList)
			{
				auto CleanName = FWadTexture2D::ToCleanName(FString(TexName.c_str()));
				if (Obj.GetClass() == UMaterial::StaticClass() && CleanName == FPaths::GetBaseFilename(Obj.GetFullName()))
				{
					Material = Cast<UMaterial>(Obj.GetAsset());
					UTexture* TmpTex = nullptr;
					Material->GetTextureParameterValue(paramInfo, TmpTex);
					texture = Cast<UTexture2D>(TmpTex);
					break;
				}
			}
		}

		if (Options.TextureFolder != NAME_None)
		{
			AssetRegistryModule.Get().GetAssetsByPaths({Options.TextureFolder}, ObjectList, true);
			if (texture == nullptr)
			{
				for (const auto& Obj : ObjectList)
				{
					if (Obj.GetClass() == UTexture2D::StaticClass() && FString(TexName.c_str()) ==
						FPaths::GetBaseFilename(Obj.GetFullName()))
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
