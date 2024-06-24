#include "WadTexture2D.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/Image.h"
#include "UObject/SavePackage.h"

void FWadTexture2D::Generate(const FString &PackagePath, FString Name, const qformats::wad::QuakeTexture *Qtex)
{
	OriginalName = Name;
	CleanName = OriginalName;
	if (OriginalName[0] == '+' || OriginalName[0] == '*')
	{
		CleanName = OriginalName.RightChop(1);
	}

	//FString PackagePath = FString("/Game/QUnrealData/WadData/") + WadName + "/Textures/" + CleanName;
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

	UPackage* Pkg = CreatePackage(*PackagePath);
	Texture = NewObject<UTexture2D>(Pkg, *CleanName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	FAssetRegistryModule::AssetCreated(this->Texture);
	Pkg->FullyLoad();

	
	//Initialize the platform data to store necessary information regarding our texture asset
	Texture->SetPlatformData(new FTexturePlatformData);
	Texture->GetPlatformData()->SizeX = Qtex->width;
	Texture->GetPlatformData()->SizeY = Qtex->height;
	Texture->GetPlatformData()->SetNumSlices(1);
	Texture->GetPlatformData()->PixelFormat = EPixelFormat::PF_B8G8R8A8;

	
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	Texture->GetPlatformData()->Mips.Add(Mip);
	Mip->BulkData.Lock(LOCK_READ_WRITE);

	size_t size = Qtex->raw.size() * sizeof(qformats::wad::color);
		
	uint8* mipData = Mip->BulkData.Realloc(size);

	int i = 0;
	for (const auto &c : Qtex->raw)
	{
		mipData[i++] = c.rgba[2];
		mipData[i++] = c.rgba[1];
		mipData[i++] = c.rgba[0];
		mipData[i++] = c.rgba[3];
	}

	Mip->BulkData.Unlock();
	
	Texture->Source.Init(Qtex->width, Qtex->height, 1, 1, TSF_BGRA8, mipData);
	Texture->SRGB = true;
	Texture->Filter = TF_Nearest;
	Texture->UpdateResource();
}