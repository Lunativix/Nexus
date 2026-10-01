#include "Maps/NexusRouteSpline.h"
#include "Components/SplineComponent.h"
#include "NexusGameplaySettings.h"

ANexusRouteSpline::ANexusRouteSpline()
{
	PrimaryActorTick.bCanEverTick = false;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);
	Spline->SetDrawDebug(true);
}

float ANexusRouteSpline::GetLengthUu() const
{
	return Spline ? Spline->GetSplineLength() : 0.f;
}

float ANexusRouteSpline::GetEstimatedSeconds() const
{
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	const float Speed = FMath::Max(Settings->RotationWalkSpeedUu, 1.f);
	return GetLengthUu() / Speed;
}
