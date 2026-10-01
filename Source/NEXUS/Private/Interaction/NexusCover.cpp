#include "Interaction/NexusCover.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/NexusPenetration.h"

ANexusCover::ANexusCover()
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

void ANexusCover::BeginPlay()
{
	Super::BeginPlay();
	float HeightM = 0.8f;
	float DepthM = 0.45f;
	switch (CoverType)
	{
	case ENexusCoverType::Low: HeightM = 0.8f; break;
	case ENexusCoverType::Medium: HeightM = 1.2f; break;
	case ENexusCoverType::High: HeightM = 1.85f; break;
	}
	Mesh->SetWorldScale3D(FVector(DepthM, WidthMeters, HeightM));
	SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, HeightM * 50.f));
	if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
	{
		if (UMaterialInstanceDynamic* MID = Mesh->CreateDynamicMaterialInstance(0, Base))
		{
			MID->SetVectorParameterValue(TEXT("Color"), UNexusPenetrationStatics::GetMaterialColor(Material));
		}
	}
}
