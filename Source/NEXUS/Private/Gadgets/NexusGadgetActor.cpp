#include "Gadgets/NexusGadgetActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Art/NexusMaterialLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NexusCharacter.h"
#include "EngineUtils.h"

ANexusGadgetActor::ANexusGadgetActor()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Radius"));
	SetRootComponent(Sphere);
	Sphere->InitSphereRadius(100.f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(Sphere);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetCanEverAffectNavigation(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube.Succeeded())
	{
		VisualMesh->SetStaticMesh(Cube.Object);
	}
}

void ANexusGadgetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANexusGadgetActor, GadgetId);
	DOREPLIFETIME(ANexusGadgetActor, OwnerTeam);
	DOREPLIFETIME(ANexusGadgetActor, RadiusUu);
	DOREPLIFETIME(ANexusGadgetActor, bJamming);
	DOREPLIFETIME(ANexusGadgetActor, bSmoke);
	DOREPLIFETIME(ANexusGadgetActor, bMarking);
}

void ANexusGadgetActor::BeginPlay()
{
	Super::BeginPlay();
}

void ANexusGadgetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LifeTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void ANexusGadgetActor::Arm(ENexusGadgetId InId, ENexusTeam Team, float Duration, float RadiusMeters)
{
	GadgetId = InId;
	OwnerTeam = Team;
	RadiusUu = RadiusMeters * 100.f;
	if (USphereComponent* Sphere = Cast<USphereComponent>(RootComponent))
	{
		Sphere->SetSphereRadius(RadiusUu);
	}
	bJamming = (InId == ENexusGadgetId::Jammer || InId == ENexusGadgetId::PersonalJammer || InId == ENexusGadgetId::EMPMine || InId == ENexusGadgetId::ElectricCharge);
	bSmoke = (InId == ENexusGadgetId::Smoke || InId == ENexusGadgetId::Incendiary);
	bMarking = (InId == ENexusGadgetId::Drone || InId == ENexusGadgetId::OpticalBeacon || InId == ENexusGadgetId::Camera || InId == ENexusGadgetId::MotionSensor || InId == ENexusGadgetId::OpticalDetector);

	if (VisualMesh)
	{
		FVector Scale(0.18f, 0.18f, 0.12f);
		ENexusArtSurface Surf = ENexusArtSurface::Metal;
		switch (InId)
		{
		case ENexusGadgetId::Smoke:
		case ENexusGadgetId::Incendiary:
			Scale = FVector(0.14f, 0.14f, 0.22f);
			Surf = ENexusArtSurface::RustyMetal;
			break;
		case ENexusGadgetId::Drone:
			Scale = FVector(0.28f, 0.28f, 0.08f);
			break;
		case ENexusGadgetId::Jammer:
		case ENexusGadgetId::PersonalJammer:
		case ENexusGadgetId::EMPMine:
			Scale = FVector(0.2f, 0.2f, 0.16f);
			Surf = ENexusArtSurface::DirtyConcrete;
			break;
		default:
			break;
		}
		VisualMesh->SetRelativeScale3D(Scale);
		UNexusMaterialLibrary::ApplyToPrimitive(VisualMesh, Surf);
	}

	if (bJamming)
	{
		TArray<ANexusGadgetActor*> ToDestroy;
		for (TActorIterator<ANexusGadgetActor> It(GetWorld()); It; ++It)
		{
			ANexusGadgetActor* Other = *It;
			if (!IsValid(Other) || Other == this || !Other->bMarking)
			{
				continue;
			}
			if (FVector::Dist(GetActorLocation(), Other->GetActorLocation()) <= RadiusUu)
			{
				ToDestroy.Add(Other);
			}
		}
		for (ANexusGadgetActor* Other : ToDestroy)
		{
			if (IsValid(Other))
			{
				Other->Destroy();
			}
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(LifeTimer, this, &ANexusGadgetActor::Expire, FMath::Max(Duration, 0.2f), false);
	}
}

bool ANexusGadgetActor::IsCounteredBy(const ANexusGadgetActor* Other) const
{
	if (!Other) { return false; }
	if (bMarking && Other->bJamming) { return true; }
	return false;
}

void ANexusGadgetActor::Expire()
{
	Destroy();
}
