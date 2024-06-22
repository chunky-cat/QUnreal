#pragma once
#include "QEntityActor.h"
#include "QuakeMapData.h"

#include "Engine/PointLight.h"
#include "QWorldSpawnActor.generated.h"

UCLASS()
class QUNREAL_API AQWorldSpawnActor : public AActor
{
	GENERATED_BODY()
public:
	AQWorldSpawnActor();
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;
	
public:
	UPROPERTY(EditAnywhere, Category = "QuakeMapAsset")
	TObjectPtr<UQuakeMapData> MapData;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Quake Entities")
	TArray<AQEntityActor*> SolidEntities;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Point Lights")
	TArray<APointLight*> PointLights;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* WorldSpawnMeshComponent;
	virtual void OnConstruction(const FTransform& Transform) override;
	void OnQuakeMapUpdated();
	void ReloadFromAsset();
	virtual void PostRegisterAllComponents() override;

private:
	bool bAlreadyPostRegistered = false;
	void ImportLightFromMap(const FEntity &LightEntity);
};
