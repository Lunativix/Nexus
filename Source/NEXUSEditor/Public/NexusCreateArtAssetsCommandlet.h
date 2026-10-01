#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NexusCreateArtAssetsCommandlet.generated.h"

UCLASS()
class UNexusCreateArtAssetsCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UNexusCreateArtAssetsCommandlet();
	virtual int32 Main(const FString& Params) override;
};
