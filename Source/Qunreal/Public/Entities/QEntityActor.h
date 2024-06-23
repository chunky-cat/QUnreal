#pragma once

#include <CoreMinimal.h>
#include "QEntityActor.generated.h"


USTRUCT()
struct QUNREAL_API FQEntityProperty
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere) FString Key;
	UPROPERTY(VisibleAnywhere) FString Value;
};

USTRUCT(Blueprintable, BlueprintType)
struct FQEntityData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName EntityName;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString ClassName;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString TargetName;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Message;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Angle;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FString,FString> Properties;
};


UINTERFACE(Blueprintable, BlueprintType)
class QUNREAL_API UQEntityInitiator : public UInterface
{
	GENERATED_BODY()
};

class QUNREAL_API IQEntityInitiator
{
	GENERATED_BODY()

public:
	void Initialize();
	virtual void Setup();
	virtual FQEntityData& GetEntityData() = 0;
};

UCLASS(Blueprintable)
class QUNREAL_API AQEntityActor : public AActor, public IQEntityInitiator
{
		GENERATED_BODY()
public:
	AQEntityActor();
	virtual FQEntityData& GetEntityData() override {return EntityData;}
		
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FQEntityData EntityData;

	UFUNCTION(BlueprintImplementableEvent,  Category = "QUnreal")
	void OnTriggered(AActor* TriggerActor, AQEntityActor* Caller);
};