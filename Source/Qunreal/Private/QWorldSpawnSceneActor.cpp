#include "QWorldSpawnSceneActor.h"

// Sets default values
AQWorldSpawnSceneActor::AQWorldSpawnSceneActor()
{
	WorldSpawnMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BSPMesh"));
    PrimaryActorTick.bCanEverTick = false;
}

void AQWorldSpawnSceneActor::BeginDestroy()
{
	SolidEntities.Empty();
	Super::BeginDestroy();
}

void AQWorldSpawnSceneActor::Destroyed()
{
	TArray<AActor*> Actors;
	GetAttachedActors(Actors);
	
	for (auto Actor : Actors)
	{
		Actor->Destroy();
	}
	Super::Destroyed();
}

void AQWorldSpawnSceneActor::OnConstruction(const FTransform& Transform)
{
	if (QuakeMapAsset != nullptr && QuakeMapAsset->WorldSpawnMesh != nullptr)
	{
		WorldSpawnMeshComponent->SetStaticMesh(QuakeMapAsset->WorldSpawnMesh);
		onAssetCallbackID = QuakeMapAsset->RegisterOnUpdatedCallback([this](){OnAssetUpdate();});
	}
}

void AQWorldSpawnSceneActor::OnAssetUpdate() const
{
	WorldSpawnMeshComponent->SetStaticMesh(QuakeMapAsset->WorldSpawnMesh);
}
