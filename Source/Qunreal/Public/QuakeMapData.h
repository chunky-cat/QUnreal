#pragma once

#include "Entities/QEntityActor.h"
#include "QuakeMapData.generated.h"

UENUM()
enum EEntityType
{
	EntityType_Point = 0,
	EntityType_Solid = 1,
};

USTRUCT()
struct FEntity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere) TEnumAsByte<EEntityType> Type;
	UPROPERTY(EditAnywhere) FString UniqueClassName;
	UPROPERTY(VisibleAnywhere) FString ClassName;
	UPROPERTY(VisibleAnywhere) FVector3d Origin;
	UPROPERTY(VisibleAnywhere) float Angle;
	UPROPERTY(VisibleAnywhere) TMap<FString,FString> Properties;
	UPROPERTY(VisibleAnywhere) TSubclassOf<AQEntityActor> ClassTemplate;
};

USTRUCT()
struct FSolidEntity : public FEntity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere) UStaticMesh *Mesh;
	UPROPERTY(VisibleAnywhere) FVector3d Center;
};

UCLASS()
class QUNREAL_API UQuakeMapData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere) UStaticMesh *WorldSpawnMesh;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "UniqueClassName"))
	TArray<FSolidEntity> SolidEntities;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "UniqueClassName"))
	TArray<FEntity> PointEntities;

	bool bImportLights = false;
	float InverseScale = 1.0;

	FSimpleMulticastDelegate QuakeMapUpdated;
	
};
