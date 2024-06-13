#include "QSolidEntityActor.h"
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
	EntityMeshComponent->SetMobility(EComponentMobility::Static);
	EntityMeshComponent->SetVisibility(true);
	EntityMeshComponent->bHiddenInGame = false;
		
	if (ClassName.Contains("trigger"))
	{
		EntityMeshComponent->SetVisibility(false);
		EntityMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		EntityMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		EntityMeshComponent->SetMobility(EComponentMobility::Movable);
		EntityMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		EntityMeshComponent->bHiddenInGame = true;
	}
}
