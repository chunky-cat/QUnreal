#include "Entities/QWorldSpawnActor.h"
#include "Entities/QSolidEntityActor.h"
#include "Kismet/BlueprintTypeConversions.h"

#include "PhysicsEngine/BodySetup.h"

// Sets default values
AQWorldSpawnActor::AQWorldSpawnActor()
{
	WorldSpawnMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldSpawnMesh"));
	RootComponent = WorldSpawnMeshComponent;
	WorldSpawnMeshComponent->SetMobility(EComponentMobility::Static);
    PrimaryActorTick.bCanEverTick = false;
}

void AQWorldSpawnActor::BeginDestroy()
{
	SolidEntities.Empty();
	Super::BeginDestroy();
}

void AQWorldSpawnActor::Destroyed()
{
	TArray<AActor*> Actors;
	GetAttachedActors(Actors);
	
	for (auto Actor : Actors)
	{
		Actor->Destroy();
	}
	Super::Destroyed();
}

void AQWorldSpawnActor::OnConstruction(const FTransform& Transform)
{
#ifdef UE_EDITOR
	if (MapData->IsValidLowLevel() && MapData->WorldSpawnMesh != nullptr)
	{
		WorldSpawnMeshComponent->SetMobility(EComponentMobility::Static);
		WorldSpawnMeshComponent->SetStaticMesh(MapData->WorldSpawnMesh);
		WorldSpawnMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		WorldSpawnMeshComponent->UpdateCollisionFromStaticMesh();
	}

#endif
}

void AQWorldSpawnActor::ReloadFromAsset()
{
	if (!MapData->IsValidLowLevel() && !MapData->WorldSpawnMesh)
	{
		return;
	}

	WorldSpawnMeshComponent->SetMobility(EComponentMobility::Static);
	WorldSpawnMeshComponent->SetStaticMesh(MapData->WorldSpawnMesh);
	WorldSpawnMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	WorldSpawnMeshComponent->UpdateCollisionFromStaticMesh();
	
	
	for (auto Actor : SolidEntities)
	{
		Actor->Destroy();
		Actor->Owner = nullptr;
	}

	for (auto Actor : PointEntities)
	{
		Actor->Destroy();
		Actor->Owner = nullptr;
	}

	SolidEntities.Empty();
	PointEntities.Empty();
	TriggerTargets.Empty();
	
	for (int i = 0;  i < MapData->SolidEntities.Num(); i++)
	{
		auto Entity = MapData->SolidEntities[i];
		auto MeshStr = FString("AS_"+Entity.UniqueClassName);
		FActorSpawnParameters p;
		p.Owner = this;
		
		auto EntityActor = GetWorld()->SpawnActor<AQSolidEntityActor>(
			Entity.ClassTemplate,GetTransform(),p);

		if (EntityActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not spawn EntityActor (%s)"), *Entity.UniqueClassName);
			continue;
		}
		
		
#if WITH_EDITOR
		FActorLabelUtilities::SetActorLabelUnique(EntityActor, MeshStr);
		EntityActor->SetPivotOffset(Entity.Center);
#endif
		
		EntityActor->GetEntityData().EntityName = (*MeshStr);
		EntityActor->GetEntityData().ClassName = Entity.ClassName;
		EntityActor->GetEntityData().Properties = Entity.Properties;
		EntityActor->GetEntityData().Angle = Entity.Angle;
		EntityActor->EntityMeshComponent->SetStaticMesh(Entity.Mesh);
		EntityActor->Initialize();

		EntityActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		SolidEntities.Emplace(EntityActor);
		if (EntityActor->EntityData.TargetName != "")
		{
			if (TriggerTargets.Contains(EntityActor->EntityData.TargetName))
			{
				TriggerTargets[EntityActor->EntityData.TargetName].Targets.Add(EntityActor);
			} else
			{
				TriggerTargets.Add(EntityActor->EntityData.TargetName, {{EntityActor}});
			}
		}
	}

	for (int i = 0;  i < MapData->PointEntities.Num(); i++)
	{
		auto Entity = MapData->PointEntities[i];
		ImportPointEntity(Entity);
	}
}

void AQWorldSpawnActor::PostRegisterAllComponents()
{
	if (!GetWorld()->IsEditorWorld()  || GetWorld()->IsPlayInEditor() || MapData == nullptr)
	{
		return;
	}
#if WITH_EDITOR
	if (!bAlreadyPostRegistered && MapData->WorldSpawnMesh->IsValidLowLevel() && !HasAnyFlags(RF_Transient))
	{
		bAlreadyPostRegistered = true;
		UE_LOG(LogTemp, Warning, TEXT("registered all components, REROLL!!!"));
		ReloadFromAsset();
		MapData->QuakeMapUpdated.AddUObject(this, &AQWorldSpawnActor::ReloadFromAsset);
	}
#endif

	Super::PostRegisterAllComponents();
}

void AQWorldSpawnActor::ImportPointEntity(const FEntity& Entity)
{
	auto MeshStr = FString("AP_"+Entity.UniqueClassName);
	FActorSpawnParameters p;
	p.Owner = this;

	auto Trans = Entity.Origin + GetTransform().GetLocation();
	auto PtActor = GetWorld()->SpawnActor<AActor>(Entity.ClassTemplate,FTransform3d(Trans),p);

	if (PtActor == nullptr)
	{
		return;
	}

	auto Angle = Entity.Angle;
	if (Angle != 270 && Angle != 90)
	{
		Angle += 180;
	}
	
	PtActor->SetActorRotation(FRotator(0,Angle,0));
	
	/*
	float Brightness = 300;
	if (LightEntity.Properties.Contains("light"))
	{
		Brightness = FCString::Atof(*LightEntity.Properties["light"]);
	}

	Brightness = Brightness/MapData->InverseScale;
	PtLight->SetRadius(Brightness*6);
	PtLight->SetBrightness(Brightness*0.7);
	*/
#if WITH_EDITOR
	FActorLabelUtilities::SetActorLabelUnique(PtActor, MeshStr);
#endif

	PtActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	//PtActor->SetMobility(EComponentMobility::Static);
	PointEntities.Emplace(PtActor);
}

void AQWorldSpawnActor::OnQuakeMapUpdated()
{
#ifdef UE_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("map updated event, REROLL!!!"));
	ReloadFromAsset();
#endif
}


