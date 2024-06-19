#include "Entities/QEntityActor.h"

AQEntityActor::AQEntityActor(): QuakeMapAsset(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AQEntityActor::Setup()
{
	
}
