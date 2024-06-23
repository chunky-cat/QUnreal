// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/QEntityClassesData.h"
#include "QUnrealSettings.generated.h"

USTRUCT(Blueprintable)
struct QUNREAL_API FQuakeMapAssetOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UQEntityClassesData* EntityClassOverrides;
	
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
};



UCLASS(Config=Editor, defaultconfig, meta = (DisplayName="QUnreal"))
class QUNREAL_API UQUnrealSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UQUnrealSettings() {};
	
 	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FQuakeMapAssetOptions MapAssetOptions;
};
