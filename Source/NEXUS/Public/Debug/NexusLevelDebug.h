#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusLevelDebug.generated.h"

UCLASS()
class NEXUS_API ANexusLevelDebug : public AActor
{
	GENERATED_BODY()

public:
	ANexusLevelDebug();
	virtual void Tick(float DeltaSeconds) override;

	static void Toggle(UWorld* World, int32 Mask);

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 ChannelMask = -1;
};
