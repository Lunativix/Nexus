#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusNavFloor.generated.h"

class UBoxComponent;

/** Thin walkable plane used only to seed Recast when ISM instances do not export nav. */
UCLASS()
class NEXUS_API ANexusNavFloor : public AActor
{
	GENERATED_BODY()

public:
	ANexusNavFloor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UBoxComponent> Floor;
};
