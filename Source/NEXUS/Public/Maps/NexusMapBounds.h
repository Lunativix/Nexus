#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusMapBounds.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class NEXUS_API ANexusMapBounds : public AActor
{
	GENERATED_BODY()

public:
	ANexusMapBounds();

	virtual void BeginPlay() override;

	/** Resize the query box to the catalog playable half-extent for this world. */
	void ApplyPlayableExtent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UBoxComponent> Bounds;

	/** Velasqo default. Per-map extent comes from FNexusMapCatalog::PlayableHalfUU. */
	static constexpr float HalfXY = 6000.f;
	static constexpr float HeightZ = 2500.f;
};
