#include "QEntityActor.h"
#include "PhysicsEngine/BodySetup.h"

AQEntitySceneActor::AQEntitySceneActor()
{
	EntityMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	PrimaryActorTick.bCanEverTick = false;
}

void AQEntitySceneActor::OnConstruction(const FTransform& Transform)
{

}

void AQEntitySceneActor::Setup()
{
	if (ClassName.Contains("trigger"))
	{
		EntityMeshComponent->SetVisibility(false);
		EntityMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		EntityMeshComponent->UpdateCollisionFromStaticMesh();
		EntityMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		EntityMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		EntityMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		EntityMeshComponent->bHiddenInGame = true;
	}
}
