#include "Entities/QSolidEntityActor.h"
#include "PhysicsEngine/BodySetup.h"

AQSolidEntityActor::AQSolidEntityActor()
{
	EntityMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	RootComponent = EntityMeshComponent;
	ClipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClipMesh"));
	ClipMeshComponent->bHiddenInGame = true;
	PrimaryActorTick.bCanEverTick = false;
}

void AQSolidEntityActor::Setup()
{
	if (EntityMeshComponent->GetStaticMesh()->IsValidLowLevelFast() && EntityMeshComponent->GetStaticMesh()->GetNumSourceModels() > 0)
	{
		EntityMeshComponent->UpdateCollisionFromStaticMesh();
		EntityMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		EntityMeshComponent->SetMobility(Mobility);
		EntityMeshComponent->SetVisibility(true);
		EntityMeshComponent->bHiddenInGame = false;
	}

	if (ClipMeshComponent->GetStaticMesh()->IsValidLowLevelFast() && ClipMeshComponent->GetStaticMesh()->GetNumSourceModels() > 0)
	{
		ClipMeshComponent->UpdateCollisionFromStaticMesh();
		ClipMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		ClipMeshComponent->SetMobility(Mobility);
		ClipMeshComponent->SetVisibility(true);
		ClipMeshComponent->bHiddenInGame = true;
	}
}
