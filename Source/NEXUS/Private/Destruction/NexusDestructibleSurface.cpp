#include "Destruction/NexusDestructibleSurface.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/NexusPenetration.h"
#include "Art/NexusMaterialLibrary.h"

ANexusDestructibleSurface::ANexusDestructibleSurface()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
}

void ANexusDestructibleSurface::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANexusDestructibleSurface, ReplicatedHealth);
}

void ANexusDestructibleSurface::BeginPlay()
{
	Super::BeginPlay();
	ReplicatedHealth = Health;
	InitialHealth = Health;
	RefreshVisual();
}

void ANexusDestructibleSurface::ApplyDamage(float Amount)
{
	if (!HasAuthority() || !bCanBeDestroyed)
	{
		return;
	}
	Health = FMath::Max(0.f, Health - Amount);
	ReplicatedHealth = Health;
	if (Health <= 0.f)
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetVisibility(false);
	}
	RefreshVisual();
}

void ANexusDestructibleSurface::OnRep_Health()
{
	Health = ReplicatedHealth;
	RefreshVisual();
}

void ANexusDestructibleSurface::RefreshVisual()
{
	if (Health <= 0.f)
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetVisibility(false);
		return;
	}
	UNexusMaterialLibrary::ApplyGameplayMaterial(Mesh, MaterialType);
}

void ANexusDestructibleSurface::ResetForNewRound()
{
	if (!HasAuthority())
	{
		return;
	}
	Health = InitialHealth;
	ReplicatedHealth = Health;
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetVisibility(true);
	RefreshVisual();
}
