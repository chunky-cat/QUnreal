#pragma once

#include <CoreMinimal.h>
#include <qformats/wad/wad.h>
#include "WadTexture2D.generated.h"

USTRUCT()
struct FWadTexture2D
{
	GENERATED_BODY()
	
	void Generate(const FString &PackagePath, FString Name, const qformats::wad::QuakeTexture *Qtex);
	
	UPROPERTY(VisibleAnywhere) FString OriginalName;
	UPROPERTY(VisibleAnywhere) FString CleanName;
	UPROPERTY(VisibleAnywhere) FString WadName;
	UPROPERTY(VisibleAnywhere) UTexture2D *Texture = nullptr;
};
