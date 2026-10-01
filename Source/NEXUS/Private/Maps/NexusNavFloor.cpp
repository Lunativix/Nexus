#include "Maps/NexusNavFloor.h"
#include "Components/BoxComponent.h"

ANexusNavFloor::ANexusNavFloor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	Floor = CreateDefaultSubobject<UBoxComponent>(TEXT("Floor"));
	SetRootComponent(Floor);
	Floor->SetBoxExtent(FVector(6000.f, 6000.f, 8.f));
	Floor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Floor->SetCollisionObjectType(ECC_WorldStatic);
	Floor->SetCollisionResponseToAllChannels(ECR_Block);
	Floor->SetCanEverAffectNavigation(true);
	Floor->SetHiddenInGame(true);
	SetActorLocation(FVector(0.f, 0.f, 8.f));
}
