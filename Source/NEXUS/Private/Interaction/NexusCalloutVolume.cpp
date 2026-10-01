#include "Interaction/NexusCalloutVolume.h"
#include "Components/BoxComponent.h"
#include "NexusCharacter.h"

ANexusCalloutVolume::ANexusCalloutVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	SetRootComponent(Volume);
	Volume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Volume->SetCollisionResponseToAllChannels(ECR_Ignore);
	Volume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Volume->SetGenerateOverlapEvents(true);
	Volume->OnComponentBeginOverlap.AddDynamic(this, &ANexusCalloutVolume::OnOverlapBegin);
}

void ANexusCalloutVolume::OnOverlapBegin(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (ANexusCharacter* Character = Cast<ANexusCharacter>(OtherActor))
	{
		Character->SetCallout(DisplayName);
	}
}
