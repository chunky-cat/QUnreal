#include "Entities/QSolidEntityActor.h"
#include "PhysicsEngine/BodySetup.h"

AQSolidEntityActor::AQSolidEntityActor()
{
	EntityMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	RootComponent = EntityMeshComponent;
	PrimaryActorTick.bCanEverTick = false;
}

void AQSolidEntityActor::Setup()
{
	EntityMeshComponent->UpdateCollisionFromStaticMesh();
	EntityMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	EntityMeshComponent->SetMobility(Mobility);
	EntityMeshComponent->SetVisibility(true);
	EntityMeshComponent->bHiddenInGame = false;
}
