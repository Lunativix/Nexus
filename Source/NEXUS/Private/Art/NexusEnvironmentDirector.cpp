#include "Art/NexusEnvironmentDirector.h"
#include "Art/NexusArtSettings.h"
#include "Art/NexusMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Maps/NexusVelasqoBuilder.h"
#include "Maps/NexusVelasqoLayout.h"
#include "Maps/NexusMapCatalog.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "Interaction/NexusDoor.h"
#include "Interaction/NexusCover.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"
#include "Interaction/NexusCalloutVolume.h"
#include "Components/BoxComponent.h"
#include "Debug/NexusVelasqoValidation.h"
#include "NEXUS.h"

ANexusEnvironmentDirector::ANexusEnvironmentDirector()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube.Succeeded())
	{
		CubeMesh = Cube.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cyl(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (Cyl.Succeeded())
	{
		CylinderMesh = Cyl.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sph(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (Sph.Succeeded())
	{
		SphereMesh = Sph.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cone(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (Cone.Succeeded())
	{
		ConeMesh = Cone.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (Plane.Succeeded())
	{
		PlaneMesh = Plane.Object;
	}

	auto MakeISM = [this, Root](const TCHAR* Name) -> UInstancedStaticMeshComponent*
	{
		UInstancedStaticMeshComponent* ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(Name);
		ISM->SetupAttachment(Root);
		ConfigureVisualISM(ISM);
		return ISM;
	};
	VisualWindows = MakeISM(TEXT("VisualWindows"));
	VisualTrim = MakeISM(TEXT("VisualTrim"));
	VisualProps = MakeISM(TEXT("VisualProps"));
	VisualGround = MakeISM(TEXT("VisualGround"));
	VisualMarketCloth = MakeISM(TEXT("VisualMarketCloth"));
	VisualMarketGoods = MakeISM(TEXT("VisualMarketGoods"));
	VisualWarehouse = MakeISM(TEXT("VisualWarehouse"));
	VisualPipes = MakeISM(TEXT("VisualPipes"));
	VisualRoofs = MakeISM(TEXT("VisualRoofs"));
	VisualVehicles = MakeISM(TEXT("VisualVehicles"));
	VisualFoliage = MakeISM(TEXT("VisualFoliage"));
	VisualSigns = MakeISM(TEXT("VisualSigns"));
	VisualBackdrop = MakeISM(TEXT("VisualBackdrop"));
	VisualWater = MakeISM(TEXT("VisualWater"));
	VisualFacades = MakeISM(TEXT("VisualFacades"));
	VisualMilitary = MakeISM(TEXT("VisualMilitary"));
	VisualDomes = MakeISM(TEXT("VisualDomes"));
}

void ANexusEnvironmentDirector::ConfigureVisualISM(UInstancedStaticMeshComponent* ISM)
{
	if (!ISM)
	{
		return;
	}
	ISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ISM->SetCanEverAffectNavigation(false);
	ISM->SetCastShadow(true);
	ISM->SetCullDistances(2500, 12000);
}

void ANexusEnvironmentDirector::BeginPlay()
{
	Super::BeginPlay();
	ApplyArtPass();
}

void ANexusEnvironmentDirector::Ensure(UWorld* World)
{
	if (!World)
	{
		return;
	}
	for (TActorIterator<ANexusEnvironmentDirector> It(World); It; ++It)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	World->SpawnActor<ANexusEnvironmentDirector>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
}

void ANexusEnvironmentDirector::ApplyArtPass()
{
	if (bArtApplied || !GetWorld())
	{
		return;
	}
	ActiveMap = FNexusMapCatalog::DetectFromWorld(GetWorld());
	if (PlaneMesh)
	{
		VisualGround->SetStaticMesh(PlaneMesh);
	}
	else if (CubeMesh)
	{
		VisualGround->SetStaticMesh(CubeMesh);
	}
	if (CubeMesh)
	{
		VisualWindows->SetStaticMesh(CubeMesh);
		VisualTrim->SetStaticMesh(CubeMesh);
		VisualProps->SetStaticMesh(CubeMesh);
		VisualMarketCloth->SetStaticMesh(CubeMesh);
		VisualMarketGoods->SetStaticMesh(CubeMesh);
		VisualWarehouse->SetStaticMesh(CubeMesh);
		VisualVehicles->SetStaticMesh(CubeMesh);
		VisualSigns->SetStaticMesh(CubeMesh);
		VisualBackdrop->SetStaticMesh(CubeMesh);
		VisualFacades->SetStaticMesh(CubeMesh);
		VisualMilitary->SetStaticMesh(CubeMesh);
		VisualWater->SetStaticMesh(CubeMesh);
	}
	if (PlaneMesh)
	{
		VisualRoofs->SetStaticMesh(PlaneMesh);
	}
	else if (CubeMesh)
	{
		VisualRoofs->SetStaticMesh(CubeMesh);
	}
	if (SphereMesh)
	{
		VisualFoliage->SetStaticMesh(SphereMesh);
		VisualDomes->SetStaticMesh(SphereMesh);
	}
	else if (CubeMesh)
	{
		VisualFoliage->SetStaticMesh(CubeMesh);
		VisualDomes->SetStaticMesh(CubeMesh);
	}
	if (CylinderMesh)
	{
		VisualPipes->SetStaticMesh(CylinderMesh);
	}
	UNexusMaterialLibrary::ApplyToPrimitive(VisualWindows, ENexusArtSurface::Glass);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualTrim, ENexusArtSurface::Sandstone);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualProps, ENexusArtSurface::Wood);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualGround,
		(ActiveMap == ENexusMapId::Outpost) ? ENexusArtSurface::RoadDust : ENexusArtSurface::Asphalt);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualMarketCloth, ENexusArtSurface::Fabric);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualMarketGoods, ENexusArtSurface::Wood);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualWarehouse, ENexusArtSurface::DirtyConcrete);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualPipes, ENexusArtSurface::Metal);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualRoofs, ENexusArtSurface::Ceramic);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualVehicles, ENexusArtSurface::Metal);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualFoliage, ENexusArtSurface::Foliage);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualSigns, ENexusArtSurface::PaintedConcrete);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualBackdrop, ENexusArtSurface::Sandstone);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualWater, ENexusArtSurface::Water);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualFacades, ENexusArtSurface::Plaster);
	UNexusMaterialLibrary::ApplyToPrimitive(VisualDomes, ENexusArtSurface::Sandstone);
	if (UMaterialInstanceDynamic* Blue = UNexusMaterialLibrary::MakeSurfaceMID(VisualMilitary, ENexusArtSurface::PaintedConcrete))
	{
		const FLinearColor DustyBlue(0.22f, 0.38f, 0.55f);
		Blue->SetVectorParameterValue(TEXT("Color"), DustyBlue);
		Blue->SetVectorParameterValue(TEXT("BaseColor"), DustyBlue);
		VisualMilitary->SetMaterial(0, Blue);
	}

	ApplyGameplayMaterials();
	ApplyLightingAndFog();
	SpawnVisualModules();
	bArtApplied = true;
	UE_LOG(LogNexusArt, Log, TEXT("NEXUS %s visual wrap (primitives, continuous facades, NoCollision, gameplay unchanged) instances=%d"),
		FNexusMapCatalog::MapName(ActiveMap), CountVisualInstances());
}

void ANexusEnvironmentDirector::ApplyGameplayMaterials()
{
	ConcealGreyboxVisuals();
	for (TActorIterator<ANexusDestructibleSurface> It(GetWorld()); It; ++It)
	{
		It->RefreshVisual();
	}
}

void ANexusEnvironmentDirector::ConcealGreyboxVisuals()
{
	int32 Hidden = 0;
	for (TActorIterator<ANexusVelasqoBuilder> It(GetWorld()); It; ++It)
	{
		for (UInstancedStaticMeshComponent* ISM : It->MaterialMeshes)
		{
			if (!ISM)
			{
				continue;
			}
			ISM->SetVisibility(false, true);
			ISM->SetHiddenInGame(true);
			ISM->SetCastShadow(false);
			ISM->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			UNexusMaterialLibrary::ApplyToPrimitive(ISM, ENexusArtSurface::DirtyConcrete);
			++Hidden;
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.0 concealed greybox ISM layers=%d (collision+nav kept, render off)"), Hidden);
}

void ANexusEnvironmentDirector::SpawnPavementGrid()
{
	const bool bPlane = PlaneMesh && VisualGround && VisualGround->GetStaticMesh() == PlaneMesh;
	const float Extent = (ActiveMap == ENexusMapId::Outpost) ? 140.f : ((ActiveMap == ENexusMapId::Skyline) ? 130.f : 120.f);
	if (bPlane)
	{
		AddVisualInstance(VisualGround, NexusMeters(FVector(0.f, 0.f, 0.02f)), FVector(Extent, Extent, 1.f));
	}
	else
	{
		AddVisualInstance(VisualGround, NexusMeters(FVector(0.f, 0.f, 0.02f)), FVector(Extent, Extent, 0.05f));
	}
	if (ActiveMap == ENexusMapId::Velasqo)
	{
		AddVisualInstance(VisualBackdrop, NexusMeters(FVector(22.f, 15.f, 0.05f)), FVector(22.f, 20.f, 0.05f));
		AddVisualInstance(VisualGround, NexusMeters(FVector(-18.f, -17.f, 0.04f)),
			bPlane ? FVector(23.f, 20.f, 1.f) : FVector(23.f, 20.f, 0.06f));
		AddVisualInstance(VisualProps, NexusMeters(FVector(-10.f, 10.f, 0.045f)), FVector(22.f, 12.f, 0.04f));
		AddVisualInstance(VisualTrim, NexusMeters(FVector(0.f, -25.f, 0.04f)), FVector(84.f, 22.f, 0.04f));
	}
	else if (ActiveMap == ENexusMapId::Skyline)
	{
		AddVisualInstance(VisualWarehouse, NexusMeters(FVector(0.f, 2.f, 0.04f)), FVector(22.f, 22.f, 0.04f));
		AddVisualInstance(VisualWarehouse, NexusMeters(FVector(26.f, -18.f, 0.04f)), FVector(26.f, 20.f, 0.04f));
	}
	else
	{
		AddVisualInstance(VisualTrim, NexusMeters(FVector(-30.f, 24.f, 0.04f)), FVector(28.f, 22.f, 0.05f));
		AddVisualInstance(VisualWarehouse, NexusMeters(FVector(32.f, -8.f, 0.04f)), FVector(24.f, 18.f, 0.05f));
	}
	UE_LOG(LogNexusArt, Log, TEXT("%s street planes (continuous ground, no tile grid)"), FNexusMapCatalog::MapName(ActiveMap));
}

void ANexusEnvironmentDirector::PlaceWindowKit(const FVector& CenterMeters, bool bAlongY, float OutSign, int32& Windows, bool bDeepFacade)
{
	if (Windows >= 900)
	{
		return;
	}
	const int32 Variant = Windows % 3;
	const float W = (Variant == 0) ? 0.72f : (Variant == 1) ? 0.95f : 0.55f;
	const float H = (Variant == 0) ? 1.15f : (Variant == 1) ? 1.35f : 0.95f;
	const float Recess = bDeepFacade ? 0.18f : 0.06f;
	const float In = -Recess * OutSign;
	const float Face = 0.02f * OutSign;
	const float Out = (bDeepFacade ? 0.07f : 0.05f) * OutSign;
	if (bAlongY)
	{
		AddVisualInstance(VisualWarehouse, NexusMeters(CenterMeters + FVector(In, 0.f, 0.f)), FVector(bDeepFacade ? 0.12f : 0.08f, W + 0.18f, H + 0.22f));
		AddVisualInstance(VisualTrim, NexusMeters(CenterMeters + FVector(Face, 0.f, 0.f)), FVector(0.05f, W + 0.12f, H + 0.16f));
		AddVisualInstance(VisualWindows, NexusMeters(CenterMeters + FVector(In * 0.45f, 0.f, 0.02f)), FVector(0.03f, W, H));
		AddVisualInstance(VisualProps, NexusMeters(CenterMeters + FVector(Out, 0.f, -H * 0.52f)), FVector(0.14f, W + 0.16f, 0.06f));
		if (Variant == 2)
		{
			AddVisualInstance(VisualMarketCloth, NexusMeters(CenterMeters + FVector(Out + 0.02f * OutSign, -W * 0.42f, 0.f)), FVector(0.04f, 0.14f, H * 0.92f));
			AddVisualInstance(VisualMarketCloth, NexusMeters(CenterMeters + FVector(Out + 0.02f * OutSign, W * 0.42f, 0.f)), FVector(0.04f, 0.14f, H * 0.92f));
		}
	}
	else
	{
		AddVisualInstance(VisualWarehouse, NexusMeters(CenterMeters + FVector(0.f, In, 0.f)), FVector(W + 0.18f, bDeepFacade ? 0.12f : 0.08f, H + 0.22f));
		AddVisualInstance(VisualTrim, NexusMeters(CenterMeters + FVector(0.f, Face, 0.f)), FVector(W + 0.12f, 0.05f, H + 0.16f));
		AddVisualInstance(VisualWindows, NexusMeters(CenterMeters + FVector(0.f, In * 0.45f, 0.02f)), FVector(W, 0.03f, H));
		AddVisualInstance(VisualProps, NexusMeters(CenterMeters + FVector(0.f, Out, -H * 0.52f)), FVector(W + 0.16f, 0.14f, 0.06f));
		if (Variant == 2)
		{
			AddVisualInstance(VisualMarketCloth, NexusMeters(CenterMeters + FVector(-W * 0.42f, Out + 0.02f * OutSign, 0.f)), FVector(0.14f, 0.04f, H * 0.92f));
			AddVisualInstance(VisualMarketCloth, NexusMeters(CenterMeters + FVector(W * 0.42f, Out + 0.02f * OutSign, 0.f)), FVector(0.14f, 0.04f, H * 0.92f));
		}
	}
	++Windows;
}

void ANexusEnvironmentDirector::PlaceRoofParapet(const FVector& CenterMeters, const FVector& SizeMeters)
{
	const float Top = CenterMeters.Z + SizeMeters.Z * 0.5f;
	const float Hx = SizeMeters.X * 0.5f;
	const float Hy = SizeMeters.Y * 0.5f;
	const bool bPlaneRoof = PlaneMesh && VisualRoofs && VisualRoofs->GetStaticMesh() == PlaneMesh;
	AddVisualInstance(VisualRoofs, NexusMeters(FVector(CenterMeters.X, CenterMeters.Y, Top + 0.04f)),
		bPlaneRoof ? FVector(SizeMeters.X - 0.1f, SizeMeters.Y - 0.1f, 1.f) : FVector(SizeMeters.X - 0.1f, SizeMeters.Y - 0.1f, 0.08f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(CenterMeters.X, CenterMeters.Y + Hy - 0.06f, Top + 0.28f)), FVector(SizeMeters.X + 0.08f, 0.12f, 0.42f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(CenterMeters.X, CenterMeters.Y - Hy + 0.06f, Top + 0.28f)), FVector(SizeMeters.X + 0.08f, 0.12f, 0.42f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(CenterMeters.X + Hx - 0.06f, CenterMeters.Y, Top + 0.28f)), FVector(0.12f, SizeMeters.Y - 0.1f, 0.42f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(CenterMeters.X - Hx + 0.06f, CenterMeters.Y, Top + 0.28f)), FVector(0.12f, SizeMeters.Y - 0.1f, 0.42f));
}

void ANexusEnvironmentDirector::PlacePitchedRoof(const FVector& CenterMeters, const FVector& SizeMeters)
{
	const float PitchDeg = 32.f;
	const float Top = CenterMeters.Z + SizeMeters.Z * 0.5f;
	const float HalfY = SizeMeters.Y * 0.5f;
	const float Rise = FMath::Tan(FMath::DegreesToRadians(PitchDeg)) * (HalfY * 0.82f);
	const float MidZ = Top + Rise * 0.42f;
	const bool bPlaneRoof = PlaneMesh && VisualRoofs && VisualRoofs->GetStaticMesh() == PlaneMesh;
	const FVector RoofScale = bPlaneRoof
		? FVector(SizeMeters.X + 1.05f, SizeMeters.Y * 0.78f, 1.f)
		: FVector(SizeMeters.X + 1.05f, SizeMeters.Y * 0.78f, 0.07f);
	AddVisualInstance(VisualRoofs, NexusMeters(FVector(CenterMeters.X, CenterMeters.Y + HalfY * 0.28f, MidZ)), RoofScale, FRotator(0.f, 0.f, PitchDeg));
	AddVisualInstance(VisualRoofs, NexusMeters(FVector(CenterMeters.X, CenterMeters.Y - HalfY * 0.28f, MidZ)), RoofScale, FRotator(0.f, 0.f, -PitchDeg));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(CenterMeters.X, CenterMeters.Y, Top + Rise * 0.88f)), FVector(SizeMeters.X + 0.35f, 0.16f, 0.14f));
	const float Hx = SizeMeters.X * 0.5f;
	AddVisualInstance(VisualFacades, NexusMeters(FVector(CenterMeters.X + Hx - 0.04f, CenterMeters.Y, Top + Rise * 0.32f)), FVector(0.12f, SizeMeters.Y * 0.22f, Rise * 0.7f));
	AddVisualInstance(VisualFacades, NexusMeters(FVector(CenterMeters.X - Hx + 0.04f, CenterMeters.Y, Top + Rise * 0.32f)), FVector(0.12f, SizeMeters.Y * 0.22f, Rise * 0.7f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(CenterMeters.X + SizeMeters.X * 0.28f, CenterMeters.Y - SizeMeters.Y * 0.12f, Top + Rise + 0.55f)), FVector(0.28f, 0.28f, 1.05f));
}

void ANexusEnvironmentDirector::SkinBrokenFacade(const FVector& WallCenter, float Len, float Thick, float Height, float Bottom,
	bool bThinX, float OutSign, UInstancedStaticMeshComponent* Body, int32& Windows, int32& Modules, bool bTwoStory, bool bSkipOpenings)
{
	const bool bWh = (Body == VisualWarehouse.Get());
	const bool bSkyline = (ActiveMap == ENexusMapId::Skyline);
	const bool bOutpost = (ActiveMap == ENexusMapId::Outpost);
	const float Depth = bWh ? 0.14f : 0.10f;
	const float GroundH = FMath::Clamp(bWh ? 3.55f : (Height >= 4.6f ? 2.72f : Height * 0.62f), 1.8f, Height - 0.35f);
	const float Belt = bWh ? 0.22f : 0.16f;

	FVector BodyC = WallCenter;
	if (bThinX) { BodyC.X += OutSign * (Depth * 0.35f); }
	else { BodyC.Y += OutSign * (Depth * 0.35f); }
	AddVisualInstance(Body, NexusMeters(BodyC),
		bThinX ? FVector(Thick + Depth, Len, Height) : FVector(Len, Thick + Depth, Height));
	++Modules;

	FVector Plinth = WallCenter;
	Plinth.Z = Bottom + 0.42f;
	if (bThinX) { Plinth.X += OutSign * 0.12f; }
	else { Plinth.Y += OutSign * 0.12f; }
	AddVisualInstance(VisualTrim, NexusMeters(Plinth),
		bThinX ? FVector(Thick + 0.24f, Len + 0.04f, 0.84f) : FVector(Len + 0.04f, Thick + 0.24f, 0.84f));

	if (bTwoStory && Height >= 4.4f)
	{
		FVector BeltC = WallCenter;
		BeltC.Z = Bottom + GroundH;
		if (bThinX) { BeltC.X += OutSign * 0.16f; }
		else { BeltC.Y += OutSign * 0.16f; }
		AddVisualInstance(VisualTrim, NexusMeters(BeltC),
			bThinX ? FVector(Thick + 0.28f, Len + 0.08f, Belt) : FVector(Len + 0.08f, Thick + 0.28f, Belt));
	}

	FVector Cornice = WallCenter;
	Cornice.Z = Bottom + Height - 0.08f;
	if (bThinX) { Cornice.X += OutSign * 0.18f; }
	else { Cornice.Y += OutSign * 0.18f; }
	AddVisualInstance(VisualTrim, NexusMeters(Cornice),
		bThinX ? FVector(Thick + 0.32f, Len + 0.12f, 0.18f) : FVector(Len + 0.12f, Thick + 0.32f, 0.18f));

	if (Len >= 6.f)
	{
		for (float T : { -0.47f, 0.47f })
		{
			FVector Col = WallCenter;
			Col.Z = Bottom + Height * 0.48f;
			if (bThinX) { Col.Y += T * Len; Col.X += OutSign * (Thick * 0.5f + 0.12f); }
			else { Col.X += T * Len; Col.Y += OutSign * (Thick * 0.5f + 0.12f); }
			AddVisualInstance(VisualPipes, NexusMeters(Col), FVector(0.18f, 0.18f, Height * 0.92f));
		}
	}

	if (!bSkipOpenings && Height >= 2.4f && Len >= 3.2f)
	{
		const int32 Cols = FMath::Clamp(FMath::FloorToInt(Len / (bSkyline ? 2.8f : 2.5f)), 1, 10);
		const int32 Rows = (bTwoStory && Height >= 5.f) ? 2 : 1;
		for (int32 Row = 0; Row < Rows; ++Row)
		{
			for (int32 Col = 0; Col < Cols; ++Col)
			{
				if (Col == 0 || Col == Cols - 1)
				{
					continue;
				}
				FVector WC = WallCenter;
				const float Along = ((Col + 0.5f) / Cols - 0.5f) * Len;
				if (bThinX) { WC.Y += Along; WC.X += OutSign * 0.22f; }
				else { WC.X += Along; WC.Y += OutSign * 0.22f; }
				WC.Z = Bottom + (Row == 0 ? 1.42f : (GroundH + Belt + 1.1f));
				PlaceWindowKit(WC, bThinX, OutSign, Windows, bWh || bSkyline);
			}
		}
	}

	if (bSkyline && Height >= 3.5f && Len >= 4.f)
	{
		FVector Glass = WallCenter;
		Glass.Z = Bottom + Height * 0.55f;
		if (bThinX) { Glass.X += OutSign * 0.22f; }
		else { Glass.Y += OutSign * 0.22f; }
		AddVisualInstance(VisualWindows, NexusMeters(Glass),
			bThinX ? FVector(0.04f, Len * 0.82f, Height * 0.42f) : FVector(Len * 0.82f, 0.04f, Height * 0.42f));
		const int32 Mullions = FMath::Clamp(FMath::RoundToInt(Len / 3.2f), 2, 8);
		for (int32 i = 0; i < Mullions; ++i)
		{
			FVector M = Glass;
			const float Along = ((i + 0.5f) / Mullions - 0.5f) * Len * 0.8f;
			if (bThinX) { M.Y += Along; }
			else { M.X += Along; }
			AddVisualInstance(VisualPipes, NexusMeters(M), FVector(0.06f, 0.06f, Height * 0.5f));
		}
	}

	if ((bOutpost || bWh) && Len >= 5.f)
	{
		for (float ZOff : { Height * 0.32f, Height * 0.68f })
		{
			FVector Rib = WallCenter;
			Rib.Z = Bottom + ZOff;
			if (bThinX) { Rib.X += OutSign * (Thick * 0.5f + 0.1f); }
			else { Rib.Y += OutSign * (Thick * 0.5f + 0.1f); }
			AddVisualInstance(VisualPipes, NexusMeters(Rib),
				bThinX ? FVector(0.06f, Len * 0.92f, 0.06f) : FVector(Len * 0.92f, 0.06f, 0.06f),
				bThinX ? FRotator(0.f, 0.f, 90.f) : FRotator(90.f, 0.f, 0.f));
		}
	}

	if (ActiveMap == ENexusMapId::Velasqo && !bWh && !bSkipOpenings && Height >= 2.6f && Len >= 5.f)
	{
		FVector Aw = WallCenter;
		Aw.Z = Bottom + 2.45f;
		if (bThinX) { Aw.X += OutSign * 0.45f; }
		else { Aw.Y += OutSign * 0.45f; }
		AddVisualInstance(VisualMarketCloth, NexusMeters(Aw),
			bThinX ? FVector(0.9f, Len * 0.55f, 0.03f) : FVector(Len * 0.55f, 0.9f, 0.03f),
			bThinX ? FRotator(0.f, 0.f, OutSign * 24.f) : FRotator(OutSign * 24.f, 0.f, 0.f));
	}
}

void ANexusEnvironmentDirector::PlaceRoofKit(const FVector& RoofMeters, int32& Count)
{
	AddVisualInstance(VisualPipes, NexusMeters(RoofMeters + FVector(0.55f, 0.2f, 0.35f)), FVector(0.55f, 0.42f, 0.38f));
	AddVisualInstance(VisualPipes, NexusMeters(RoofMeters + FVector(-0.7f, -0.15f, 0.85f)), FVector(0.08f, 0.08f, 1.15f));
	AddVisualInstance(VisualWarehouse, NexusMeters(RoofMeters + FVector(0.1f, -0.55f, 0.22f)), FVector(0.7f, 0.45f, 0.28f));
	AddVisualInstance(VisualSigns, NexusMeters(RoofMeters + FVector(-0.2f, 0.6f, 0.12f)), FVector(0.9f, 0.55f, 0.06f));
	++Count;
}

void ANexusEnvironmentDirector::SkinBox(const FNexusBoxDef& Box, int32& Windows, int32& Modules)
{
	if (Box.Id == TEXT("Terrain"))
	{
		return;
	}

	const FString Id = Box.Id.ToString();
	const FVector C = Box.CenterMeters;
	const FVector S = Box.SizeMeters;
	const float MinXY = FMath::Min(S.X, S.Y);
	const float MaxXY = FMath::Max(S.X, S.Y);
	const bool bIndustrial = Id.StartsWith(TEXT("B_")) || Id.Contains(TEXT("Wh"));
	const bool bBlue = Id.StartsWith(TEXT("BH"));
	const bool bEast = Id.StartsWith(TEXT("EH"));
	const bool bOldQ = Id.StartsWith(TEXT("VQ_")) || Id.StartsWith(TEXT("Alley"));
	const bool bPlace = Id.StartsWith(TEXT("A_"));
	UInstancedStaticMeshComponent* Body = VisualFacades;
	if (bIndustrial) { Body = VisualWarehouse; }
	else if (bBlue) { Body = VisualMilitary; }
	else if (bOldQ) { Body = VisualBackdrop; }
	else if (bEast) { Body = VisualFacades; }
	if (Id.StartsWith(TEXT("Mkt")) || Box.Material == ENexusMaterialType::Wood)
	{
		Body = VisualProps;
	}
	if (Id.StartsWith(TEXT("TW_")) || Id.StartsWith(TEXT("OFW_")) || Id.StartsWith(TEXT("OFE_")) || Id.StartsWith(TEXT("PK_"))
		|| Id.StartsWith(TEXT("PLZ_")) || Id.StartsWith(TEXT("BLK_")) || Id.StartsWith(TEXT("CHI")) || Id.StartsWith(TEXT("Chi")))
	{
		Body = VisualWarehouse;
	}
	if (Id.StartsWith(TEXT("HG_")) || Id.StartsWith(TEXT("WH_")) || Id.StartsWith(TEXT("ZC_")) || Id.StartsWith(TEXT("TG_"))
		|| Id.StartsWith(TEXT("CN_")))
	{
		Body = VisualMilitary;
	}
	if (Box.Material == ENexusMaterialType::Glass)
	{
		AddVisualInstance(VisualWindows, NexusMeters(C), FVector(S.X, S.Y, S.Z));
		++Modules;
		return;
	}

	const bool bColumn = (MinXY >= 0.25f && MaxXY <= 1.15f && S.Z >= 2.2f && FMath::Abs(S.X - S.Y) < 0.35f);
	if (bColumn || Id.Contains(TEXT("_Col")) || Id.Contains(TEXT("_Pil")) || Id.Contains(TEXT("_Leg")) || Id.Contains(TEXT("ShedP")))
	{
		PlaceColumn(C, S);
		++Modules;
		return;
	}
	if (Id.StartsWith(TEXT("CN_")))
	{
		PlaceContainerKit(C, S);
		++Modules;
		return;
	}
	if (Id.StartsWith(TEXT("TRK_")) || Id.StartsWith(TEXT("PK_Car")))
	{
		PlaceVehicleKit(C, S);
		++Modules;
		return;
	}
	if (Id.Contains(TEXT("Fuel")))
	{
		PlaceTankKit(C, S);
		++Modules;
		return;
	}

	if (Box.Usage == ENexusBoxUsage::Floor)
	{
		if (Id == TEXT("Terrain") || MaxXY > 40.f)
		{
			return;
		}
		if (S.Z < 0.45f && MaxXY > 6.f)
		{
			if (Id.Contains(TEXT("Roof")) || Id.Contains(TEXT("Slab")) || Id.Contains(TEXT("Canopy")) || Id.Contains(TEXT("Deck")))
			{
				PlaceRoofParapet(C, S);
				++Modules;
			}
			return;
		}
		AddVisualInstance(Box.Material == ENexusMaterialType::Wood ? VisualProps : VisualWarehouse,
			NexusMeters(C + FVector(0.f, 0.f, 0.02f)), FVector(S.X, S.Y, FMath::Max(S.Z, 0.08f)));
		++Modules;
		return;
	}

	if (Box.Usage == ENexusBoxUsage::Stairs)
	{
		AddVisualInstance(VisualProps, NexusMeters(C), FVector(S.X, S.Y, S.Z));
		++Modules;
		return;
	}

	if (Box.Usage == ENexusBoxUsage::Cover)
	{
		if (Id.Contains(TEXT("Fountain")) && !Id.Contains(TEXT("Wall")))
		{
			return;
		}
		if (Id.Contains(TEXT("Fuel")))
		{
			PlaceTankKit(C, S);
			++Modules;
			return;
		}
		if (Id.StartsWith(TEXT("PK_Car")) || Id.StartsWith(TEXT("TRK_")))
		{
			PlaceVehicleKit(C, S);
			++Modules;
			return;
		}
		if (Id.Contains(TEXT("FountainWall")) || Id.Contains(TEXT("GalRail")) || Id.Contains(TEXT("WindowCover")) || Id.Contains(TEXT("Rail")))
		{
			AddVisualInstance(VisualTrim, NexusMeters(C), FVector(S.X, S.Y, S.Z));
			++Modules;
			return;
		}
		if (Id.StartsWith(TEXT("Mkt_Stand")) || Box.Material == ENexusMaterialType::Wood)
		{
			AddVisualInstance(VisualProps, NexusMeters(C + FVector(0.f, 0.f, -S.Z * 0.15f)), FVector(S.X, S.Y, S.Z * 0.55f));
			AddVisualInstance(VisualProps, NexusMeters(C + FVector(0.f, 0.f, S.Z * 0.28f)), FVector(S.X * 0.92f, S.Y * 0.55f, 0.08f));
			AddVisualInstance(VisualMarketCloth, NexusMeters(C + FVector(0.f, 0.f, S.Z * 0.55f)), FVector(S.X * 1.05f, S.Y * 0.2f, 0.06f));
			++Modules;
			return;
		}
		AddVisualInstance(VisualWarehouse, NexusMeters(C), FVector(S.X * 0.98f, S.Y * 0.92f, S.Z * 0.88f));
		AddVisualInstance(VisualTrim, NexusMeters(C + FVector(0.f, 0.f, S.Z * 0.48f)), FVector(S.X * 1.02f, S.Y * 0.98f, 0.1f));
		++Modules;
		return;
	}

	const bool bPerimeter = Id.StartsWith(TEXT("Chi")) || Id.StartsWith(TEXT("Perim")) || Id.StartsWith(TEXT("CHI"));
	if (MinXY <= 0.65f && MaxXY >= 1.2f && S.Z >= 1.6f)
	{
		const bool bThinX = S.X < S.Y;
		const float Len = bThinX ? S.Y : S.X;
		const float Thick = bThinX ? S.X : S.Y;
		const float Bottom = C.Z - S.Z * 0.5f;
		const float OutSign = bThinX ? ((C.X >= 0.f) ? 1.f : -1.f) : ((C.Y >= 0.f) ? 1.f : -1.f);
		SkinBrokenFacade(C, Len, Thick, S.Z, Bottom, bThinX, OutSign, Body, Windows, Modules, S.Z >= 5.f, bPerimeter);
		if (Id == TEXT("BH_South"))
		{
			PlacePitchedRoof(FVector(-8.f, 2.f, 3.f), FVector(12.f, 9.f, 6.f));
		}
		return;
	}

	if (MinXY >= 1.6f && S.Z >= 2.4f)
	{
		const float T = 0.18f;
		const float Hx = S.X * 0.5f;
		const float Hy = S.Y * 0.5f;
		const float Bottom = C.Z - S.Z * 0.5f;
		SkinBrokenFacade(FVector(C.X, C.Y + Hy - T * 0.5f, C.Z), S.X, T, S.Z, Bottom, false, 1.f, Body, Windows, Modules, S.Z >= 5.f, false);
		SkinBrokenFacade(FVector(C.X, C.Y - Hy + T * 0.5f, C.Z), S.X, T, S.Z, Bottom, false, -1.f, Body, Windows, Modules, S.Z >= 5.f, false);
		SkinBrokenFacade(FVector(C.X + Hx - T * 0.5f, C.Y, C.Z), S.Y - T * 2.f, T, S.Z, Bottom, true, 1.f, Body, Windows, Modules, S.Z >= 5.f, false);
		SkinBrokenFacade(FVector(C.X - Hx + T * 0.5f, C.Y, C.Z), S.Y - T * 2.f, T, S.Z, Bottom, true, -1.f, Body, Windows, Modules, S.Z >= 5.f, false);
		if (bOldQ || bBlue)
		{
			PlacePitchedRoof(C, S);
		}
		else
		{
			PlaceRoofParapet(C, S);
		}
		return;
	}

	if (bOldQ || bBlue || bPlace || bPerimeter)
	{
		return;
	}
	if (S.Z < 1.6f && MinXY < 0.5f)
	{
		AddVisualInstance(VisualTrim, NexusMeters(C), FVector(S.X, S.Y, S.Z));
		++Modules;
		return;
	}
}

void ANexusEnvironmentDirector::RebuildArchitecture(const FNexusVelasqoLayout& Layout)
{
	SpawnPavementGrid();
	int32 Windows = 0;
	int32 Modules = 0;
	for (const FNexusBoxDef& Box : Layout.Boxes)
	{
		SkinBox(Box, Windows, Modules);
		if (Modules > 8000)
		{
			break;
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.2 rebuilt architecture modules=%d windows=%d (greybox collision unchanged)"), Modules, Windows);
}

void ANexusEnvironmentDirector::SpawnFountainArchitecture()
{
	AddVisualInstance(VisualTrim, NexusMeters(FVector(22.f, 16.f, 0.28f)), FVector(5.2f, 4.4f, 0.55f));
	AddVisualInstance(VisualBackdrop, NexusMeters(FVector(22.f, 16.f, 0.58f)), FVector(4.4f, 3.6f, 0.22f));
	AddVisualInstance(VisualWater, NexusMeters(FVector(22.f, 16.f, 0.74f)), FVector(3.7f, 3.0f, 0.1f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(22.f, 16.f, 1.15f)), FVector(0.85f, 0.85f, 1.45f));
	AddVisualInstance(VisualWater, NexusMeters(FVector(22.f, 16.f, 1.95f)), FVector(0.4f, 0.4f, 0.28f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(13.4f, 20.3f, 1.5f)), FVector(0.38f, 0.38f, 3.0f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(20.6f, 20.3f, 1.5f)), FVector(0.38f, 0.38f, 3.0f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(13.4f, 23.7f, 1.5f)), FVector(0.38f, 0.38f, 3.0f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(20.6f, 23.7f, 1.5f)), FVector(0.38f, 0.38f, 3.0f));
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 fountain basin+water (NoCollision overlay on existing cover)"));
}

void ANexusEnvironmentDirector::DressGameplayDoors()
{
	int32 N = 0;
	for (TActorIterator<ANexusDoor> It(GetWorld()); It; ++It)
	{
		if (It->Mesh)
		{
			const bool bMetal = It->DoorType == ENexusDoorType::Reinforced || It->DoorType == ENexusDoorType::Double
				|| It->DoorType == ENexusDoorType::Destructible;
			UNexusMaterialLibrary::ApplyToPrimitive(It->Mesh, bMetal ? ENexusArtSurface::Metal : ENexusArtSurface::Wood);
			const FVector Loc = It->GetActorLocation();
			const FVector Fwd = It->GetActorForwardVector();
			const FVector Frame = Loc - Fwd * 8.f + FVector(0.f, 0.f, 15.f);
			AddVisualInstance(VisualTrim, Frame, FVector(1.35f, 0.18f, 2.35f), It->GetActorRotation());
			++N;
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.2 dressed existing Nexus doors=%d (same actors + visual frame)"), N);
}

void ANexusEnvironmentDirector::DressGameplayCover()
{
	int32 N = 0;
	for (TActorIterator<ANexusCover> It(GetWorld()); It; ++It)
	{
		if (!It->Mesh)
		{
			continue;
		}
		It->Mesh->SetVisibility(false, true);
		It->Mesh->SetHiddenInGame(true);
		It->Mesh->SetCastShadow(false);
		It->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		const FVector Loc = It->GetActorLocation();
		const FRotator Rot = It->GetActorRotation();
		const FVector Ext = It->Mesh->GetComponentScale();
		if (It->CoverType == ENexusCoverType::Low)
		{
			AddVisualInstance(VisualWarehouse, Loc - FVector(0.f, 0.f, Ext.Z * 20.f), FVector(Ext.X * 1.1f, Ext.Y * 0.85f, Ext.Z * 0.45f), Rot);
			AddVisualInstance(VisualFoliage, Loc + FVector(0.f, 0.f, Ext.Z * 15.f), FVector(Ext.X * 0.7f, Ext.Y * 0.55f, Ext.Z * 0.55f), Rot);
		}
		else if (It->CoverType == ENexusCoverType::High)
		{
			AddVisualInstance(VisualProps, Loc, FVector(Ext.X * 1.05f, Ext.Y * 0.95f, Ext.Z * 0.9f), Rot);
			AddVisualInstance(VisualTrim, Loc + FVector(0.f, 0.f, Ext.Z * 42.f), FVector(Ext.X * 1.08f, Ext.Y * 0.98f, 0.08f), Rot);
		}
		else
		{
			AddVisualInstance(VisualWarehouse, Loc, FVector(Ext.X * 1.15f, Ext.Y * 0.95f, Ext.Z * 0.82f), Rot);
			AddVisualInstance(VisualTrim, Loc + FVector(0.f, 0.f, Ext.Z * 38.f), FVector(Ext.X * 1.22f, Ext.Y * 1.02f, 0.1f), Rot);
		}
		++N;
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.2 dressed gameplay cover actors=%d (collision kept, cube mesh hidden)"), N);
}

void ANexusEnvironmentDirector::ApplyLightingAndFog()
{
	UWorld* World = GetWorld();
	const UNexusArtSettings* Art = GetDefault<UNexusArtSettings>();
	const FNexusTimeOfDayPreset& TOD = (ActiveMap == ENexusMapId::Skyline) ? Art->Night
		: Art->ActiveTimeOfDay();
	const FNexusFogPreset& Fog = (ActiveMap == ENexusMapId::Skyline) ? Art->Skyline_DefaultFog
		: ((ActiveMap == ENexusMapId::Outpost) ? Art->Outpost_DefaultFog : Art->Velasqo_DefaultFog);

	ADirectionalLight* Sun = nullptr;
	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		Sun = *It;
		break;
	}
	if (!Sun)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Sun = World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 2200.f), TOD.SunRotation, Params);
	}
	if (Sun)
	{
		Sun->SetActorRotation(TOD.SunRotation);
		if (ULightComponent* Light = Sun->GetLightComponent())
		{
			Light->SetIntensity(TOD.SunIntensity);
			Light->SetLightColor(TOD.SunColor);
			Light->SetCastShadows(true);
			Light->SetMobility(EComponentMobility::Movable);
		}
	}

	ASkyLight* Sky = nullptr;
	for (TActorIterator<ASkyLight> It(World); It; ++It)
	{
		Sky = *It;
		break;
	}
	if (!Sky)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Sky = World->SpawnActor<ASkyLight>(FVector(0.f, 0.f, 1800.f), FRotator::ZeroRotator, Params);
	}
	if (Sky)
	{
		if (USkyLightComponent* Comp = Sky->GetLightComponent())
		{
			Comp->SetIntensity(TOD.SkyIntensity);
			Comp->SetLightColor(TOD.SkyColor);
			Comp->SetMobility(EComponentMobility::Movable);
			Comp->RecaptureSky();
		}
	}

	AExponentialHeightFog* FogActor = nullptr;
	for (TActorIterator<AExponentialHeightFog> It(World); It; ++It)
	{
		FogActor = *It;
		break;
	}
	if (!FogActor)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FogActor = World->SpawnActor<AExponentialHeightFog>(FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator, Params);
	}
	if (FogActor)
	{
		if (UExponentialHeightFogComponent* Comp = FogActor->GetComponent())
		{
			Comp->SetFogDensity(Fog.FogDensity);
			Comp->SetFogHeightFalloff(Fog.FogHeightFalloff);
			Comp->SetFogInscatteringColor(Fog.FogInscattering);
			Comp->SetFogMaxOpacity(Fog.FogMaxOpacity);
			Comp->SetStartDistance(Fog.FogStartDistance);
			Comp->SetVolumetricFog(false);
		}
	}

	bool bHasSkyAtmo = false;
	for (TActorIterator<ASkyAtmosphere> It(World); It; ++It)
	{
		bHasSkyAtmo = true;
		break;
	}
	if (!bHasSkyAtmo)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<ASkyAtmosphere>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	}
}

void ANexusEnvironmentDirector::AddVisualInstance(UInstancedStaticMeshComponent* ISM, const FVector& Loc, const FVector& Scale, const FRotator& Rot)
{
	if (!ISM)
	{
		return;
	}
	ISM->AddInstance(FTransform(Rot, Loc, Scale), false);
}

void ANexusEnvironmentDirector::PlaceColumn(const FVector& CenterMeters, const FVector& SizeMeters)
{
	const float D = FMath::Max(FMath::Max(SizeMeters.X, SizeMeters.Y), 0.32f);
	AddVisualInstance(VisualPipes, NexusMeters(CenterMeters), FVector(D, D, SizeMeters.Z));
}

void ANexusEnvironmentDirector::PlaceVehicleKit(const FVector& CenterMeters, const FVector& SizeMeters, const FRotator& Rot)
{
	const FQuat Q = Rot.Quaternion();
	auto At = [&](const FVector& Local) { return CenterMeters + Q.RotateVector(Local); };
	AddVisualInstance(VisualVehicles, NexusMeters(At(FVector(0.f, 0.f, -SizeMeters.Z * 0.12f))),
		FVector(SizeMeters.X, SizeMeters.Y * 0.9f, SizeMeters.Z * 0.42f), Rot);
	AddVisualInstance(VisualVehicles, NexusMeters(At(FVector(-SizeMeters.X * 0.16f, 0.f, SizeMeters.Z * 0.18f))),
		FVector(SizeMeters.X * 0.42f, SizeMeters.Y * 0.84f, SizeMeters.Z * 0.5f), Rot);
	AddVisualInstance(VisualWindows, NexusMeters(At(FVector(-SizeMeters.X * 0.12f, 0.f, SizeMeters.Z * 0.32f))),
		FVector(SizeMeters.X * 0.22f, SizeMeters.Y * 0.78f, SizeMeters.Z * 0.16f), Rot);
	const float Wx = SizeMeters.X * 0.3f;
	const float Wy = SizeMeters.Y * 0.46f;
	const FRotator WheelRot(90.f, Rot.Yaw, 0.f);
	for (int32 Sx = -1; Sx <= 1; Sx += 2)
	{
		for (int32 Sy = -1; Sy <= 1; Sy += 2)
		{
			AddVisualInstance(VisualPipes, NexusMeters(At(FVector(Sx * Wx, Sy * Wy, -SizeMeters.Z * 0.32f))),
				FVector(0.55f, 0.55f, SizeMeters.Y * 0.16f), WheelRot);
		}
	}
}

void ANexusEnvironmentDirector::PlaceContainerKit(const FVector& CenterMeters, const FVector& SizeMeters)
{
	AddVisualInstance(VisualMilitary, NexusMeters(CenterMeters), SizeMeters);
	const bool bLongX = SizeMeters.X >= SizeMeters.Y;
	const int32 Ribs = 5;
	for (int32 i = 0; i < Ribs; ++i)
	{
		const float T = ((i + 0.5f) / Ribs - 0.5f) * (bLongX ? SizeMeters.X : SizeMeters.Y) * 0.86f;
		FVector R = CenterMeters;
		if (bLongX) { R.X += T; R.Y += SizeMeters.Y * 0.52f; }
		else { R.Y += T; R.X += SizeMeters.X * 0.52f; }
		AddVisualInstance(VisualPipes, NexusMeters(R), FVector(0.07f, 0.07f, SizeMeters.Z * 0.9f));
	}
	FVector Door = CenterMeters;
	if (bLongX) { Door.X += SizeMeters.X * 0.5f; }
	else { Door.Y += SizeMeters.Y * 0.5f; }
	AddVisualInstance(VisualTrim, NexusMeters(Door),
		bLongX ? FVector(0.08f, SizeMeters.Y * 0.92f, SizeMeters.Z * 0.92f) : FVector(SizeMeters.X * 0.92f, 0.08f, SizeMeters.Z * 0.92f));
}

void ANexusEnvironmentDirector::PlaceTankKit(const FVector& CenterMeters, const FVector& SizeMeters)
{
	const float D = FMath::Max(SizeMeters.X, SizeMeters.Y);
	AddVisualInstance(VisualPipes, NexusMeters(CenterMeters), FVector(D, D, SizeMeters.Z));
	AddVisualInstance(VisualPipes, NexusMeters(CenterMeters + FVector(0.f, 0.f, SizeMeters.Z * 0.52f)), FVector(D * 0.35f, D * 0.35f, 0.45f));
	AddVisualInstance(VisualPipes, NexusMeters(CenterMeters + FVector(D * 0.42f, 0.f, SizeMeters.Z * 0.2f)),
		FVector(0.12f, 0.12f, D * 0.55f), FRotator(0.f, 0.f, 90.f));
}

void ANexusEnvironmentDirector::PlaceBackdropTower(const FVector& CenterMeters, const FVector& SizeMeters, bool bGlass)
{
	AddVisualInstance(bGlass ? VisualWarehouse : VisualBackdrop, NexusMeters(CenterMeters), SizeMeters);
	AddVisualInstance(VisualWindows, NexusMeters(CenterMeters + FVector(0.f, -SizeMeters.Y * 0.52f, SizeMeters.Z * 0.08f)),
		FVector(SizeMeters.X * 0.78f, 0.06f, SizeMeters.Z * 0.62f));
	PlaceRoofParapet(CenterMeters, SizeMeters);
	AddVisualInstance(VisualPipes, NexusMeters(CenterMeters + FVector(SizeMeters.X * 0.22f, SizeMeters.Y * 0.1f, SizeMeters.Z * 0.55f)),
		FVector(0.18f, 0.18f, SizeMeters.Z * 0.35f));
}

void ANexusEnvironmentDirector::PlaceMosqueKit(const FVector& C)
{
	AddVisualInstance(VisualBackdrop, NexusMeters(C + FVector(0.f, 0.f, 2.4f)), FVector(8.4f, 8.4f, 4.8f));
	AddVisualInstance(VisualTrim, NexusMeters(C + FVector(0.f, 0.f, 4.9f)), FVector(9.0f, 9.0f, 0.4f));
	AddVisualInstance(VisualDomes, NexusMeters(C + FVector(0.f, 0.f, 6.9f)), FVector(7.4f, 7.4f, 4.4f));
	AddVisualInstance(VisualTrim, NexusMeters(C + FVector(0.f, 0.f, 9.1f)), FVector(1.15f, 1.15f, 0.85f));
	AddVisualInstance(VisualDomes, NexusMeters(C + FVector(-3.6f, 3.2f, 5.4f)), FVector(3.2f, 3.2f, 2.0f));
	AddVisualInstance(VisualWindows, NexusMeters(C + FVector(0.f, -4.25f, 1.9f)), FVector(2.1f, 0.14f, 3.4f));
	AddVisualInstance(VisualTrim, NexusMeters(C + FVector(-1.35f, -4.32f, 1.7f)), FVector(0.3f, 0.22f, 3.4f));
	AddVisualInstance(VisualTrim, NexusMeters(C + FVector(1.35f, -4.32f, 1.7f)), FVector(0.3f, 0.22f, 3.4f));
	AddVisualInstance(VisualTrim, NexusMeters(C + FVector(0.f, -4.32f, 3.5f)), FVector(3.0f, 0.22f, 0.35f));
	const FVector Minaret = C + FVector(5.8f, 5.4f, 0.f);
	AddVisualInstance(VisualPipes, NexusMeters(Minaret + FVector(0.f, 0.f, 8.0f)), FVector(1.5f, 1.5f, 16.f));
	AddVisualInstance(VisualTrim, NexusMeters(Minaret + FVector(0.f, 0.f, 13.4f)), FVector(2.5f, 2.5f, 0.4f));
	AddVisualInstance(VisualPipes, NexusMeters(Minaret + FVector(0.f, 0.f, 16.4f)), FVector(0.42f, 0.42f, 3.4f));
	AddVisualInstance(VisualTrim, NexusMeters(Minaret + FVector(0.f, 0.f, 18.2f)), FVector(0.85f, 0.85f, 0.7f));
	auto Palm = [&](const FVector& P)
	{
		AddVisualInstance(VisualPipes, NexusMeters(P + FVector(0.f, 0.f, 2.1f)), FVector(0.28f, 0.28f, 4.2f));
		AddVisualInstance(VisualFoliage, NexusMeters(P + FVector(0.f, 0.f, 4.5f)), FVector(2.2f, 2.2f, 1.5f));
	};
	Palm(C + FVector(-6.6f, 1.4f, 0.f));
	Palm(C + FVector(6.6f, -2.8f, 0.f));
	Palm(C + FVector(-4.8f, -5.2f, 0.f));
}

void ANexusEnvironmentDirector::PlaceHelipadKit(const FVector& C)
{
	const bool bPlane = PlaneMesh && VisualRoofs && VisualRoofs->GetStaticMesh() == PlaneMesh;
	AddVisualInstance(VisualSigns, NexusMeters(C), bPlane ? FVector(9.2f, 9.2f, 1.f) : FVector(9.2f, 9.2f, 0.05f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(0.f, 0.f, 0.04f)), FVector(8.2f, 8.2f, 0.04f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(0.f, 0.f, 0.05f)), FVector(6.8f, 6.8f, 0.03f));
	AddVisualInstance(VisualSigns, NexusMeters(C + FVector(0.f, 0.f, 0.07f)), FVector(2.6f, 0.4f, 0.04f));
	AddVisualInstance(VisualSigns, NexusMeters(C + FVector(0.f, 0.f, 0.07f)), FVector(0.4f, 2.8f, 0.04f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(3.8f, 3.8f, 0.55f)), FVector(0.14f, 0.14f, 1.1f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(-3.8f, 3.8f, 0.55f)), FVector(0.14f, 0.14f, 1.1f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(3.8f, -3.8f, 0.55f)), FVector(0.14f, 0.14f, 1.1f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(-3.8f, -3.8f, 0.55f)), FVector(0.14f, 0.14f, 1.1f));
}

void ANexusEnvironmentDirector::PlaceHeliKit(const FVector& C, const FRotator& Rot)
{
	const FQuat Q = Rot.Quaternion();
	auto At = [&](const FVector& L) { return C + Q.RotateVector(L); };
	AddVisualInstance(VisualVehicles, NexusMeters(At(FVector(0.f, 0.f, 0.55f))), FVector(4.4f, 1.8f, 1.15f), Rot);
	AddVisualInstance(VisualWindows, NexusMeters(At(FVector(0.8f, 0.f, 0.85f))), FVector(1.4f, 1.6f, 0.55f), Rot);
	AddVisualInstance(VisualPipes, NexusMeters(At(FVector(-3.2f, 0.f, 0.7f))), FVector(0.22f, 0.22f, 3.4f), Rot + FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualPipes, NexusMeters(At(FVector(0.2f, 0.f, 1.45f))), FVector(4.8f, 4.8f, 0.06f));
	AddVisualInstance(VisualPipes, NexusMeters(At(FVector(-3.4f, 0.f, 1.15f))), FVector(1.4f, 1.4f, 0.05f));
	AddVisualInstance(VisualPipes, NexusMeters(At(FVector(0.6f, 1.05f, 0.15f))), FVector(0.55f, 0.18f, 0.18f), Rot);
	AddVisualInstance(VisualPipes, NexusMeters(At(FVector(0.6f, -1.05f, 0.15f))), FVector(0.55f, 0.18f, 0.18f), Rot);
}

void ANexusEnvironmentDirector::PlaceWatchtowerKit(const FVector& C)
{
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(-1.6f, -1.6f, 3.4f)), FVector(0.28f, 0.28f, 6.8f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(1.6f, -1.6f, 3.4f)), FVector(0.28f, 0.28f, 6.8f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(-1.6f, 1.6f, 3.4f)), FVector(0.28f, 0.28f, 6.8f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(1.6f, 1.6f, 3.4f)), FVector(0.28f, 0.28f, 6.8f));
	AddVisualInstance(VisualMilitary, NexusMeters(C + FVector(0.f, 0.f, 7.15f)), FVector(4.4f, 4.4f, 2.0f));
	AddVisualInstance(VisualWindows, NexusMeters(C + FVector(0.f, -2.25f, 7.2f)), FVector(3.0f, 0.08f, 1.05f));
	AddVisualInstance(VisualWindows, NexusMeters(C + FVector(0.f, 2.25f, 7.2f)), FVector(3.0f, 0.08f, 1.05f));
	AddVisualInstance(VisualWindows, NexusMeters(C + FVector(-2.25f, 0.f, 7.2f)), FVector(0.08f, 3.0f, 1.05f));
	AddVisualInstance(VisualTrim, NexusMeters(C + FVector(0.f, 0.f, 8.3f)), FVector(4.8f, 4.8f, 0.18f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(0.f, 0.f, 10.1f)), FVector(0.16f, 0.16f, 2.8f));
	AddVisualInstance(VisualPipes, NexusMeters(C + FVector(0.f, 0.f, 11.5f)), FVector(2.8f, 0.08f, 2.8f), FRotator(0.f, 0.f, 48.f));
}

void ANexusEnvironmentDirector::SpawnVisualModules()
{
	const FNexusVelasqoLayout Layout = FNexusMapCatalog::BuildLayout(ActiveMap);
	RebuildArchitecture(Layout);
	DressGameplayDoors();
	DressGameplayCover();
	if (ActiveMap == ENexusMapId::Skyline)
	{
		SpawnSkylineDress(Layout);
	}
	else if (ActiveMap == ENexusMapId::Outpost)
	{
		SpawnOutpostDress(Layout);
	}
	else
	{
		SpawnFountainArchitecture();
		SpawnMarketProps(Layout);
		SpawnWarehouseProps(Layout);
		SpawnCityProduction(Layout);
		SpawnVelasqoGddSpots(Layout);
	}

	UE_LOG(LogNexusArt, Log, TEXT("%s architecture rebuild complete totalISM=%d"), FNexusMapCatalog::MapName(ActiveMap), CountVisualInstances());
	auto Dirty = [](UInstancedStaticMeshComponent* ISM)
	{
		if (ISM) { ISM->MarkRenderStateDirty(); }
	};
	Dirty(VisualWindows); Dirty(VisualTrim); Dirty(VisualProps); Dirty(VisualGround);
	Dirty(VisualMarketCloth); Dirty(VisualMarketGoods); Dirty(VisualWarehouse); Dirty(VisualPipes);
	Dirty(VisualRoofs); Dirty(VisualVehicles); Dirty(VisualFoliage); Dirty(VisualSigns);
	Dirty(VisualBackdrop); Dirty(VisualWater); Dirty(VisualFacades); Dirty(VisualMilitary); Dirty(VisualDomes);
	FNexusVelasqoValidation::Audit(GetWorld());
}

bool ANexusEnvironmentDirector::IsGameplaySensitive(const FNexusVelasqoLayout& Layout, const FVector& Meters) const
{
	for (const FNexusSiteDef& Site : Layout.Sites)
	{
		for (const FNexusPlantZoneDef& Zone : Site.PlantZones)
		{
			if (FVector::Dist2D(Meters, Zone.CenterMeters) < 2.2f)
			{
				return true;
			}
		}
		for (const FVector& Ent : Site.EntranceCentersMeters)
		{
			if (FVector::Dist2D(Meters, Ent) < 2.5f)
			{
				return true;
			}
		}
	}
	auto DistSeg = [](const FVector& P, const FVector& A, const FVector& B) -> float
	{
		const FVector2D AP(P.X - A.X, P.Y - A.Y);
		const FVector2D AB(B.X - A.X, B.Y - A.Y);
		const float LenSq = AB.SizeSquared();
		const float T = (LenSq > KINDA_SMALL_NUMBER) ? FMath::Clamp(FVector2D::DotProduct(AP, AB) / LenSq, 0.f, 1.f) : 0.f;
		const FVector2D Closest = FVector2D(A.X, A.Y) + AB * T;
		return FVector2D::Distance(FVector2D(P.X, P.Y), Closest);
	};
	for (const FNexusRouteDef& Route : Layout.Routes)
	{
		for (int32 i = 0; i + 1 < Route.PointsMeters.Num(); ++i)
		{
			if (DistSeg(Meters, Route.PointsMeters[i], Route.PointsMeters[i + 1]) < 1.6f)
			{
				return true;
			}
		}
	}
	return false;
}

void ANexusEnvironmentDirector::SpawnMarketProps(const FNexusVelasqoLayout& Layout)
{
	int32 Count = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& ScaleUu, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (IsGameplaySensitive(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), ScaleUu, Rot);
		++Count;
	};

	// Awnings sit under the existing market roof (Z=3.5m), above standing eye height.
	Place(VisualMarketCloth, FVector(-16.f, 8.f, 3.15f), FVector(2.6f, 1.4f, 0.03f));
	Place(VisualMarketCloth, FVector(-10.f, 12.5f, 3.15f), FVector(2.6f, 1.4f, 0.03f));
	Place(VisualMarketCloth, FVector(-5.f, 8.f, 3.15f), FVector(2.6f, 1.4f, 0.03f));
	Place(VisualMarketCloth, FVector(-14.f, 10.f, 3.18f), FVector(3.2f, 0.9f, 0.025f));
	Place(VisualMarketCloth, FVector(-6.f, 12.f, 3.18f), FVector(2.8f, 0.8f, 0.025f));
	Place(VisualMarketCloth, FVector(-16.f, 8.f, 2.55f), FVector(2.5f, 0.08f, 0.55f));
	Place(VisualMarketCloth, FVector(-10.f, 12.5f, 2.55f), FVector(2.5f, 0.08f, 0.55f));
	Place(VisualMarketCloth, FVector(-5.f, 8.f, 2.55f), FVector(2.5f, 0.08f, 0.55f));

	Place(VisualMarketGoods, FVector(-16.3f, 8.f, 1.05f), FVector(0.45f, 0.35f, 0.28f));
	Place(VisualMarketGoods, FVector(-15.6f, 8.15f, 1.02f), FVector(0.22f, 0.22f, 0.35f));
	Place(VisualMarketGoods, FVector(-15.2f, 7.7f, 1.12f), FVector(0.18f, 0.32f, 0.42f));
	Place(VisualMarketGoods, FVector(-10.4f, 12.5f, 1.05f), FVector(0.5f, 0.32f, 0.22f));
	Place(VisualMarketGoods, FVector(-9.6f, 12.35f, 1.08f), FVector(0.28f, 0.28f, 0.4f));
	Place(VisualMarketGoods, FVector(-9.2f, 12.7f, 1.0f), FVector(0.2f, 0.2f, 0.25f));
	Place(VisualMarketGoods, FVector(-5.3f, 8.f, 1.04f), FVector(0.4f, 0.3f, 0.26f));
	Place(VisualMarketGoods, FVector(-4.6f, 8.2f, 1.0f), FVector(0.18f, 0.18f, 0.32f));
	Place(VisualMarketGoods, FVector(-4.9f, 7.65f, 1.15f), FVector(0.35f, 0.22f, 0.18f));

	Place(VisualMarketGoods, FVector(-18.7f, 5.4f, 0.22f), FVector(0.4f, 0.4f, 0.4f));
	Place(VisualMarketGoods, FVector(-18.7f, 14.6f, 0.22f), FVector(0.38f, 0.42f, 0.36f));
	Place(VisualMarketGoods, FVector(-1.4f, 5.4f, 0.22f), FVector(0.36f, 0.36f, 0.38f));
	Place(VisualMarketGoods, FVector(-1.3f, 14.5f, 0.22f), FVector(0.4f, 0.32f, 0.34f));
	Place(VisualMarketCloth, FVector(-18.85f, 5.2f, 2.4f), FVector(0.08f, 0.55f, 0.7f));
	Place(VisualMarketCloth, FVector(-18.85f, 14.8f, 2.4f), FVector(0.08f, 0.55f, 0.7f));
	Place(VisualPipes, FVector(-19.f, 10.f, 3.2f), FVector(0.04f, 0.04f, 4.2f), FRotator(0.f, 0.f, 90.f));
	Place(VisualPipes, FVector(-1.f, 10.f, 3.2f), FVector(0.04f, 0.04f, 4.2f), FRotator(0.f, 0.f, 90.f));

	Place(VisualSigns, FVector(-19.f, 5.55f, 2.1f), FVector(0.06f, 0.7f, 0.45f));
	Place(VisualSigns, FVector(-0.7f, 14.8f, 2.1f), FVector(0.7f, 0.06f, 0.45f));
	Place(VisualSigns, FVector(-10.f, 15.9f, 3.05f), FVector(1.8f, 0.08f, 0.55f));

	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 market props=%d (NoCollision, aisles kept)"), Count);
}

void ANexusEnvironmentDirector::SpawnWarehouseProps(const FNexusVelasqoLayout& Layout)
{
	int32 Count = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& ScaleUu, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (IsGameplaySensitive(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), ScaleUu, Rot);
		++Count;
	};

	// Wall-hug racks / crates: west and south interiors of B_Wh. Plant is at (-26,-22).
	Place(VisualWarehouse, FVector(-29.35f, -24.4f, 0.7f), FVector(0.28f, 1.1f, 1.4f));
	Place(VisualWarehouse, FVector(-29.35f, -20.0f, 0.7f), FVector(0.28f, 1.1f, 1.4f));
	Place(VisualWarehouse, FVector(-28.2f, -25.05f, 0.45f), FVector(0.7f, 0.28f, 0.9f));
	Place(VisualWarehouse, FVector(-24.0f, -25.05f, 0.45f), FVector(0.7f, 0.28f, 0.9f));

	// Pallet stacks in SW corner only.
	Place(VisualWarehouse, FVector(-29.1f, -24.8f, 0.18f), FVector(0.7f, 0.55f, 0.12f));
	Place(VisualWarehouse, FVector(-29.1f, -24.8f, 0.38f), FVector(0.62f, 0.48f, 0.12f));

	// Barrels as cylinders in the same corner.
	Place(VisualPipes, FVector(-28.4f, -24.7f, 0.4f), FVector(0.28f, 0.28f, 0.4f));
	Place(VisualPipes, FVector(-29.0f, -23.9f, 0.4f), FVector(0.26f, 0.26f, 0.38f));

	// Overhead pipes / cable trays under warehouse roof (Z=3.6), above combat height.
	Place(VisualPipes, FVector(-26.f, -24.6f, 3.25f), FVector(0.08f, 0.08f, 3.4f), FRotator(0.f, 0.f, 90.f));
	Place(VisualPipes, FVector(-28.6f, -22.f, 3.28f), FVector(0.07f, 0.07f, 2.8f), FRotator(90.f, 0.f, 0.f));
	Place(VisualWarehouse, FVector(-26.f, -24.8f, 3.32f), FVector(3.6f, 0.18f, 0.08f));

	// Industrial wall panel + lamp boxes on west wall, high.
	Place(VisualWarehouse, FVector(-29.55f, -22.f, 2.6f), FVector(0.08f, 1.8f, 0.25f));
	Place(VisualPipes, FVector(-29.5f, -24.2f, 2.9f), FVector(0.12f, 0.12f, 0.1f));
	Place(VisualPipes, FVector(-29.5f, -19.8f, 2.9f), FVector(0.12f, 0.12f, 0.1f));

	Place(VisualWarehouse, FVector(-29.35f, -19.2f, 1.1f), FVector(0.22f, 0.9f, 2.1f));
	Place(VisualPipes, FVector(-22.6f, -25.1f, 2.4f), FVector(0.12f, 0.12f, 2.2f));
	Place(VisualPipes, FVector(-22.6f, -18.9f, 2.4f), FVector(0.12f, 0.12f, 2.2f));
	Place(VisualWarehouse, FVector(-23.2f, -25.05f, 0.55f), FVector(0.9f, 0.35f, 1.05f));
	Place(VisualSigns, FVector(-22.35f, -22.f, 2.8f), FVector(0.06f, 1.4f, 0.4f));

	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 warehouse props=%d (NoCollision)"), Count);
}

int32 ANexusEnvironmentDirector::CountVisualInstances() const
{
	auto N = [](const UInstancedStaticMeshComponent* ISM) { return ISM ? ISM->GetInstanceCount() : 0; };
	return N(VisualWindows) + N(VisualTrim) + N(VisualProps) + N(VisualGround) + N(VisualMarketCloth)
		+ N(VisualMarketGoods) + N(VisualWarehouse) + N(VisualPipes) + N(VisualRoofs) + N(VisualVehicles)
		+ N(VisualFoliage) + N(VisualSigns) + N(VisualBackdrop) + N(VisualWater) + N(VisualFacades)
		+ N(VisualMilitary) + N(VisualDomes);
}

bool ANexusEnvironmentDirector::IsNearSpawn(const FNexusVelasqoLayout& Layout, const FVector& Meters) const
{
	for (const FNexusSpawnDef& S : Layout.Spawns)
	{
		if (FVector::Dist2D(Meters, S.LocationMeters) < 4.5f)
		{
			return true;
		}
	}
	return false;
}

bool ANexusEnvironmentDirector::CanDress(const FNexusVelasqoLayout& Layout, const FVector& Meters) const
{
	return !IsGameplaySensitive(Layout, Meters) && !IsNearSpawn(Layout, Meters);
}

bool ANexusEnvironmentDirector::IsBenchmarkBlock(const FVector& Meters) const
{
	return Meters.X > -20.f && Meters.X < 14.f && Meters.Y > -10.f && Meters.Y < 12.f;
}

void ANexusEnvironmentDirector::SpawnBenchmarkBlock(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (!IsBenchmarkBlock(Meters) || !CanDress(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), Scale, Rot);
		++N;
	};

	AddVisualInstance(VisualTrim, NexusMeters(FVector(-2.f, -7.15f, 0.12f)), FVector(28.f, 0.18f, 0.18f));
	AddVisualInstance(VisualBackdrop, NexusMeters(FVector(-2.f, -6.45f, 0.07f)), FVector(28.f, 1.15f, 0.1f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(-8.f, -3.35f, 0.12f)), FVector(12.f, 0.16f, 0.16f));
	AddVisualInstance(VisualBackdrop, NexusMeters(FVector(-8.f, -2.85f, 0.07f)), FVector(12.f, 0.85f, 0.09f));
	N += 4;

	Place(VisualWarehouse, FVector(7.4f, -6.35f, 0.32f), FVector(0.32f, 0.28f, 0.62f));
	Place(VisualProps, FVector(6.6f, -6.4f, 0.22f), FVector(0.4f, 0.28f, 0.38f));
	Place(VisualProps, FVector(6.55f, -6.38f, 0.48f), FVector(0.32f, 0.22f, 0.18f));
	Place(VisualFoliage, FVector(5.2f, -6.35f, 0.55f), FVector(0.35f, 0.35f, 0.7f));
	Place(VisualWarehouse, FVector(5.2f, -6.38f, 0.18f), FVector(0.42f, 0.42f, 0.22f));
	Place(VisualProps, FVector(1.8f, -6.5f, 0.28f), FVector(1.1f, 0.32f, 0.42f));
	Place(VisualPipes, FVector(-1.2f, -7.05f, 1.7f), FVector(0.08f, 0.08f, 1.7f));
	Place(VisualSigns, FVector(-1.2f, -7.05f, 3.15f), FVector(0.12f, 0.12f, 0.18f));
	Place(VisualWarehouse, FVector(-6.2f, -2.7f, 0.3f), FVector(0.3f, 0.26f, 0.55f));
	Place(VisualPipes, FVector(7.9f, -6.15f, 2.55f), FVector(0.36f, 0.26f, 0.22f));
	Place(VisualPipes, FVector(7.9f, -6.15f, 3.1f), FVector(0.04f, 0.04f, 0.9f), FRotator(0.f, 0.f, 90.f));
	Place(VisualWarehouse, FVector(-4.6f, 6.2f, 1.55f), FVector(0.12f, 0.22f, 0.32f));
	Place(VisualPipes, FVector(-4.6f, 6.2f, 2.4f), FVector(0.035f, 0.035f, 1.4f));
	Place(VisualSigns, FVector(-8.f, -2.2f, 2.55f), FVector(0.7f, 0.06f, 0.32f));
	Place(VisualProps, FVector(-10.4f, -2.65f, 0.2f), FVector(0.45f, 0.22f, 0.35f));
	Place(VisualFoliage, FVector(-12.2f, -2.7f, 0.7f), FVector(0.28f, 0.28f, 0.85f));
	Place(VisualWarehouse, FVector(-12.2f, -2.72f, 0.16f), FVector(0.38f, 0.38f, 0.2f));
	Place(VisualPipes, FVector(4.2f, 3.35f, 2.7f), FVector(0.04f, 0.04f, 2.6f), FRotator(0.f, 0.f, 90.f));

	for (TActorIterator<ANexusDoor> It(GetWorld()); It; ++It)
	{
		const FVector M = It->GetActorLocation() / 100.f;
		if (!IsBenchmarkBlock(M) || !It->Mesh)
		{
			continue;
		}
		const FVector Loc = It->GetActorLocation();
		const FVector Fwd = It->GetActorForwardVector();
		const FVector Right = It->GetActorRightVector();
		AddVisualInstance(VisualTrim, Loc - Fwd * 6.f + FVector(0.f, 0.f, -95.f), FVector(1.15f, 0.22f, 0.08f), It->GetActorRotation());
		AddVisualInstance(VisualPipes, Loc + Right * 42.f + FVector(0.f, 0.f, 10.f), FVector(0.05f, 0.05f, 0.08f));
		++N;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		const TArray<FVector> Extra = { FVector(2.f, -7.1f, 3.15f), FVector(-8.f, -2.6f, 3.1f), FVector(8.f, -6.2f, 3.05f) };
		for (const FVector& M : Extra)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = this;
			if (APointLight* L = World->SpawnActor<APointLight>(NexusMeters(M), FRotator::ZeroRotator, Params))
			{
				if (UPointLightComponent* C = Cast<UPointLightComponent>(L->GetLightComponent()))
				{
					C->SetIntensity(1100.f);
					C->SetLightColor(FLinearColor(1.f, 0.8f, 0.52f));
					C->SetAttenuationRadius(800.f);
					C->SetCastShadows(false);
					C->SetMobility(EComponentMobility::Movable);
				}
				++N;
			}
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.2 benchmark Old Quarter + Blue House pieces=%d (sidewalk/curb/human scale, NoCollision)"), N);
}

void ANexusEnvironmentDirector::SpawnCityProduction(const FNexusVelasqoLayout& Layout)
{
	TSet<FName> Have;
	for (TActorIterator<ANexusCalloutVolume> It(GetWorld()); It; ++It)
	{
		Have.Add(It->CalloutId);
	}
	int32 Extra = 0;
	for (const FNexusCalloutDef& Call : Layout.Callouts)
	{
		if (Have.Contains(Call.CalloutId))
		{
			continue;
		}
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = this;
		ANexusCalloutVolume* Actor = GetWorld()->SpawnActor<ANexusCalloutVolume>(NexusMeters(Call.CenterMeters), FRotator::ZeroRotator, Params);
		if (Actor)
		{
			Actor->CalloutId = Call.CalloutId;
			Actor->DisplayName = Call.DisplayName;
			if (Actor->Volume)
			{
				Actor->Volume->SetBoxExtent(NexusMeters(Call.ExtentMeters));
			}
			++Extra;
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6 extra callout volumes=%d (no gameplay geo)"), Extra);

	SpawnRooftopKit(Layout);
	SpawnAlleyDressing(Layout);
	SpawnPlazaAndFountain(Layout);
	SpawnCaravanseraiDress(Layout);
	SpawnBenchmarkBlock(Layout);
	SpawnVehiclesAndMilitary(Layout);
	SpawnFoliageAndSignage(Layout);
	SpawnBackdropSkyline();
	SpawnPracticalLights();
}

void ANexusEnvironmentDirector::SpawnRooftopKit(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	for (const FNexusBoxDef& Box : Layout.Boxes)
	{
		if (Box.Usage != ENexusBoxUsage::Structure)
		{
			continue;
		}
		const float MinXY = FMath::Min(Box.SizeMeters.X, Box.SizeMeters.Y);
		if (Box.SizeMeters.Z < 3.0f || MinXY < 2.4f)
		{
			continue;
		}
		const FVector RoofMeters = Box.CenterMeters + FVector(0.f, 0.f, Box.SizeMeters.Z * 0.5f + 0.45f);
		if (!CanDress(Layout, FVector(RoofMeters.X, RoofMeters.Y, 0.f)))
		{
			continue;
		}
		PlaceRoofKit(RoofMeters, N);
		if (N >= 40)
		{
			break;
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 rooftop kit=%d (NoCollision, no new walkable roofs)"), N);
}

void ANexusEnvironmentDirector::SpawnAlleyDressing(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (!CanDress(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), Scale, Rot);
		++N;
	};

	// Wall-hug AC / meters / cables: Y edges of south alleys, never the 2.2m walk corridor.
	for (float X = -42.f; X <= 42.f; X += 5.f)
	{
		Place(VisualPipes, FVector(X, -34.6f, 2.55f), FVector(0.42f, 0.28f, 0.26f));
		Place(VisualPipes, FVector(X, -15.4f, 2.55f), FVector(0.42f, 0.28f, 0.26f));
		Place(VisualPipes, FVector(X, -34.7f, 3.5f), FVector(0.04f, 0.04f, 3.2f), FRotator(0.f, 0.f, 90.f));
		Place(VisualProps, FVector(X + 1.5f, -34.55f, 0.22f), FVector(0.28f, 0.22f, 0.4f));
		Place(VisualProps, FVector(X + 1.5f, -15.45f, 0.22f), FVector(0.26f, 0.2f, 0.38f));
		Place(VisualTrim, FVector(X + 2.4f, -34.55f, 1.1f), FVector(0.9f, 0.12f, 2.15f));
		Place(VisualSigns, FVector(X, -34.5f, 2.9f), FVector(0.55f, 0.06f, 0.28f));
	}

	for (const FNexusBoxDef& Box : Layout.Boxes)
	{
		if (Box.Usage != ENexusBoxUsage::Structure || Box.SizeMeters.Z < 2.6f)
		{
			continue;
		}
		const float MinXY = FMath::Min(Box.SizeMeters.X, Box.SizeMeters.Y);
		if (MinXY > 0.55f)
		{
			continue;
		}
		const bool bThinX = Box.SizeMeters.X < Box.SizeMeters.Y;
		FVector AC = Box.CenterMeters;
		AC.Z = Box.CenterMeters.Z + 0.2f;
		if (bThinX)
		{
			AC.X += (AC.X >= 0.f) ? 0.38f : -0.38f;
		}
		else
		{
			AC.Y += (AC.Y >= 0.f) ? 0.38f : -0.38f;
		}
		if (CanDress(Layout, AC) && N < 90)
		{
			AddVisualInstance(VisualFacades, NexusMeters(AC), FVector(0.38f, 0.3f, 0.24f));
			++N;
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 alley dressing=%d (NoCollision, corridor 2.2m kept)"), N);
}

void ANexusEnvironmentDirector::SpawnPlazaAndFountain(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (!CanDress(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), Scale, Rot);
		++N;
	};

	// Water plane sits on existing fountain cover — NoCollision overlay, allowed even near plant because it does not add collision.
	AddVisualInstance(VisualWater, NexusMeters(FVector(22.f, 16.f, 0.42f)), FVector(2.1f, 1.7f, 0.04f));
	++N;

	Place(VisualSigns, FVector(27.6f, 8.4f, 2.4f), FVector(0.08f, 0.9f, 0.55f));
	Place(VisualSigns, FVector(16.4f, 8.2f, 2.4f), FVector(0.9f, 0.08f, 0.55f));
	Place(VisualFoliage, FVector(27.2f, 20.6f, 1.4f), FVector(0.45f, 0.45f, 1.8f));
	Place(VisualFoliage, FVector(16.8f, 9.2f, 1.15f), FVector(0.38f, 0.38f, 1.5f));
	Place(VisualFoliage, FVector(28.4f, 9.4f, 0.55f), FVector(0.28f, 0.28f, 0.7f));
	Place(VisualProps, FVector(27.4f, 20.4f, 0.22f), FVector(0.55f, 0.22f, 0.45f));
	Place(VisualProps, FVector(16.6f, 20.6f, 0.22f), FVector(0.55f, 0.22f, 0.45f));
	Place(VisualProps, FVector(28.2f, 8.8f, 0.18f), FVector(0.4f, 0.4f, 0.22f));
	Place(VisualFacades, FVector(31.f, 14.f, 3.05f), FVector(1.6f, 0.08f, 0.35f));
	Place(VisualFacades, FVector(17.f, 22.f, 3.05f), FVector(1.8f, 0.08f, 0.3f));
	Place(VisualPipes, FVector(27.6f, 8.4f, 3.6f), FVector(0.08f, 0.08f, 0.9f));
	Place(VisualPipes, FVector(16.4f, 8.2f, 3.6f), FVector(0.08f, 0.08f, 0.9f));
	Place(VisualPipes, FVector(14.8f, 22.f, 3.8f), FVector(0.07f, 0.07f, 1.1f));

	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 plaza dressing=%d"), N);
}

void ANexusEnvironmentDirector::SpawnCaravanseraiDress(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (IsGameplaySensitive(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), Scale, Rot);
		++N;
	};
	Place(VisualSigns, FVector(-18.f, -6.9f, 5.6f), FVector(2.2f, 0.1f, 0.6f));
	Place(VisualWarehouse, FVector(-8.2f, -24.5f, 0.7f), FVector(0.8f, 1.2f, 1.3f));
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 caravanserai dress=%d (no courtyard pergola)"), N);
}

void ANexusEnvironmentDirector::SpawnVehiclesAndMilitary(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator)
	{
		if (FMath::Abs(Meters.X) < 60.f && FMath::Abs(Meters.Y) < 60.f && !CanDress(Layout, Meters))
		{
			return;
		}
		if (FMath::Abs(Meters.X) < 60.f && FMath::Abs(Meters.Y) < 60.f && IsNearSpawn(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), Scale, Rot);
		++N;
	};

	// Parking just outside playable bounds — skyline / approach, no nav, no collision.
	PlaceVehicleKit(FVector(66.f, 8.f, 0.7f), FVector(4.4f, 1.8f, 1.4f), FRotator(0.f, 90.f, 0.f));
	PlaceVehicleKit(FVector(66.f, -6.f, 0.75f), FVector(4.6f, 1.9f, 1.5f), FRotator(0.f, 90.f, 0.f));
	PlaceVehicleKit(FVector(-66.f, 4.f, 0.7f), FVector(4.2f, 1.7f, 1.35f));
	PlaceVehicleKit(FVector(-64.f, -12.f, 0.9f), FVector(5.2f, 2.1f, 1.8f));
	PlaceVehicleKit(FVector(8.f, 66.f, 0.8f), FVector(4.5f, 1.9f, 1.5f));
	PlaceVehicleKit(FVector(48.f, 36.f, 0.65f), FVector(4.2f, 1.7f, 1.3f), FRotator(0.f, 20.f, 0.f));
	PlaceVehicleKit(FVector(-32.f, -38.f, 0.6f), FVector(4.0f, 1.6f, 1.2f), FRotator(0.f, 90.f, 0.f));
	PlaceVehicleKit(FVector(-48.f, 58.f, 0.55f), FVector(4.4f, 1.8f, 1.4f));
	Place(VisualPipes, FVector(48.f, 58.f, 0.4f), FVector(0.9f, 0.25f, 0.55f));
	Place(VisualPipes, FVector(-48.f, 58.f, 0.4f), FVector(0.9f, 0.25f, 0.55f));
	N += 8;

	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 vehicles=%d (NoCollision — not extra gameplay cover)"), N);
}

void ANexusEnvironmentDirector::SpawnFoliageAndSignage(const FNexusVelasqoLayout& Layout)
{
	int32 N = 0;
	auto Place = [&](UInstancedStaticMeshComponent* ISM, const FVector& Meters, const FVector& Scale)
	{
		if (!CanDress(Layout, Meters))
		{
			return;
		}
		AddVisualInstance(ISM, NexusMeters(Meters), Scale);
		++N;
	};

	Place(VisualFoliage, FVector(-8.f, 7.6f, 1.5f), FVector(0.38f, 0.38f, 2.0f));
	Place(VisualFoliage, FVector(6.f, -8.f, 1.2f), FVector(0.32f, 0.32f, 1.6f));
	Place(VisualFoliage, FVector(40.f, 18.f, 1.35f), FVector(0.34f, 0.34f, 1.75f));
	Place(VisualFoliage, FVector(-22.f, -4.f, 1.1f), FVector(0.28f, 0.28f, 1.4f));
	Place(VisualFoliage, FVector(12.f, 28.f, 0.7f), FVector(0.24f, 0.24f, 0.9f));
	Place(VisualFoliage, FVector(-36.f, 8.f, 0.55f), FVector(0.22f, 0.22f, 0.7f));

	Place(VisualSigns, FVector(-10.f, 16.4f, 3.1f), FVector(1.4f, 0.08f, 0.5f));
	Place(VisualSigns, FVector(0.f, 8.f, 3.2f), FVector(0.08f, 1.2f, 0.45f));
	Place(VisualSigns, FVector(-18.f, -6.5f, 4.2f), FVector(1.6f, 0.08f, 0.5f));
	Place(VisualSigns, FVector(38.f, 30.6f, 3.4f), FVector(1.2f, 0.08f, 0.4f));
	Place(VisualSigns, FVector(-8.f, 6.8f, 5.6f), FVector(0.9f, 0.08f, 0.35f));

	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 foliage/signage=%d"), N);
}

void ANexusEnvironmentDirector::SpawnBackdropSkyline()
{
	int32 N = 0;
	auto Tower = [&](float X, float Y, float W, float D, float H, bool bGlass)
	{
		if (FMath::Abs(X) < 62.f && FMath::Abs(Y) < 62.f)
		{
			return;
		}
		PlaceBackdropTower(FVector(X, Y, H * 0.5f), FVector(W, D, H), bGlass);
		++N;
	};
	Tower(0.f, 86.f, 12.f, 10.f, 22.f, false);
	Tower(18.f, 88.f, 8.f, 8.f, 16.f, false);
	Tower(-20.f, 90.f, 7.f, 9.f, 14.f, false);
	Tower(36.f, 84.f, 6.f, 7.f, 11.f, false);
	Tower(-38.f, 85.f, 7.f, 6.f, 12.f, false);
	Tower(78.f, 8.f, 8.f, 10.f, 15.f, false);
	Tower(80.f, -18.f, 7.f, 8.f, 12.f, false);
	Tower(-80.f, 6.f, 8.f, 9.f, 14.f, false);
	Tower(-82.f, -22.f, 6.f, 7.f, 10.f, false);
	Tower(12.f, -86.f, 9.f, 8.f, 13.f, false);
	Tower(-16.f, -88.f, 7.f, 7.f, 11.f, false);
	AddVisualInstance(VisualPipes, NexusMeters(FVector(22.f, 86.f, 14.f)), FVector(1.05f, 1.05f, 18.f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(22.f, 86.f, 23.2f)), FVector(1.7f, 1.7f, 0.3f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(16.f, 80.f, 4.2f)), FVector(0.28f, 0.28f, 8.2f));
	AddVisualInstance(VisualFoliage, NexusMeters(FVector(16.f, 80.f, 9.1f)), FVector(2.4f, 2.4f, 2.1f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(28.5f, 79.5f, 3.6f)), FVector(0.24f, 0.24f, 7.2f));
	AddVisualInstance(VisualFoliage, NexusMeters(FVector(28.5f, 79.5f, 7.8f)), FVector(2.0f, 2.0f, 1.8f));
	N += 3;
	UE_LOG(LogNexusArt, Log, TEXT("V0.6 backdrop+mosque skyline=%d (outside playable, NoCollision, no nav)"), N);
}

void ANexusEnvironmentDirector::SpawnPracticalLights()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const TArray<FVector> Lamps = {
		FVector(-10.f, 16.5f, 3.4f),
		FVector(22.f, 8.2f, 3.6f),
		FVector(38.f, 24.f, 4.2f),
		FVector(-8.f, 2.f, 3.8f),
		FVector(-26.f, -22.f, 3.4f),
		FVector(-18.f, -10.f, 7.4f),
		FVector(0.f, -25.f, 3.5f),
		FVector(31.f, 14.f, 3.3f),
		FVector(17.f, 22.f, 3.4f),
		FVector(-30.f, -25.f, 3.2f),
		FVector(10.f, -25.f, 3.2f),
		FVector(-8.f, 16.f, 3.3f),
		FVector(4.f, 6.f, 3.5f)
	};
	int32 Spawned = 0;
	for (const FVector& M : Lamps)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = this;
		APointLight* L = World->SpawnActor<APointLight>(NexusMeters(M), FRotator::ZeroRotator, Params);
		if (!L)
		{
			continue;
		}
		if (UPointLightComponent* C = Cast<UPointLightComponent>(L->GetLightComponent()))
		{
			C->SetIntensity(1250.f);
			C->SetLightColor(FLinearColor(1.f, 0.78f, 0.48f));
			C->SetAttenuationRadius(1100.f);
			C->SetCastShadows(false);
			C->SetMobility(EComponentMobility::Movable);
		}
		++Spawned;
	}
	UE_LOG(LogNexusArt, Log, TEXT("V0.6.1 practical lights=%d (no shadow)"), Spawned);
}

void ANexusEnvironmentDirector::SpawnVelasqoGddSpots(const FNexusVelasqoLayout& Layout)
{
	(void)Layout;
	int32 N = 0;
	auto Spot = [&](const TCHAR* Name)
	{
		++N;
		UE_LOG(LogNexusArt, Log, TEXT("GDD SPOT VELASQO %d/6 %s"), N, Name);
	};

	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(-16.f, 5.6f, 2.55f)), FVector(3.2f, 0.08f, 0.7f));
	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(-4.f, 5.6f, 2.55f)), FVector(3.2f, 0.08f, 0.7f));
	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(-16.f, 14.4f, 2.55f)), FVector(3.2f, 0.08f, 0.7f));
	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(-4.f, 14.4f, 2.55f)), FVector(3.2f, 0.08f, 0.7f));
	AddVisualInstance(VisualProps, NexusMeters(FVector(-16.f, 5.5f, 0.55f)), FVector(2.4f, 0.7f, 0.85f));
	AddVisualInstance(VisualProps, NexusMeters(FVector(-4.f, 5.5f, 0.55f)), FVector(2.4f, 0.7f, 0.85f));
	AddVisualInstance(VisualProps, NexusMeters(FVector(-16.f, 14.5f, 0.55f)), FVector(2.4f, 0.7f, 0.85f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-10.f, 10.f, 3.05f)), FVector(0.08f, 0.08f, 0.16f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-16.f, 10.f, 3.05f)), FVector(0.08f, 0.08f, 0.16f));
	Spot(TEXT("Marche"));

	PlaceMosqueKit(FVector(8.f, 32.f, 0.f));
	Spot(TEXT("Mosquee"));

	AddVisualInstance(VisualPipes, NexusMeters(FVector(15.f, 12.f, 4.35f)), FVector(0.7f, 0.7f, 0.18f), FRotator(28.f, 12.f, 0.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(8.f, -4.f, 4.15f)), FVector(0.55f, 0.08f, 0.55f), FRotator(25.f, 10.f, 0.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(31.f, 14.f, 4.2f)), FVector(0.8f, 0.55f, 0.4f));
	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(8.f, -1.4f, 3.85f)), FVector(2.6f, 0.08f, 0.9f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(15.2f, 12.4f, 4.7f)), FVector(0.06f, 0.06f, 1.1f));
	Spot(TEXT("Toits"));

	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(-10.f, -34.2f, 3.15f)), FVector(6.f, 0.08f, 0.55f));
	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(10.f, -34.2f, 3.2f)), FVector(6.f, 0.08f, 0.5f));
	PlaceVehicleKit(FVector(-8.f, -28.6f, 0.65f), FVector(3.8f, 1.6f, 1.3f), FRotator(0.f, 90.f, 0.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(0.f, -34.5f, 3.4f)), FVector(0.04f, 0.04f, 14.f), FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(-2.f, -34.45f, 1.4f)), FVector(1.1f, 0.12f, 2.4f));
	Spot(TEXT("Ruelle"));

	AddVisualInstance(VisualTrim, NexusMeters(FVector(-8.f, -2.35f, 1.7f)), FVector(1.8f, 0.18f, 0.28f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(-8.9f, -2.35f, 1.2f)), FVector(0.22f, 0.18f, 2.4f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(-7.1f, -2.35f, 1.2f)), FVector(0.22f, 0.18f, 2.4f));
	PlaceVehicleKit(FVector(-8.f, -0.6f, 0.65f), FVector(3.6f, 1.5f, 1.2f), FRotator(0.f, 8.f, 0.f));
	AddVisualInstance(VisualMarketCloth, NexusMeters(FVector(-8.f, 2.f, 0.06f)), FVector(3.2f, 2.4f, 0.04f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-8.f, 2.f, 2.6f)), FVector(0.12f, 0.12f, 0.2f));
	Spot(TEXT("Interieur"));

	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(-29.2f, -24.6f, 0.85f)), FVector(0.7f, 1.2f, 1.5f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(-29.2f, -19.6f, 0.85f)), FVector(0.7f, 1.2f, 1.5f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(-23.2f, -25.0f, 0.55f)), FVector(1.1f, 0.45f, 1.05f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-26.f, -24.7f, 3.2f)), FVector(0.1f, 0.1f, 4.2f), FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-29.4f, -22.f, 2.85f)), FVector(0.16f, 0.16f, 0.12f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(-22.4f, -22.f, 2.7f)), FVector(0.06f, 1.6f, 0.45f));
	Spot(TEXT("SiteB"));

	UE_LOG(LogNexusArt, Log, TEXT("GDD SPOTS VELASQO %d/6 (NoCollision, Cube/Cylinder/Plane)"), N);
}

void ANexusEnvironmentDirector::SpawnSkylineDress(const FNexusVelasqoLayout& Layout)
{
	(void)Layout;
	int32 N = 0;
	auto Spot = [&](const TCHAR* Name)
	{
		++N;
		UE_LOG(LogNexusArt, Log, TEXT("GDD SPOT SKYLINE %d/6 %s"), N, Name);
	};

	AddVisualInstance(VisualWindows, NexusMeters(FVector(0.f, 2.f, 4.32f)), FVector(12.f, 12.f, 0.05f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(0.f, 2.f, 4.2f)), FVector(0.08f, 0.08f, 12.f), FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(0.f, 2.f, 4.2f)), FVector(0.08f, 0.08f, 12.f), FRotator(90.f, 0.f, 0.f));
	AddVisualInstance(VisualFoliage, NexusMeters(FVector(-5.2f, -4.2f, 0.7f)), FVector(1.1f, 1.1f, 1.2f));
	AddVisualInstance(VisualFoliage, NexusMeters(FVector(5.2f, -4.2f, 0.7f)), FVector(1.1f, 1.1f, 1.2f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-5.2f, -4.2f, 0.35f)), FVector(0.22f, 0.22f, 0.7f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(5.2f, -4.2f, 0.35f)), FVector(0.22f, 0.22f, 0.7f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(0.f, 2.f, 0.05f)), FVector(16.f, 16.f, 0.04f));
	Spot(TEXT("Hall"));

	auto Desk = [&](const FVector& D)
	{
		AddVisualInstance(VisualProps, NexusMeters(D), FVector(1.6f, 0.8f, 0.75f));
		AddVisualInstance(VisualWindows, NexusMeters(D + FVector(0.f, 0.f, 0.55f)), FVector(0.55f, 0.08f, 0.4f));
		AddVisualInstance(VisualProps, NexusMeters(D + FVector(0.f, 0.7f, 0.45f)), FVector(0.45f, 0.45f, 0.9f));
	};
	Desk(FVector(34.f, 2.f, 0.55f));
	Desk(FVector(38.f, 6.f, 0.55f));
	Desk(FVector(32.f, 7.f, 0.55f));
	Desk(FVector(-24.f, 0.f, 0.55f));
	Desk(FVector(-22.f, 4.f, 0.55f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(36.f, -2.9f, 2.2f)), FVector(8.f, 0.06f, 2.4f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(36.f, 4.f, 3.9f)), FVector(0.08f, 0.08f, 8.f), FRotator(0.f, 0.f, 90.f));
	Spot(TEXT("Bureaux"));

	PlaceHelipadKit(FVector(0.f, 2.f, 9.55f));
	PlaceHeliKit(FVector(2.4f, 4.2f, 10.15f), FRotator(0.f, 25.f, 0.f));
	Spot(TEXT("Helipad"));

	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(26.f, -18.f, 3.38f)), FVector(22.f, 16.f, 0.1f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(21.f, -21.f, 0.04f)), FVector(3.8f, 0.14f, 0.03f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(29.f, -15.f, 0.04f)), FVector(3.8f, 0.14f, 0.03f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(32.f, -22.f, 0.04f)), FVector(3.8f, 0.14f, 0.03f));
	PlaceVehicleKit(FVector(21.f, -21.f, 0.7f), FVector(4.2f, 1.8f, 1.35f), FRotator(0.f, 8.f, 0.f));
	PlaceVehicleKit(FVector(29.f, -15.f, 0.7f), FVector(4.2f, 1.8f, 1.35f), FRotator(0.f, -6.f, 0.f));
	PlaceVehicleKit(FVector(32.f, -22.f, 0.7f), FVector(4.2f, 1.8f, 1.35f), FRotator(0.f, 4.f, 0.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(26.f, -18.f, 3.15f)), FVector(0.08f, 0.08f, 16.f), FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(26.f, -12.f, 3.15f)), FVector(0.08f, 0.08f, 16.f), FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(26.f, -24.f, 3.15f)), FVector(0.08f, 0.08f, 16.f), FRotator(0.f, 0.f, 90.f));
	Spot(TEXT("Parking"));

	AddVisualInstance(VisualWindows, NexusMeters(FVector(18.5f, 3.35f, 5.2f)), FVector(18.f, 0.06f, 1.4f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(18.5f, 0.65f, 5.2f)), FVector(18.f, 0.06f, 1.4f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(18.5f, 2.f, 4.42f)), FVector(18.f, 2.2f, 0.04f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(-13.5f, 3.35f, 5.2f)), FVector(8.5f, 0.06f, 1.4f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(-13.5f, 0.65f, 5.2f)), FVector(8.5f, 0.06f, 1.4f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(18.5f, 2.f, 5.55f)), FVector(0.1f, 0.1f, 18.f), FRotator(0.f, 0.f, 90.f));
	Spot(TEXT("Passerelle"));

	AddVisualInstance(VisualProps, NexusMeters(FVector(2.f, 5.f, 4.95f)), FVector(2.2f, 0.9f, 0.75f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(2.f, 5.f, 5.5f)), FVector(0.7f, 0.08f, 0.45f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(0.f, -8.7f, 6.8f)), FVector(14.f, 0.06f, 2.8f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(0.f, -9.2f, 8.3f)), FVector(5.2f, 0.08f, 0.7f));
	AddVisualInstance(VisualFoliage, NexusMeters(FVector(-4.f, 6.f, 4.95f)), FVector(0.7f, 0.7f, 0.9f));
	Spot(TEXT("SiteA"));

	PlaceBackdropTower(FVector(0.f, 88.f, 18.f), FVector(10.f, 9.f, 36.f), true);
	PlaceBackdropTower(FVector(22.f, 86.f, 14.f), FVector(8.f, 8.f, 28.f), true);
	PlaceBackdropTower(FVector(-24.f, 90.f, 12.f), FVector(7.f, 8.f, 24.f), true);

	UWorld* World = GetWorld();
	if (World)
	{
		const TArray<FVector> Lamps = { FVector(-24.f, 22.f, 4.2f), FVector(18.5f, 2.f, 6.2f), FVector(26.f, -18.f, 3.8f), FVector(0.f, 2.f, 4.6f), FVector(36.f, 4.f, 4.2f) };
		for (const FVector& M : Lamps)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = this;
			if (APointLight* Lgt = World->SpawnActor<APointLight>(NexusMeters(M), FRotator::ZeroRotator, Params))
			{
				if (UPointLightComponent* C = Cast<UPointLightComponent>(Lgt->GetLightComponent()))
				{
					C->SetIntensity(1800.f);
					C->SetLightColor(FLinearColor(0.55f, 0.72f, 1.f));
					C->SetAttenuationRadius(1400.f);
					C->SetCastShadows(false);
					C->SetMobility(EComponentMobility::Movable);
				}
			}
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("GDD SPOTS SKYLINE %d/6 (NoCollision, Cube/Cylinder/Plane)"), N);
}

void ANexusEnvironmentDirector::SpawnOutpostDress(const FNexusVelasqoLayout& Layout)
{
	(void)Layout;
	int32 N = 0;
	auto Spot = [&](const TCHAR* Name)
	{
		++N;
		UE_LOG(LogNexusArt, Log, TEXT("GDD SPOT OUTPOST %d/6 %s"), N, Name);
	};

	const float HgX = -30.f, HgY = 24.f;
	AddVisualInstance(VisualPipes, NexusMeters(FVector(HgX, HgY, 8.8f)), FVector(4.2f, 4.2f, 25.f), FRotator(0.f, 90.f, 90.f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(HgX, HgY - 10.2f, 4.2f)), FVector(26.4f, 0.22f, 0.35f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(HgX - 12.6f, HgY - 9.6f, 3.6f)), FVector(0.28f, 0.28f, 7.2f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(HgX + 12.6f, HgY - 9.6f, 3.6f)), FVector(0.28f, 0.28f, 7.2f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(HgX, HgY - 10.4f, 5.6f)), FVector(7.5f, 0.08f, 1.0f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(HgX - 10.f, HgY - 6.f, 0.7f)), FVector(1.6f, 1.1f, 1.3f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(HgX + 10.f, HgY - 6.f, 0.7f)), FVector(1.6f, 1.1f, 1.3f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(HgX, HgY - 2.f, 6.6f)), FVector(0.12f, 0.12f, 18.f), FRotator(0.f, 0.f, 90.f));
	Spot(TEXT("Hangar"));

	PlaceContainerKit(FVector(-24.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(-12.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(0.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(12.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(24.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(-18.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(-6.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(6.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(18.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(6.f, -34.f, 3.9f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(-12.f, -28.f, 3.9f), FVector(6.f, 2.4f, 2.6f));
	PlaceContainerKit(FVector(-28.f, -31.f, 1.3f), FVector(2.4f, 6.f, 2.6f));
	Spot(TEXT("Conteneurs"));

	PlaceWatchtowerKit(FVector(0.f, 20.f, 0.f));
	Spot(TEXT("TourGarde"));

	AddVisualInstance(VisualWindows, NexusMeters(FVector(-46.f, -1.9f, 2.4f)), FVector(6.4f, 0.08f, 2.2f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(-46.f, 9.9f, 2.4f)), FVector(6.4f, 0.08f, 2.2f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(-46.f, 4.f, 6.15f)), FVector(8.4f, 12.4f, 0.22f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(-46.f, 8.f, 6.7f)), FVector(0.55f, 0.4f, 0.28f));
	AddVisualInstance(VisualWindows, NexusMeters(FVector(43.05f, 8.f, 2.4f)), FVector(0.08f, 8.f, 2.2f));
	AddVisualInstance(VisualTrim, NexusMeters(FVector(46.f, 8.f, 6.15f)), FVector(6.4f, 12.4f, 0.22f));
	Spot(TEXT("Batiments"));

	PlaceTankKit(FVector(-2.f, -13.f, 1.5f), FVector(3.2f, 3.2f, 3.2f));
	PlaceTankKit(FVector(2.5f, -13.f, 1.25f), FVector(2.6f, 2.6f, 2.6f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(0.2f, -13.f, 0.85f)), FVector(0.1f, 0.1f, 3.4f), FRotator(0.f, 0.f, 90.f));
	PlaceVehicleKit(FVector(-14.f, 14.f, 1.1f), FVector(5.5f, 2.2f, 2.2f), FRotator(0.f, 12.f, 0.f));
	PlaceVehicleKit(FVector(18.f, 10.f, 1.1f), FVector(5.5f, 2.2f, 2.2f), FRotator(0.f, -18.f, 0.f));
	PlaceVehicleKit(FVector(14.f, -18.f, 1.1f), FVector(5.5f, 2.2f, 2.2f), FRotator(0.f, 90.f, 0.f));
	AddVisualInstance(VisualMilitary, NexusMeters(FVector(-4.f, 8.f, 0.7f)), FVector(2.2f, 1.6f, 1.4f));
	Spot(TEXT("Ravitaillement"));

	const float WhX = 32.f, WhY = -8.f;
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(WhX + 2.f, WhY - 10.f, 3.4f)), FVector(10.4f, 4.6f, 0.16f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(WhX - 2.4f, WhY - 11.8f, 1.7f)), FVector(0.22f, 0.22f, 3.4f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(WhX + 6.4f, WhY - 11.8f, 1.7f)), FVector(0.22f, 0.22f, 3.4f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(WhX - 8.f, WhY + 4.f, 0.9f)), FVector(1.4f, 0.9f, 1.6f));
	AddVisualInstance(VisualWarehouse, NexusMeters(FVector(WhX + 8.f, WhY - 4.f, 0.9f)), FVector(1.4f, 0.9f, 1.6f));
	AddVisualInstance(VisualPipes, NexusMeters(FVector(WhX, WhY, 5.6f)), FVector(0.1f, 0.1f, 14.f), FRotator(0.f, 0.f, 90.f));
	AddVisualInstance(VisualSigns, NexusMeters(FVector(WhX, WhY - 8.1f, 4.4f)), FVector(5.5f, 0.08f, 0.7f));
	Spot(TEXT("SiteB"));

	PlaceBackdropTower(FVector(0.f, 86.f, 5.f), FVector(14.f, 8.f, 10.f), false);
	PlaceBackdropTower(FVector(22.f, 84.f, 4.2f), FVector(10.f, 7.f, 8.4f), false);
	AddVisualInstance(VisualPipes, NexusMeters(FVector(0.f, 86.f, 10.2f)), FVector(12.f, 12.f, 14.f), FRotator(0.f, 0.f, 90.f));

	UWorld* World = GetWorld();
	if (World)
	{
		const TArray<FVector> Lamps = { FVector(-30.f, 14.f, 5.5f), FVector(0.f, 4.f, 4.2f), FVector(32.f, -8.f, 4.6f), FVector(0.f, -28.f, 3.8f), FVector(0.f, 20.f, 7.2f) };
		for (const FVector& M : Lamps)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = this;
			if (APointLight* Lgt = World->SpawnActor<APointLight>(NexusMeters(M), FRotator::ZeroRotator, Params))
			{
				if (UPointLightComponent* C = Cast<UPointLightComponent>(Lgt->GetLightComponent()))
				{
					C->SetIntensity(1400.f);
					C->SetLightColor(FLinearColor(1.f, 0.72f, 0.42f));
					C->SetAttenuationRadius(1200.f);
					C->SetCastShadows(false);
					C->SetMobility(EComponentMobility::Movable);
				}
			}
		}
	}
	UE_LOG(LogNexusArt, Log, TEXT("GDD SPOTS OUTPOST %d/6 (NoCollision, Cube/Cylinder/Plane)"), N);
}

