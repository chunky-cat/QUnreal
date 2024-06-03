#pragma once

#include <CoreMinimal.h>
#include "QuakeMapAsset.h"
#include "QEntityActor.generated.h"

UCLASS()
class QUNREAL_API AQEntitySceneActor : public AActor
{
	GENERATED_BODY()
public:
	AQEntitySceneActor();

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QuakeMapAsset")
	UQuakeMapAsset* QuakeMapAsset;

	UPROPERTY()
	UStaticMesh* EntityMeshReference;
	UPROPERTY()
	UStaticMeshComponent* EntityMeshComponent;
	UPROPERTY()
	FName EntityName;
	UPROPERTY()
	FString ClassName;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Setup();
};
