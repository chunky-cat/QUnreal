// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "QuakeMap.generated.h"

class QUNREAL_API FQMapAssetTypeAction : public FAssetTypeActions_Base
{
public:
	UClass* GetSupportedClass() const override;
	FText GetName() const override;
	FColor GetTypeColor() const override;
	uint32 GetCategories() override;
};

UCLASS()
class QUNREAL_API UQMap : public UObject
{
	GENERATED_BODY()
public:
	UQMap();
	UPROPERTY(VisibleAnywhere) FString SourceQMapFile;
};
