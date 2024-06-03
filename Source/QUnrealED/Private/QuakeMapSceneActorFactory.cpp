#include "QuakeMapSceneActorFactory.h"
#include <QuakeMapAsset.h>

#include "QWorldSpawnSceneActor.h"


UQuakeMapSceneActorFactory::UQuakeMapSceneActorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = FText::FromString(TEXT("BSP Scene"));
	NewActorClass = AQWorldSpawnSceneActor::StaticClass();
	bUsePlacementExtent = true;
}

bool UQuakeMapSceneActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) {

	if (AssetData.IsValid() && !AssetData.GetClass()->IsChildOf(AQWorldSpawnSceneActor::StaticClass()))
	{
		OutErrorMsg = FText::FromString(TEXT("BSP Asset is not valid"));
		return false;
	}

	return true;
}

void UQuakeMapSceneActorFactory::PostSpawnActor(UObject *Asset, AActor *NewActor)
{
	UQuakeMapAsset *QMAsset = Cast<UQuakeMapAsset>(Asset);

	if (QMAsset != nullptr)
	{
		AQWorldSpawnSceneActor *NewBSPScene = CastChecked<AQWorldSpawnSceneActor>(NewActor);
		FActorLabelUtilities::SetActorLabelUnique(NewBSPScene, QMAsset->GetName());
		NewBSPScene->QuakeMapAsset = QMAsset;
	}
}

bool UQuakeMapSceneActorFactory::CanPlaceElementsFromAssetData(const FAssetData& InAssetData)
{
	return true;
}

AActor* UQuakeMapSceneActorFactory::GetDefaultActor(const FAssetData& AssetData)
{
	return NewActorClass->GetDefaultObject<AQWorldSpawnSceneActor>();
}

AActor* UQuakeMapSceneActorFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams)
{
	AQWorldSpawnSceneActor* actor;
	{
		actor = Cast<AQWorldSpawnSceneActor>(Super::SpawnActor(InAsset, InLevel, InTransform, InSpawnParams));
	}

	actor->QuakeMapAsset = Cast<UQuakeMapAsset>(InAsset);

	if (actor->HasAnyFlags(RF_Transient))
	{
		return actor;
	}
	if (actor->SolidEntities.Num() > 0) return actor;
	int meshNum = 0;
	for (int i = 0;  i < actor->QuakeMapAsset->EntityMeshes.Num(); i++)
	{
		auto EntityMesh = actor->QuakeMapAsset->EntityMeshes[i];
		auto MeshStr = FString("EntityMesh_");
		MeshStr.AppendInt(meshNum++);
		FActorSpawnParameters p;
		//p.Name = FName(*MeshStr);
		p.Owner = actor;
		auto EntityActor = actor->GetWorld()->SpawnActor<AQEntitySceneActor>(
			AQEntitySceneActor::StaticClass(),actor->GetTransform(),p);
		EntityActor->QuakeMapAsset = actor->QuakeMapAsset;
		FActorLabelUtilities::SetActorLabelUnique(EntityActor, MeshStr);
		
		EntityActor->EntityName = (*MeshStr);
		EntityActor->ClassName = actor->QuakeMapAsset->GetClassName(i);
		EntityActor->EntityMeshReference = EntityMesh;
		EntityActor->EntityMeshComponent->SetStaticMesh(EntityMesh);
		EntityActor->Setup();
		
		EntityActor->AttachToActor(actor, FAttachmentTransformRules::KeepWorldTransform);
		actor->SolidEntities.Emplace(EntityActor);
	}
	
	return actor;
}

/*
FAxisAlignedBox3d UseBox = (EditPivotActions->bUseWorldBox) ? WorldBounds : ObjectBounds;
FVector3d Point = UseBox.Center();


NewTransform = FTransform((FVector)Point);

ActiveGizmos[0].TransformGizmo->SetNewGizmoTransform(NewTransform);
*/
void UQuakeMapSceneActorFactory::PostPlaceAsset(TArrayView<const FTypedElementHandle> InHandle,
	const FAssetPlacementInfo& InPlacementInfo, const FPlacementOptions& InPlacementOptions)
{
	Super::PostPlaceAsset(InHandle, InPlacementInfo, InPlacementOptions);
}
