#pragma once

#include "AssetRegistry/AssetRegistryModule.h"

template<typename T>
void GetObjectsOfClass(TArray<T*>& OutArray, FName inPackage = "/Script/QUnreal")
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	
	FName ClassName = T::StaticClass()->GetFName();
	FTopLevelAssetPath TopLevelAssetPath(inPackage,ClassName);
	AssetRegistryModule.Get().GetAssetsByClass(TopLevelAssetPath, AssetData);
	for (int i = 0; i < AssetData.Num(); i++) {
		T* Object = Cast<T>(AssetData[i].GetAsset());
		OutArray.Add(Object);
	}
}
