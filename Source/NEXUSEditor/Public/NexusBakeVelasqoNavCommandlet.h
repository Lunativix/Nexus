#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NexusBakeVelasqoNavCommandlet.generated.h"

UCLASS()
class UNexusBakeVelasqoNavCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UNexusBakeVelasqoNavCommandlet();
	virtual int32 Main(const FString& Params) override;
};
