#pragma once
#include "QSolidEntityActor.h"
#include "QWorldSpawnActor.h"
#include "QSolidTriggerActor.generated.h"

UCLASS(Blueprintable)
class QUNREAL_API AQSolidTriggerActor : public AQSolidEntityActor
{
	GENERATED_BODY()
public:
	virtual void Setup() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Category="Trigger")
	bool bOnce = false;
	UPROPERTY(EditAnywhere, Category="Trigger")
	bool bTeleport = false;
	UPROPERTY(EditAnywhere, Category="Trigger")
	bool bTriggered = false;
	UPROPERTY(EditAnywhere, Category="Trigger")
	FString TriggerTarget;
};
