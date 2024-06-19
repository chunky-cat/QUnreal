// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/QEntityClassesData.h"
#include "QUnrealSettings.generated.h"


UCLASS(Config=Editor, defaultconfig, meta = (DisplayName="QUnreal"))
class QUNREAL_API UQUnrealSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	float InverseScale = 1.0;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Textures")
	FName TextureFolder = "/Game/Textures";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Textures")
	FName MaterialFolder = "/Game/Materials";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Textures")
	FString ClipTexture = "clip";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Textures")
	FString SkipTexture = "skip";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Textures")
	FString SkyTexture = "sky";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Entities")
	TSoftObjectPtr<UQEntityClassesData> DefaultEntityClasses;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Entities")
	bool bImportLights = false;
};
