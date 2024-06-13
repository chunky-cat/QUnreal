#pragma once

#include <CoreMinimal.h>
#include "QEntityActor.h"
#include "QSolidEntityActor.generated.h"

UCLASS()
class QUNREAL_API AQSolidEntityActor : public AQEntityActor
{
	GENERATED_BODY()
public:
	AQSolidEntityActor();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* EntityMeshComponent;
	virtual void Setup() override;
};
