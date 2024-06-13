#include "QEntityActor.h"
#include "PhysicsEngine/BodySetup.h"

AQEntityActor::AQEntityActor(): QuakeMapAsset(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AQEntityActor::Setup()
{
}
