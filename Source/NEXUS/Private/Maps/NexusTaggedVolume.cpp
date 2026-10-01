#include "Maps/NexusTaggedVolume.h"
#include "Components/BoxComponent.h"

ANexusTaggedVolume::ANexusTaggedVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	SetRootComponent(Volume);
	Volume->SetBoxExtent(FVector(100.f, 100.f, 150.f));
	Volume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Volume->SetCollisionResponseToAllChannels(ECR_Overlap);
}
