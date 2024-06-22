#include "Assets/QuakeWadAsset.h"
#include "Assets/AssetHelper.h"

UQuakeWadAsset::UQuakeWadAsset() {}

UQuakeWadAsset::~UQuakeWadAsset()
{
	//object->MarkPendingKill(); <-- this way we can kill stuff
	FWadManager::GetInstance()->RemoveWad(this);
}

void UQuakeWadAsset::LoadWadFromFile(FString FileName)
{
	this->SourceQWadFile = FileName;
	auto WadBaseFileName = FPaths::GetBaseFilename(FileName);
	WadFile = qformats::wad::QuakeWad::FromFile(std::string(TCHAR_TO_UTF8(*FileName)));
	
	for(const auto &Tex : WadFile->Textures())
	{
		if (Tex.second.header.type != qformats::wad::QuakeWadEntry::QuakeWadEntryType::QWET_MipsTexture)
			continue;
		
		const auto qtex = WadFile->GetTexture(Tex.first);
		auto Utex = FWadTexture2D();
		Utex.WadName = WadBaseFileName;
		Utex.Generate(GetPackage()->GetPathName(),FString(Tex.first.c_str()), qtex);
		Textures.Emplace(Utex);
	}
}

FWadManager* FWadManager::GetInstance()
{
	if (FWadManager::Instance == nullptr)
	{
		FWadManager::Instance = new FWadManager();
	}
	return FWadManager::Instance;
}

void FWadManager::Refresh()
{
	TArray<FAssetData> ObjectList;
	FAssetRegistryModule AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));;
	AssetRegistryModule.Get().GetAssetsByPaths({"/Game"}, ObjectList, true);
	for (const auto& Obj : ObjectList)
	{
		if (Obj.GetClass() == UQuakeWadAsset::StaticClass())
		{
			auto w = Cast<UQuakeWadAsset>(Obj.GetAsset());
			AddWadAsset(w);
		}
	}
	
}


FWadManager *FWadManager::Instance = nullptr;

void FWadManager::AddWadAsset(UQuakeWadAsset* Wa)
{
	if (Wa == nullptr || Wa->bAlreadyInManager) return;
	Wa->bAlreadyInManager = true;
	WadAssets.push_back(Wa);
}

void FWadManager::RemoveWad(UQuakeWadAsset* Wa)
{
	if (Wa == nullptr) return;
	if (const auto Elem = std::ranges::find(WadAssets, Wa); Elem != WadAssets.end())
	{
		WadAssets.erase(Elem);
	}
}

UTexture2D* FWadManager::FindTexture(const FName Name)
{
	if (TextureCache.contains(Name.ToString()))
	{
		return TextureCache[Name.ToString()];
	}

	for (const auto Wad : WadAssets)
	{
		for (const auto WadTex : Wad->Textures)
		{
			if (WadTex.OriginalName == Name)
			{
				TextureCache[Name.ToString()] = WadTex.Texture;
				return WadTex.Texture;
			}
		}
	}
	return nullptr;
}

