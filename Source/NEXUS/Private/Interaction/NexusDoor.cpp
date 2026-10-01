#include "Interaction/NexusDoor.h"
#include "NexusCharacter.h"
#include "NexusGameplaySettings.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/NexusPenetration.h"

ANexusDoor::ANexusDoor()
{
	bReplicates = true;
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetBoxExtent(FVector(6.f, 50.f, 105.f));
	Collision->SetCollisionProfileName(TEXT("BlockAll"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ANexusDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANexusDoor, bOpen);
	DOREPLIFETIME(ANexusDoor, Health);
	DOREPLIFETIME(ANexusDoor, DoorState);
	DOREPLIFETIME(ANexusDoor, bLocked);
}

void ANexusDoor::BeginPlay()
{
	Super::BeginPlay();
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	if (DoorType == ENexusDoorType::Reinforced)
	{
		Health = Settings->ReinforcedDoorHP;
		Collision->SetBoxExtent(FVector(8.f, 90.f, 120.f));
		Mesh->SetWorldScale3D(FVector(0.16f, 1.8f, 2.4f));
	}
	else if (DoorType == ENexusDoorType::Double)
	{
		Health = Settings->StandardDoorHP;
		Collision->SetBoxExtent(FVector(6.f, 90.f, 120.f));
		Mesh->SetWorldScale3D(FVector(0.12f, 1.8f, 2.4f));
	}
	else
	{
		Health = Settings->StandardDoorHP;
		Collision->SetBoxExtent(FVector(6.f, 50.f, 105.f));
		Mesh->SetWorldScale3D(FVector(0.12f, 1.0f, 2.1f));
	}
	ApplyVisualState();
	DoorState = ComputeState();
}

void ANexusDoor::ServerToggle()
{
	if (!HasAuthority() || bLocked)
	{
		return;
	}
	if (ComputeState() == ENexusDoorState::Destroyed)
	{
		return;
	}
	bOpen = !bOpen;
	DoorState = ComputeState();
	ApplyVisualState();
}

ENexusDoorState ANexusDoor::ComputeState() const
{
	if (Health <= 0.f)
	{
		return ENexusDoorState::Destroyed;
	}
	if (bLocked)
	{
		return ENexusDoorState::Locked;
	}
	return bOpen ? ENexusDoorState::Open : ENexusDoorState::Closed;
}

bool ANexusDoor::CanNexusInteract(ANexusCharacter* Character) const
{
	return Character && Character->Health > 0.f && ComputeState() != ENexusDoorState::Destroyed && !bLocked;
}

void ANexusDoor::ExecuteNexusInteract(ANexusCharacter* Character)
{
	if (HasAuthority() && CanNexusInteract(Character))
	{
		ServerToggle();
	}
}

void ANexusDoor::OnRep_DoorState()
{
	ApplyVisualState();
}

void ANexusDoor::ApplyDamage(float Amount)
{
	if (!HasAuthority() || Health <= 0.f)
	{
		return;
	}
	if (DoorType == ENexusDoorType::Standard && Amount > 0.f)
	{
		// Standard doors open, they do not require destruction — still take damage if tagged destructible.
	}
	if (DoorType == ENexusDoorType::Destructible || DoorType == ENexusDoorType::Reinforced)
	{
		Health = FMath::Max(0.f, Health - Amount);
		if (Health <= 0.f)
		{
			bOpen = true;
		}
		DoorState = ComputeState();
		ApplyVisualState();
	}
}

void ANexusDoor::OnRep_Open()
{
	ApplyVisualState();
}

void ANexusDoor::OnRep_Health()
{
	ApplyVisualState();
}

void ANexusDoor::ApplyVisualState()
{
	const bool bBlocking = !bOpen && Health > 0.f;
	Collision->SetCollisionEnabled(bBlocking ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	Mesh->SetVisibility(bBlocking);
	const ENexusMaterialType Mat = (DoorType == ENexusDoorType::Reinforced) ? ENexusMaterialType::ThickStone : ENexusMaterialType::Wood;
	if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
	{
		if (UMaterialInstanceDynamic* MID = Mesh->CreateDynamicMaterialInstance(0, Base))
		{
			MID->SetVectorParameterValue(TEXT("Color"), UNexusPenetrationStatics::GetMaterialColor(Mat));
		}
	}
}

void ANexusDoor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ANexusDoor::ResetForNewRound()
{
	if (!HasAuthority())
	{
		return;
	}
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	Health = (DoorType == ENexusDoorType::Reinforced) ? Settings->ReinforcedDoorHP : Settings->StandardDoorHP;
	bOpen = false;
	DoorState = ENexusDoorState::Closed;
	ApplyVisualState();
}
