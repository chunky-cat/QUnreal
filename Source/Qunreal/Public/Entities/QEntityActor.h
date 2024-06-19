#pragma once

#include <CoreMinimal.h>
#include "Assets/QuakeMapAsset.h"
#include "QEntityActor.generated.h"


USTRUCT()
struct QUNREAL_API FQEntityProperty
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere) FString Key;
	UPROPERTY(VisibleAnywhere) FString Value;
	
};

UCLASS()
class QUNREAL_API AQEntityActor : public AActor
{
	GENERATED_BODY()
public:
	AQEntityActor();

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QuakeMapAsset")
	UQuakeMapAsset *QuakeMapAsset;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName EntityName;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString ClassName;
	UPROPERTY(VisibleAnywhere)
	TMap<FString,FString> Properties;
	virtual void Setup();
};
