#include "Maps/NexusVelasqoBuilder.h"
#include "Maps/NexusVelasqoLayout.h"
#include "Maps/NexusMapCatalog.h"
#include "Maps/NexusMapBounds.h"
#include "Maps/NexusTeamStart.h"
#include "Maps/NexusRouteSpline.h"
#include "Maps/NexusTaggedVolume.h"
#include "Interaction/NexusDoor.h"
#include "Interaction/NexusCover.h"
#include "Interaction/NexusCalloutVolume.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "Objectives/NexusBombSite.h"
#include "Weapons/NexusPenetration.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NEXUS.h"

ANexusVelasqoBuilder::ANexusVelasqoBuilder()
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

	for (int32 i = 0; i < 7; ++i)
	{
		const FName Name(*FString::Printf(TEXT("ISM_%d"), i));
		UInstancedStaticMeshComponent* ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(Name);
		ISM->SetupAttachment(Root);
		ISM->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ISM->SetCollisionProfileName(TEXT("BlockAll"));
		ISM->SetCanEverAffectNavigation(true);
		MaterialMeshes.Add(ISM);
	}
}

void ANexusVelasqoBuilder::BeginPlay()
{
	Super::BeginPlay();
	if (bBuildOnBeginPlay && HasAuthority())
	{
		BuildMap();
	}
}

UInstancedStaticMeshComponent* ANexusVelasqoBuilder::GetMeshFor(ENexusMaterialType Type)
{
	const int32 Index = FMath::Clamp(static_cast<int32>(Type), 0, MaterialMeshes.Num() - 1);
	return MaterialMeshes[Index];
}

void ANexusVelasqoBuilder::ClearMap()
{
	for (UInstancedStaticMeshComponent* ISM : MaterialMeshes)
	{
		if (ISM)
		{
			ISM->ClearInstances();
		}
	}
	bBuilt = false;
}

void ANexusVelasqoBuilder::BuildMap()
{
	if (bBuilt || !GetWorld())
	{
		return;
	}
	const ENexusMapId MapId = FNexusMapCatalog::DetectFromWorld(GetWorld());
	for (TActorIterator<ANexusBombSite> It(GetWorld()); It; ++It)
	{
		bBuilt = true;
		UE_LOG(LogNEXUS, Log, TEXT("%s already present in world, skipping rebuild"), FNexusMapCatalog::MapName(MapId));
		return;
	}

	UMaterialInterface* BaseMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	for (int32 i = 0; i < MaterialMeshes.Num(); ++i)
	{
		UInstancedStaticMeshComponent* ISM = MaterialMeshes[i];
		if (!ISM)
		{
			continue;
		}
		ISM->SetStaticMesh(CubeMesh);
		if (BaseMat)
		{
			if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this))
			{
				MID->SetVectorParameterValue(TEXT("Color"), UNexusPenetrationStatics::GetMaterialColor(static_cast<ENexusMaterialType>(i)));
				ISM->SetMaterial(0, MID);
			}
		}
	}

	const FNexusVelasqoLayout Layout = FNexusMapCatalog::BuildLayout(MapId);
	for (const FNexusBoxDef& Box : Layout.Boxes)
	{
		const FVector SizeUu = NexusMeters(Box.SizeMeters);
		const FVector CenterUu = NexusMeters(Box.CenterMeters);
		if (Box.bCanBeDestroyed)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ANexusDestructibleSurface* Surf = GetWorld()->SpawnActor<ANexusDestructibleSurface>(CenterUu, FRotator::ZeroRotator, Params);
			if (Surf)
			{
				Surf->MaterialType = Box.Material;
				Surf->Health = Box.Health;
				Surf->ReplicatedHealth = Box.Health;
				Surf->Thickness = Box.ThicknessMeters * 100.f;
				Surf->bCanBeDestroyed = true;
				Surf->Mesh->SetWorldScale3D(SizeUu / 100.f);
				Surf->RefreshVisual();
			}
			continue;
		}

		UInstancedStaticMeshComponent* ISM = GetMeshFor(Box.Material);
		if (!ISM)
		{
			continue;
		}
		const FTransform Xform(FRotator::ZeroRotator, CenterUu, SizeUu / 100.f);
		ISM->AddInstance(Xform, true);
	}

	SpawnGameplay(Layout);

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		const EWorldType::Type WT = GetWorld()->WorldType;
		if (WT != EWorldType::Game && WT != EWorldType::PIE)
		{
			NavSys->Build();
		}
	}

	bBuilt = true;
	UE_LOG(LogNEXUS, Log, TEXT("%s greybox built: %d boxes, %d spawns, %d sites"), FNexusMapCatalog::MapName(MapId), Layout.Boxes.Num(), Layout.Spawns.Num(), Layout.Sites.Num());
}

void ANexusVelasqoBuilder::SpawnGameplay(const FNexusVelasqoLayout& Layout)
{
	UWorld* World = GetWorld();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ANexusMapBounds* MapBounds = World->SpawnActor<ANexusMapBounds>(FVector::ZeroVector, FRotator::ZeroRotator, Params))
	{
		MapBounds->ApplyPlayableExtent();
	}

	if (ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(FVector(0.f, 0.f, 400.f), FRotator::ZeroRotator, Params))
	{
		// Runtime SpawnActor does not run UActorFactoryBoxVolume, so the cube UModel
		// is empty (Map Check VolumeActorZeroRadius). Editor commandlets rebuild it
		// via NexusEnsureNavMeshBoundsBrush without changing this scale.
		const ENexusMapId MapId = FNexusMapCatalog::DetectFromWorld(World);
		const float NavS = FNexusMapCatalog::NavBoundsScale(MapId);
		NavBounds->SetActorScale3D(FVector(NavS, NavS, 8.f));
	}

	if (ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 2000.f), FRotator(-45.f, 40.f, 0.f), Params))
	{
		if (ULightComponent* Comp = Sun->GetLightComponent())
		{
			Comp->SetIntensity(10.f);
			Comp->SetCastShadows(true);
		}
	}
	if (ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector(0.f, 0.f, 1500.f), FRotator::ZeroRotator, Params))
	{
		if (USkyLightComponent* Comp = Sky->GetLightComponent())
		{
			Comp->SetIntensity(1.f);
			Comp->RecaptureSky();
		}
	}

	for (const FNexusSpawnDef& Spawn : Layout.Spawns)
	{
		const FVector Loc = NexusMeters(Spawn.LocationMeters) + FVector(0.f, 0.f, 92.f);
		ANexusTeamStart* Start = World->SpawnActor<ANexusTeamStart>(Loc, Spawn.Rotation, Params);
		if (Start)
		{
			Start->Team = Spawn.Team;
			Start->SpawnID = Spawn.SpawnID;
			Start->PreferredRoute = Spawn.PreferredRoute;
			Start->SpawnRotation = Spawn.Rotation;
			Start->SafeZoneRadius = Spawn.SafeZoneRadiusMeters * 100.f;
			Start->SafeZone->SetSphereRadius(Start->SafeZoneRadius);
			Start->SetActorRotation(Spawn.Rotation);
		}
	}

	for (const FNexusSiteDef& Site : Layout.Sites)
	{
		ANexusBombSite* BombSite = World->SpawnActor<ANexusBombSite>(NexusMeters(Site.CenterMeters) + FVector(0.f, 0.f, 150.f), FRotator::ZeroRotator, Params);
		if (BombSite)
		{
			BombSite->SiteID = Site.SiteId;
			BombSite->SiteVolume->SetBoxExtent(NexusMeters(Site.SizeMeters) * 0.5f);
			for (const FNexusPlantZoneDef& Zone : Site.PlantZones)
			{
				BombSite->AllowedPlantZones.Add(NexusMeters(Zone.CenterMeters));
			}
		}
		for (const FVector& Entrance : Site.EntranceCentersMeters)
		{
			if (ANexusTaggedVolume* Vol = World->SpawnActor<ANexusTaggedVolume>(NexusMeters(Entrance), FRotator::ZeroRotator, Params))
			{
				Vol->VolumeType = ENexusTaggedVolumeType::SiteEntrance;
				Vol->SiteId = Site.SiteId;
			}
		}
		for (const FVector& DefPos : Site.DefensivePositionsMeters)
		{
			if (ANexusTaggedVolume* Vol = World->SpawnActor<ANexusTaggedVolume>(NexusMeters(DefPos) + FVector(0.f, 0.f, 90.f), FRotator::ZeroRotator, Params))
			{
				Vol->VolumeType = ENexusTaggedVolumeType::DefensivePosition;
				Vol->SiteId = Site.SiteId;
			}
		}
	}

	for (const FNexusDoorDef& Door : Layout.Doors)
	{
		const float HalfH = (Door.Type == ENexusDoorType::Double || Door.Type == ENexusDoorType::Reinforced) ? 120.f : 105.f;
		ANexusDoor* Actor = World->SpawnActor<ANexusDoor>(NexusMeters(Door.LocationMeters) + FVector(0.f, 0.f, HalfH), Door.Rotation, Params);
		if (Actor)
		{
			Actor->DoorType = Door.Type;
		}
	}

	for (const FNexusCoverDef& Cover : Layout.Covers)
	{
		ANexusCover* Actor = World->SpawnActor<ANexusCover>(NexusMeters(Cover.LocationMeters), Cover.Rotation, Params);
		if (Actor)
		{
			Actor->CoverType = Cover.Type;
			Actor->WidthMeters = Cover.WidthMeters;
		}
	}

	for (const FNexusCalloutDef& Call : Layout.Callouts)
	{
		ANexusCalloutVolume* Actor = World->SpawnActor<ANexusCalloutVolume>(NexusMeters(Call.CenterMeters), FRotator::ZeroRotator, Params);
		if (Actor)
		{
			Actor->CalloutId = Call.CalloutId;
			Actor->DisplayName = Call.DisplayName;
			Actor->Volume->SetBoxExtent(NexusMeters(Call.ExtentMeters));
		}
	}

	for (const FNexusRouteDef& Route : Layout.Routes)
	{
		ANexusRouteSpline* Actor = World->SpawnActor<ANexusRouteSpline>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (!Actor)
		{
			continue;
		}
		Actor->RouteId = Route.RouteId;
		Actor->DisplayName = Route.DisplayName;
		Actor->Spline->ClearSplinePoints(false);
		for (int32 i = 0; i < Route.PointsMeters.Num(); ++i)
		{
			Actor->Spline->AddSplinePoint(NexusMeters(Route.PointsMeters[i]) + FVector(0.f, 0.f, 20.f), ESplineCoordinateSpace::World, false);
		}
		Actor->Spline->UpdateSpline();
	}
}
