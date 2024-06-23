#include "Entities/QEntityActor.h"

void IQEntityInitiator::Initialize()
{
	if (GetEntityData().Properties.Contains("targetname"))
	{
		GetEntityData().TargetName = GetEntityData().Properties["targetname"];	
	}
	if (GetEntityData().Properties.Contains("message"))
	{
		GetEntityData().Message = GetEntityData().Properties["message"];	
	}

	Setup();
}

void IQEntityInitiator::Setup()
{
	
}

AQEntityActor::AQEntityActor()
{
}
