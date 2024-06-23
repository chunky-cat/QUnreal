#include "Entities/QSolidTriggerActor.h"


void AQSolidTriggerActor::Setup()
{
	Super::Setup();
	EntityMeshComponent->SetVisibility(false);
	EntityMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EntityMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	EntityMeshComponent->SetMobility(EComponentMobility::Movable);
	EntityMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	EntityMeshComponent->bHiddenInGame = true;

	bOnce = EntityData.ClassName.Contains("once");
	bTeleport = EntityData.ClassName.Contains("teleport");
	if (EntityData.Properties.Contains("target"))
	{
		TriggerTarget = EntityData.Properties["target"];	
	}
}

void AQSolidTriggerActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AQWorldSpawnActor* WorldSpawn = Cast<AQWorldSpawnActor>(GetOwner());
	if (WorldSpawn == nullptr || !WorldSpawn->TriggerTargets.Contains(TriggerTarget))
	{
		Super::NotifyActorBeginOverlap(OtherActor);
		return;
	}

	for (const auto &Targets = WorldSpawn->TriggerTargets[TriggerTarget].Targets; const auto &Target : Targets)
	{
		if (Target == nullptr)
		{
			continue;
		}
		Target->OnTriggered(OtherActor, this);
	}
	
	Super::NotifyActorBeginOverlap(OtherActor);
}
