// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/QEntityClassesData.h"
#include "QUnrealSettings.generated.h"

UENUM(Blueprintable)
enum EQuakeEntityPivot
{
	Lower_Left = 0,
	Upper_Left = 1,
	Lower_Right = 2,
	Upper_Right = 3,
	Center = 4,
	Lower_Center = 5,
	Upper_Center = 6,
};

USTRUCT(Blueprintable)
struct QUNREALED_API FQuakeMapAssetOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UQEntityClassesData* EntityClassOverrides = nullptr;
	
	UPROPERTY(EditAnywhere)
	UMaterial *BaseMaterial = nullptr;
	
	UPROPERTY(EditAnywhere)
	float InverseScale = 1;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EQuakeEntityPivot> DefaultPivot = Lower_Left;

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
};



UCLASS(Config=Editor, defaultconfig, meta = (DisplayName="QUnreal"))
class QUNREALED_API UQUnrealSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UQUnrealSettings() {};
	
 	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FQuakeMapAssetOptions MapAssetOptions;
};
