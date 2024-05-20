#pragma once

#include <CoreMinimal.h>
#include <QuakeWAD.generated.h>

UCLASS()
class UQuakeWAD : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere) FString SourceQMapFile;
	void ProcessFile(FString sourceFile);
};
