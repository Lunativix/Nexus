#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusTaggedVolume.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class ENexusTaggedVolumeType : uint8
{
	SiteEntrance,
	DefensivePosition,
	SpawnSafeCluster
};

UCLASS(Blueprintable)
class NEXUS_API ANexusTaggedVolume : public AActor
{
	GENERATED_BODY()

public:
	ANexusTaggedVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusTaggedVolumeType VolumeType = ENexusTaggedVolumeType::SiteEntrance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusSiteId SiteId = ENexusSiteId::None;
};
