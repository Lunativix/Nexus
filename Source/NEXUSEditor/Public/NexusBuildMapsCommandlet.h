#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NexusBuildMapsCommandlet.generated.h"

UCLASS()
class UNexusBuildMapsCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UNexusBuildMapsCommandlet();
	virtual int32 Main(const FString& Params) override;
};
