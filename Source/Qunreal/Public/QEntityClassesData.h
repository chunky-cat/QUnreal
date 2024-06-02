#pragma once

#include <CoreMinimal.h>
#include <QEntityActor.h>
#include "QEntityClassesData.generated.h"


USTRUCT()
struct FQEntityClassesDataEntry
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	FName ClassName;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AQEntitySceneActor> Class;
};

UCLASS(BlueprintType, Category="QUnreal")
class UQEntityClassesData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ClassName"))
	TArray<FQEntityClassesDataEntry> Classes;
};
