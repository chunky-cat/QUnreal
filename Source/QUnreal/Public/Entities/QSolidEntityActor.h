#pragma once

#include <CoreMinimal.h>
#include "QEntityActor.h"
#include "QSolidEntityActor.generated.h"

UCLASS(Blueprintable)
class QUNREAL_API AQSolidEntityActor : public AQEntityActor
{
	GENERATED_BODY()
public:
	AQSolidEntityActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Static;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* EntityMeshComponent;
	virtual void Setup() override;
};
