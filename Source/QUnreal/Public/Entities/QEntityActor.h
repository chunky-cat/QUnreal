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
class QUNREAL_API UQEntityEvents : public UInterface
{
	GENERATED_BODY()
};

class QUNREAL_API IQEntityEvents
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,  Category = "QUnreal")
	void OnTriggered(AActor* TriggerActor, AQEntityActor* Caller);
};

UCLASS(ClassGroup=(Custom), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class UQEntityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FQEntityData EntityData;
};

UCLASS(Blueprintable)
class QUNREAL_API AQEntityActor : public AActor, public IQEntityEvents
{
		GENERATED_BODY()
public:
	AQEntityActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName="QUnreal Entity Component")
	UQEntityComponent *EntityComponent;
	virtual void Setup();
};