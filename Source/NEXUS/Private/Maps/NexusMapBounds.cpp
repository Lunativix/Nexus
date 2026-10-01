#include "Maps/NexusMapBounds.h"
#include "Maps/NexusMapCatalog.h"
#include "Components/BoxComponent.h"

ANexusMapBounds::ANexusMapBounds()
{
	PrimaryActorTick.bCanEverTick = false;
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	SetRootComponent(Bounds);
	Bounds->SetBoxExtent(FVector(HalfXY, HalfXY, HeightZ * 0.5f));
	SetActorLocation(FVector(0.f, 0.f, HeightZ * 0.5f));
	Bounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Bounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	Bounds->SetHiddenInGame(true);
}

void ANexusMapBounds::BeginPlay()
{
	Super::BeginPlay();
	ApplyPlayableExtent();
}

void ANexusMapBounds::ApplyPlayableExtent()
{
	if (!Bounds)
	{
		return;
	}
	const float H = FNexusMapCatalog::PlayableHalfUU(FNexusMapCatalog::DetectFromWorld(GetWorld()));
	Bounds->SetBoxExtent(FVector(H, H, HeightZ * 0.5f));
}
