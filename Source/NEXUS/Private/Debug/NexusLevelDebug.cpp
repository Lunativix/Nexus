#include "Debug/NexusLevelDebug.h"
#include "Maps/NexusTeamStart.h"
#include "Maps/NexusRouteSpline.h"
#include "Maps/NexusTaggedVolume.h"
#include "Objectives/NexusBombSite.h"
#include "Interaction/NexusCalloutVolume.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NexusTypes.h"
#include "NEXUS.h"

TAutoConsoleVariable<int32> CVarNexusLevelDebug(
	TEXT("nexus.LevelDebug"),
	0,
	TEXT("NEXUS LEVEL DEBUG bitmask. -1 = all. 0 = off."),
	ECVF_Default);

ANexusLevelDebug::ANexusLevelDebug()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.15f;
	SetActorHiddenInGame(true);
}

void ANexusLevelDebug::Toggle(UWorld* World, int32 Mask)
{
	if (!World)
	{
		return;
	}
	ANexusLevelDebug* Existing = nullptr;
	for (TActorIterator<ANexusLevelDebug> It(World); It; ++It)
	{
		Existing = *It;
		break;
	}
	if (Mask == 0)
	{
		CVarNexusLevelDebug->Set(0);
		if (Existing)
		{
			Existing->Destroy();
		}
		return;
	}
	CVarNexusLevelDebug->Set(Mask);
	if (!Existing)
	{
		FActorSpawnParameters Params;
		Existing = World->SpawnActor<ANexusLevelDebug>(Params);
	}
	if (Existing)
	{
		Existing->ChannelMask = Mask;
	}
	UE_LOG(LogNEXUS, Log, TEXT("NEXUS LEVEL DEBUG mask=%d"), Mask);
}

void ANexusLevelDebug::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UWorld* World = GetWorld();
	const int32 Mask = (ChannelMask != 0) ? ChannelMask : CVarNexusLevelDebug.GetValueOnGameThread();
	if (!World || Mask == 0)
	{
		return;
	}

	const bool bAll = Mask < 0;
	auto On = [&](uint32 Bit) { return bAll || ((static_cast<uint32>(Mask) & Bit) != 0); };

	if (On(static_cast<uint32>(ENexusDebugChannel::Spawns)))
	{
		for (TActorIterator<ANexusTeamStart> It(World); It; ++It)
		{
			const FColor Color = It->Team == ENexusTeam::Attackers ? FColor::Red : FColor::Blue;
			DrawDebugCapsule(World, It->GetActorLocation(), 90.f, 30.f, FQuat::Identity, Color, false, 0.2f, 0, 4.f);
			DrawDebugSphere(World, It->GetActorLocation(), It->SafeZoneRadius, 12, Color, false, 0.2f, 0, 2.f);
		}
	}
	if (On(static_cast<uint32>(ENexusDebugChannel::Sites)) || On(static_cast<uint32>(ENexusDebugChannel::PlantZones)))
	{
		for (TActorIterator<ANexusBombSite> It(World); It; ++It)
		{
			DrawDebugBox(World, It->GetActorLocation(), It->SiteVolume ? It->SiteVolume->GetScaledBoxExtent() : FVector(400.f), FColor::Yellow, false, 0.2f, 0, 6.f);
			for (const FVector& Zone : It->AllowedPlantZones)
			{
				DrawDebugSphere(World, Zone + FVector(0, 0, 20), It->PlantRadius, 16, FColor::Orange, false, 0.2f, 0, 3.f);
			}
		}
	}
	if (On(static_cast<uint32>(ENexusDebugChannel::Routes)) || On(static_cast<uint32>(ENexusDebugChannel::RotationPaths)))
	{
		for (TActorIterator<ANexusRouteSpline> It(World); It; ++It)
		{
			const FColor Color = (It->RouteId == ENexusRouteId::RouteA) ? FColor::Red
				: (It->RouteId == ENexusRouteId::RouteB) ? FColor::Green
				: (It->RouteId == ENexusRouteId::RouteC) ? FColor::Cyan
				: FColor::Magenta;
			const int32 Pts = It->Spline->GetNumberOfSplinePoints();
			for (int32 i = 1; i < Pts; ++i)
			{
				DrawDebugLine(World,
					It->Spline->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::World),
					It->Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World),
					Color, false, 0.2f, 0, 8.f);
			}
		}
	}
	if (On(static_cast<uint32>(ENexusDebugChannel::Callouts)))
	{
		for (TActorIterator<ANexusCalloutVolume> It(World); It; ++It)
		{
			DrawDebugBox(World, It->GetActorLocation(), It->Volume->GetScaledBoxExtent(), FColor::Silver, false, 0.2f, 0, 2.f);
		}
	}
	if (On(static_cast<uint32>(ENexusDebugChannel::DestructibleWalls)) || On(static_cast<uint32>(ENexusDebugChannel::PenetrationMaterials)))
	{
		for (TActorIterator<ANexusDestructibleSurface> It(World); It; ++It)
		{
			DrawDebugBox(World, It->GetActorLocation(), It->Mesh->Bounds.BoxExtent, FColor::Purple, false, 0.2f, 0, 3.f);
		}
	}
	if (On(static_cast<uint32>(ENexusDebugChannel::NavMesh)))
	{
		if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
		{
			if (ANavigationData* Nav = NavSys->GetDefaultNavDataInstance())
			{
				DrawDebugBox(World, Nav->GetBounds().GetCenter(), Nav->GetBounds().GetExtent(), FColor::Cyan, false, 0.2f, 0, 2.f);
			}
		}
		for (TActorIterator<ANavMeshBoundsVolume> It(World); It; ++It)
		{
			DrawDebugBox(World, It->GetActorLocation(), It->GetActorScale3D() * 100.f, FColor::Blue, false, 0.2f, 0, 3.f);
		}
	}
	if (On(static_cast<uint32>(ENexusDebugChannel::LineOfSight)))
	{
		TArray<ANexusTeamStart*> Spawns;
		TArray<ANexusBombSite*> Sites;
		for (TActorIterator<ANexusTeamStart> It(World); It; ++It) { Spawns.Add(*It); }
		for (TActorIterator<ANexusBombSite> It(World); It; ++It) { Sites.Add(*It); }
		for (ANexusTeamStart* S : Spawns)
		{
			for (ANexusBombSite* Site : Sites)
			{
				FHitResult Hit;
				FCollisionQueryParams Params(SCENE_QUERY_STAT(DbgLOS), true);
				const FVector From = S->GetActorLocation() + FVector(0, 0, 80);
				const FVector To = Site->GetActorLocation();
				World->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params);
				const FColor Color = (!Hit.bBlockingHit || Hit.GetActor() == Site) ? FColor::Red : FColor::Green;
				DrawDebugLine(World, From, Hit.bBlockingHit ? Hit.ImpactPoint : To, Color, false, 0.2f, 0, 2.f);
			}
		}
		for (TActorIterator<ANexusTaggedVolume> It(World); It; ++It)
		{
			if (It->VolumeType == ENexusTaggedVolumeType::SiteEntrance)
			{
				DrawDebugBox(World, It->GetActorLocation(), FVector(80.f), FColor::Green, false, 0.2f, 0, 3.f);
			}
		}
	}
}
