#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "NexusTypes.h"
#include "NexusTeamStart.generated.h"

class USphereComponent;

UCLASS(Blueprintable)
class NEXUS_API ANexusTeamStart : public APlayerStart
{
	GENERATED_BODY()

public:
	ANexusTeamStart(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusTeam Team = ENexusTeam::Attackers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	int32 SpawnID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusRouteId PreferredRoute = ENexusRouteId::RouteA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FRotator SpawnRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float SafeZoneRadius = 400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<USphereComponent> SafeZone;

	bool bOccupied = false;
};
