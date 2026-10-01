#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusRouteSpline.generated.h"

class USplineComponent;

UCLASS(Blueprintable)
class NEXUS_API ANexusRouteSpline : public AActor
{
	GENERATED_BODY()

public:
	ANexusRouteSpline();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusRouteId RouteId = ENexusRouteId::RouteA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName DisplayName;

	float GetLengthUu() const;
	float GetEstimatedSeconds() const;
};
