#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusBombSite.generated.h"

class UBoxComponent;
class USphereComponent;
class ANexusCharacter;

UCLASS(Blueprintable)
class NEXUS_API ANexusBombSite : public AActor
{
	GENERATED_BODY()

public:
	ANexusBombSite();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UBoxComponent> SiteVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusSiteId SiteID = ENexusSiteId::A;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float PlantRadius = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float PlantTime = 7.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float DefuseTime = 7.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float BombTimer = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	TArray<FVector> AllowedPlantZones;

	bool IsInPlantZone(const FVector& WorldLocation) const;
};
