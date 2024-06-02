#include "QEntityActor.h"

AQEntitySceneActor::AQEntitySceneActor()
{
	EntityMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	PrimaryActorTick.bCanEverTick = false;
}

void AQEntitySceneActor::OnConstruction(const FTransform& Transform)
{
	if (ClassName.Contains("trigger"))
	{
		EntityMeshComponent->SetVisibility(false);
		EntityMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		EntityMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		EntityMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		EntityMeshComponent->bHiddenInGame = true;
	}
}
