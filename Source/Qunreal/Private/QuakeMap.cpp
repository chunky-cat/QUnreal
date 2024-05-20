// Fill out your copyright notice in the Description page of Project Settings.


#include "QuakeMap.h"

UQMap::UQMap()
{
}

UClass* FQMapAssetTypeAction::GetSupportedClass() const
{
	return UQMap::StaticClass();
}
 
FText FQMapAssetTypeAction::GetName() const
{
	return INVTEXT("Quake Map");
}
 
FColor FQMapAssetTypeAction::GetTypeColor() const
{
	return FColor::Cyan;
}
 
uint32 FQMapAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::None;
}