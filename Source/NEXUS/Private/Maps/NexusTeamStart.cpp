#include "Maps/NexusTeamStart.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

ANexusTeamStart::ANexusTeamStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	SafeZone = CreateDefaultSubobject<USphereComponent>(TEXT("SafeZone"));
	SafeZone->SetupAttachment(RootComponent);
	SafeZone->SetSphereRadius(400.f);
	SafeZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SafeZone->SetCollisionResponseToAllChannels(ECR_Overlap);
	SafeZone->SetHiddenInGame(true);
}
