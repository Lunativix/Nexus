#include "Objectives/NexusBombSite.h"
#include "NexusGameplaySettings.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

ANexusBombSite::ANexusBombSite()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SiteVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SiteVolume"));
	SetRootComponent(SiteVolume);
	SiteVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SiteVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void ANexusBombSite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ANexusBombSite::BeginPlay()
{
	Super::BeginPlay();
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	PlantTime = Settings->PlantTimeSeconds;
	DefuseTime = Settings->DefuseTimeSeconds;
	BombTimer = Settings->BombTimerSeconds;
	PlantRadius = Settings->PlantRadiusMeters * 100.f;
}

bool ANexusBombSite::IsInPlantZone(const FVector& WorldLocation) const
{
	if (AllowedPlantZones.Num() == 0)
	{
		return FVector::Dist2D(WorldLocation, GetActorLocation()) <= PlantRadius;
	}
	for (const FVector& Zone : AllowedPlantZones)
	{
		if (FVector::Dist2D(WorldLocation, Zone) <= PlantRadius)
		{
			return true;
		}
	}
	return false;
}
