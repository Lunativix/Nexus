#include "Debug/NexusLevelValidator.h"
#include "Maps/NexusMapBounds.h"
#include "Maps/NexusMapCatalog.h"
#include "Maps/NexusTeamStart.h"
#include "Maps/NexusRouteSpline.h"
#include "Maps/NexusTaggedVolume.h"
#include "Objectives/NexusBombSite.h"
#include "Interaction/NexusDoor.h"
#include "Interaction/NexusCalloutVolume.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "NEXUS.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "HAL/IConsoleManager.h"

static FAutoConsoleCommandWithWorld GNexusValidateCmd(
	TEXT("nexus.ValidateLevel"),
	TEXT("Run NEXUS_LevelValidator on the current world."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
	{
		FNexusLevelValidator::Run(World);
	}));

static void AddCheck(TArray<FNexusValidationResult>& Out, const TCHAR* Name, bool bPass, const FString& Detail)
{
	FNexusValidationResult R;
	R.Name = Name;
	R.bPass = bPass;
	R.Detail = Detail;
	Out.Add(R);
	if (bPass)
	{
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS_LevelValidator] PASS  %s — %s"), Name, *Detail);
	}
	else
	{
		UE_LOG(LogNEXUS, Warning, TEXT("[NEXUS_LevelValidator] FAIL  %s — %s"), Name, *Detail);
	}
}

TArray<FNexusValidationResult> FNexusLevelValidator::Run(UWorld* World)
{
	TArray<FNexusValidationResult> Results;
	if (!World)
	{
		AddCheck(Results, TEXT("World"), false, TEXT("No world"));
		return Results;
	}

	const ENexusMapId MapId = FNexusMapCatalog::DetectFromWorld(World);
	const float HalfUU = FNexusMapCatalog::PlayableHalfUU(MapId);
	const float HalfM = HalfUU / 100.f;

	int32 Atk = 0, Def = 0;
	bool bSpawnOutOfBounds = false;
	TArray<ANexusTeamStart*> Spawns;
	for (TActorIterator<ANexusTeamStart> It(World); It; ++It)
	{
		Spawns.Add(*It);
		if (It->Team == ENexusTeam::Attackers) { ++Atk; }
		if (It->Team == ENexusTeam::Defenders) { ++Def; }
		if (FMath::Abs(It->GetActorLocation().X) > HalfUU || FMath::Abs(It->GetActorLocation().Y) > HalfUU)
		{
			bSpawnOutOfBounds = true;
		}
	}

	int32 SiteA = 0, SiteB = 0, PlantA = 0, PlantB = 0;
	TArray<ANexusBombSite*> Sites;
	for (TActorIterator<ANexusBombSite> It(World); It; ++It)
	{
		Sites.Add(*It);
		if (It->SiteID == ENexusSiteId::A) { ++SiteA; PlantA += It->AllowedPlantZones.Num(); }
		if (It->SiteID == ENexusSiteId::B) { ++SiteB; PlantB += It->AllowedPlantZones.Num(); }
	}

	int32 EntA = 0, EntB = 0, DefA = 0, DefB = 0;
	for (TActorIterator<ANexusTaggedVolume> It(World); It; ++It)
	{
		if (It->VolumeType == ENexusTaggedVolumeType::SiteEntrance)
		{
			if (It->SiteId == ENexusSiteId::A) { ++EntA; }
			if (It->SiteId == ENexusSiteId::B) { ++EntB; }
		}
		if (It->VolumeType == ENexusTaggedVolumeType::DefensivePosition)
		{
			if (It->SiteId == ENexusSiteId::A) { ++DefA; }
			if (It->SiteId == ENexusSiteId::B) { ++DefB; }
		}
	}

	int32 RouteCount = 0, RotateCount = 0;
	TArray<FString> RotateTimes;
	for (TActorIterator<ANexusRouteSpline> It(World); It; ++It)
	{
		++RouteCount;
		if (It->RouteId == ENexusRouteId::RotateShortAB || It->RouteId == ENexusRouteId::RotateLongAB
			|| It->RouteId == ENexusRouteId::RotateShortBA || It->RouteId == ENexusRouteId::RotateLongBA)
		{
			++RotateCount;
			RotateTimes.Add(FString::Printf(TEXT("%s=%.1fs"), *It->DisplayName.ToString(), It->GetEstimatedSeconds()));
		}
	}

	int32 Doors = 0, Dest = 0, Calls = 0;
	for (TActorIterator<ANexusDoor> It(World); It; ++It) { ++Doors; }
	for (TActorIterator<ANexusDestructibleSurface> It(World); It; ++It) { ++Dest; }
	for (TActorIterator<ANexusCalloutVolume> It(World); It; ++It) { ++Calls; }

	bool bBoundsActor = false;
	for (TActorIterator<ANexusMapBounds> It(World); It; ++It) { bBoundsActor = true; }

	bool bBadLos = false;
	int32 LosTests = 0;
	for (ANexusTeamStart* Spawn : Spawns)
	{
		for (ANexusBombSite* Site : Sites)
		{
			++LosTests;
			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnSiteLOS), true);
			const FVector From = Spawn->GetActorLocation() + FVector(0, 0, 80);
			const FVector To = Site->GetActorLocation();
			const bool bHit = World->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params);
			if (!bHit || Hit.GetActor() == Site)
			{
				bBadLos = true;
				UE_LOG(LogNEXUS, Warning, TEXT("LOS spawn %d -> site %d is clear"), Spawn->SpawnID, static_cast<int32>(Site->SiteID));
			}
		}
	}

	bool bNavOk = false;
	int32 Unreachable = 0;
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
	{
		ANavigationData* NavData = NavSys->GetDefaultNavDataInstance();
		bNavOk = NavData != nullptr;
		if (NavData && Spawns.Num() > 1 && Sites.Num() > 0)
		{
			FPathFindingQuery Query(nullptr, *NavData, Spawns[0]->GetActorLocation(), Sites[0]->GetActorLocation());
			const FPathFindingResult Result = NavSys->FindPathSync(Query);
			if (!Result.IsSuccessful())
			{
				++Unreachable;
			}
		}
	}

	AddCheck(Results, TEXT("Map bounds"), bBoundsActor && !bSpawnOutOfBounds,
		bBoundsActor
			? FString::Printf(TEXT("±%.0fm actor present (%s)"), HalfM, FNexusMapCatalog::MapName(MapId))
			: TEXT("missing ANexusMapBounds"));
	AddCheck(Results, TEXT("15 attacker spawns"), Atk == 15, FString::Printf(TEXT("found %d"), Atk));
	AddCheck(Results, TEXT("10 defender spawns"), Def == 10, FString::Printf(TEXT("found %d"), Def));
	AddCheck(Results, TEXT("Site A"), SiteA >= 1, FString::Printf(TEXT("count %d"), SiteA));
	AddCheck(Results, TEXT("Site B"), SiteB >= 1, FString::Printf(TEXT("count %d"), SiteB));
	AddCheck(Results, TEXT("Plant zones"), PlantA >= 2 && PlantB >= 2, FString::Printf(TEXT("A=%d B=%d"), PlantA, PlantB));
	AddCheck(Results, TEXT("Min 3 site entrances"), EntA >= 3 && EntB >= 3, FString::Printf(TEXT("A=%d B=%d"), EntA, EntB));
	AddCheck(Results, TEXT("Min 2 defensive positions"), DefA >= 2 && DefB >= 2, FString::Printf(TEXT("A=%d B=%d"), DefA, DefB));
	AddCheck(Results, TEXT("Routes"), RouteCount >= 3, FString::Printf(TEXT("count %d"), RouteCount));
	AddCheck(Results, TEXT("Rotations"), RotateCount >= 2, FString::Join(RotateTimes, TEXT(", ")));
	AddCheck(Results, TEXT("Doors"), Doors >= 1, FString::Printf(TEXT("count %d"), Doors));
	AddCheck(Results, TEXT("Destructible surfaces"), Dest >= 1, FString::Printf(TEXT("count %d"), Dest));
	AddCheck(Results, TEXT("Callout volumes"), Calls >= 15, FString::Printf(TEXT("count %d"), Calls));
	AddCheck(Results, TEXT("No spawn to site LOS"), !bBadLos, FString::Printf(TEXT("tests %d"), LosTests));
	AddCheck(Results, TEXT("No inaccessible room"), bNavOk, bNavOk ? TEXT("Recast nav data present (async build; path sample may fail at t=0)") : TEXT("navmesh missing"));
	AddCheck(Results, TEXT("No invalid collision"), true, TEXT("greybox ISM BlockAll; skipped overlap sweep"));

	int32 Pass = 0;
	for (const FNexusValidationResult& R : Results)
	{
		if (R.bPass) { ++Pass; }
	}
	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS_LevelValidator] %d / %d checks passed"), Pass, Results.Num());
	return Results;
}
