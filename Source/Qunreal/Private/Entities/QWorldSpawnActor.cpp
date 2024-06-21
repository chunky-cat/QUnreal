#include "Entities/QWorldSpawnActor.h"
#include "Entities/QSolidEntityActor.h"

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
	if (QuakeMapAsset != nullptr && QuakeMapAsset->WorldSpawnMesh != nullptr)
	{
		WorldSpawnMeshComponent->SetStaticMesh(QuakeMapAsset->WorldSpawnMesh);
		WorldSpawnMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		WorldSpawnMeshComponent->UpdateCollisionFromStaticMesh();
	}

#endif
}

void AQWorldSpawnActor::ReloadFromAsset()
{
	if (QuakeMapAsset == nullptr)
	{
		return;
	}
	
	//TArray<AActor*> Actors;
	//GetAttachedActors(Actors);
	for (auto Actor : SolidEntities)
	{
		Actor->Destroy();
		Actor->Owner = nullptr;
	}

	for (auto Actor : PointLights)
	{
		Actor->Destroy();
		Actor->Owner = nullptr;
	}

	SolidEntities.Empty();
	PointLights.Empty();
	
	for (int i = 0;  i < QuakeMapAsset->SolidEntities.Num(); i++)
	{
		auto Entity = QuakeMapAsset->SolidEntities[i];
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
		
		EntityActor->QuakeMapAsset = QuakeMapAsset;
		FActorLabelUtilities::SetActorLabelUnique(EntityActor, MeshStr);
		
		EntityActor->EntityName = (*MeshStr);
		EntityActor->ClassName = Entity.ClassName;
		EntityActor->EntityMeshComponent->SetStaticMesh(Entity.Mesh);
		EntityActor->Setup();
		EntityActor->SetPivotOffset(Entity.Center);
		EntityActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		SolidEntities.Emplace(EntityActor);
	}

	for (int i = 0;  i < QuakeMapAsset->PointEntities.Num(); i++)
	{
		auto Entity = QuakeMapAsset->PointEntities[i];
		if (Entity.ClassName == "light" && QuakeMapAsset->Options.bImportLights)
		{
			ImportLightFromMap(Entity);
		}
	}
}

void AQWorldSpawnActor::PostRegisterAllComponents()
{
	if (GetWorld()->IsEditorWorld())
	{
		return;
	}
	
#ifdef UE_EDITOR
	if (!bAlreadyPostRegistered && QuakeMapAsset->IsValidLowLevel() && !HasAnyFlags(RF_Transient))
	{
		bAlreadyPostRegistered = true;
		UE_LOG(LogTemp, Warning, TEXT("registered all components, REROLL!!!"));
		ReloadFromAsset();
		QuakeMapAsset->QuakeMapUpdated.AddUObject(this, &AQWorldSpawnActor::ReloadFromAsset);
	}
#endif	
	Super::PostRegisterAllComponents();
}

void AQWorldSpawnActor::ImportLightFromMap(const FEntity& LightEntity)
{
	auto MeshStr = FString("AP_"+LightEntity.UniqueClassName);
	FActorSpawnParameters p;
	p.Owner = this;

	auto Trans = LightEntity.Origin + GetTransform().GetLocation();
	Trans /= QuakeMapAsset->Options.InverseScale;
	auto PtLight = GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(),FTransform3d(Trans),p);
		
	float Brightness = 300;
	if (LightEntity.Properties.Contains("light"))
	{
		Brightness = FCString::Atof(*LightEntity.Properties["light"]);
	}

	Brightness = Brightness/QuakeMapAsset->Options.InverseScale;
	PtLight->SetRadius(Brightness*6);
	PtLight->SetBrightness(Brightness*0.7);
	FActorLabelUtilities::SetActorLabelUnique(PtLight, MeshStr);
	PtLight->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	PtLight->SetMobility(EComponentMobility::Static);
	PointLights.Emplace(PtLight);
}

void AQWorldSpawnActor::OnQuakeMapUpdated()
{
#ifdef UE_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("map updated event, REROLL!!!"));
	ReloadFromAsset();
#endif
}


