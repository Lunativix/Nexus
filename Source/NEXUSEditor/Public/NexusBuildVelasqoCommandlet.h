#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NexusBuildVelasqoCommandlet.generated.h"

UCLASS()
class UNexusBuildVelasqoCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UNexusBuildVelasqoCommandlet();
	virtual int32 Main(const FString& Params) override;
};
