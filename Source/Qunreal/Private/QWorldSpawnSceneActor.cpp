#include "QWorldSpawnSceneActor.h"
#include "PhysicsEngine/BodySetup.h"

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
		WorldSpawnMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		WorldSpawnMeshComponent->UpdateCollisionFromStaticMesh();
	}
}
