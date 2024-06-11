#pragma once
#include "QEntityActor.h"
#include "QuakeMapAsset.h"
#include "QWorldSpawnSceneActor.generated.h"

UCLASS()
class QUNREAL_API AQWorldSpawnSceneActor : public AActor
{
	GENERATED_BODY()
public:
	AQWorldSpawnSceneActor();
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QuakeMapAsset")
	TWeakObjectPtr<UQuakeMapAsset> QuakeMapAsset;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Quake Entities")
	TArray<AQEntitySceneActor*> SolidEntities;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* WorldSpawnMeshComponent;
	virtual void OnConstruction(const FTransform& Transform) override;
private:
	void OnAssetUpdate() const;
	int onAssetCallbackID = -1;
};
