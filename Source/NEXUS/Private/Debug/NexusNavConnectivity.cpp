#include "Debug/NexusNavConnectivity.h"
#include "Maps/NexusVelasqoLayout.h"
#include "Maps/NexusMapCatalog.h"
#include "Maps/NexusTeamStart.h"
#include "NexusTypes.h"
#include "NEXUS.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "NavMesh/RecastNavMesh.h"

struct FNexusNavProbe
{
	FString Name;
	FVector Raw = FVector::ZeroVector;
	FVector Proj = FVector::ZeroVector;
	NavNodeRef Poly = INVALID_NAVNODEREF;
	NavNodeRef Cluster = INVALID_NAVNODEREF;
	uint32 TileIndex = 0;
	uint32 PolyIndex = 0;
	int32 Neighbors = 0;
	bool bProjected = false;
};

static FNexusNavProbe MakeProbe(UNavigationSystemV1* NavSys, ARecastNavMesh* Recast, const FString& Name, const FVector& Raw)
{
	FNexusNavProbe P;
	P.Name = Name;
	P.Raw = Raw;
	FNavLocation Loc;
	P.bProjected = NavSys && NavSys->ProjectPointToNavigation(Raw, Loc, FVector(400.f, 400.f, 800.f));
	if (!P.bProjected && NavSys)
	{
		P.bProjected = NavSys->ProjectPointToNavigation(Raw, Loc, FVector(1500.f, 1500.f, 1500.f));
	}
	if (P.bProjected)
	{
		P.Proj = Loc.Location;
		P.Poly = Loc.NodeRef;
		if (Recast)
		{
			if (P.Poly == INVALID_NAVNODEREF)
			{
				P.Poly = Recast->FindNearestPoly(P.Proj, FVector(200.f, 200.f, 400.f));
			}
			Recast->GetPolyTileIndex(P.Poly, P.PolyIndex, P.TileIndex);
			P.Cluster = Recast->GetClusterRef(P.Poly);
			TArray<NavNodeRef> Nbrs;
			Recast->GetPolyNeighbors(P.Poly, Nbrs);
			P.Neighbors = Nbrs.Num();
		}
	}
	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NAV] PROBE %s RAW=%s PROJECTED=%s ok=%d POLY=%llu TILE=%u POLYIDX=%u CLUSTER=%llu NBR=%d"),
		*P.Name, *P.Raw.ToCompactString(), P.bProjected ? *P.Proj.ToCompactString() : TEXT("NONE"),
		P.bProjected ? 1 : 0, P.Poly, P.TileIndex, P.PolyIndex, P.Cluster, P.Neighbors);
	return P;
}

static FString PathCell(UNavigationSystemV1* NavSys, ANavigationData* NavData, ARecastNavMesh* Recast,
	const FNexusNavProbe& A, const FNexusNavProbe& B)
{
	if (!NavSys || !NavData || !A.bProjected || !B.bProjected)
	{
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NAV] PATH %s -> %s STATUS=FAILED CAUSE=off-mesh"), *A.Name, *B.Name);
		return TEXT("FAILED");
	}

	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NAV] PATH %s -> %s"), *A.Name, *B.Name);
	UE_LOG(LogNEXUS, Log, TEXT("  RAW START=%s RAW END=%s"), *A.Raw.ToCompactString(), *B.Raw.ToCompactString());
	UE_LOG(LogNEXUS, Log, TEXT("  PROJECTED START=%s PROJECTED END=%s"), *A.Proj.ToCompactString(), *B.Proj.ToCompactString());
	UE_LOG(LogNEXUS, Log, TEXT("  START POLY=%llu TILE=%u CLUSTER=%llu END POLY=%llu TILE=%u CLUSTER=%llu SAMEPOLY=%d SAMECLUSTER=%d"),
		A.Poly, A.TileIndex, A.Cluster, B.Poly, B.TileIndex, B.Cluster,
		(A.Poly != INVALID_NAVNODEREF && A.Poly == B.Poly) ? 1 : 0,
		(A.Cluster != INVALID_NAVNODEREF && A.Cluster == B.Cluster) ? 1 : 0);

	FPathFindingQuery Query(nullptr, *NavData, A.Proj, B.Proj);
	const FPathFindingResult Result = NavSys->FindPathSync(Query);
	const bool bValid = Result.IsSuccessful() && Result.Path.IsValid();
	if (!bValid)
	{
		UE_LOG(LogNEXUS, Log, TEXT("  PATH STATUS=FAILED ISVALID=0 ISPARTIAL=%d"), Result.IsPartial() ? 1 : 0);
		return TEXT("FAILED");
	}
	const TArray<FNavPathPoint>& Pts = Result.Path->GetPathPoints();
	const FVector PathEnd = Pts.Num() ? Pts.Last().Location : FVector::ZeroVector;
	const float LenM = Result.Path->GetLength() / 100.f;
	const float EndErrM = FVector::Dist2D(PathEnd, B.Proj) / 100.f;
	const bool bPartial = Result.IsPartial();
	UE_LOG(LogNEXUS, Log, TEXT("  PATH STATUS=OK POINTCOUNT=%d PATHLENGTH=%.1fm ENDERROR=%.1fm ISPARTIAL=%d ISVALID=1 NAVDATA=%s"),
		Pts.Num(), LenM, EndErrM, bPartial ? 1 : 0, *NavData->GetName());

	if (EndErrM <= 8.f && Pts.Num() >= 2)
	{
		return FString::Printf(TEXT("CONNECTED:%.1fm"), LenM);
	}
	return FString::Printf(TEXT("PARTIAL:%.1fm/err%.1f"), LenM, EndErrM);
}

void FNexusNavConnectivity::Run(UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [FAIL] System=NAV Test=Connectivity Expected=world Actual=null Cause=no world"));
		return;
	}
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	ANavigationData* NavData = NavSys ? NavSys->GetDefaultNavDataInstance() : nullptr;
	ARecastNavMesh* Recast = Cast<ARecastNavMesh>(NavData);
	if (!NavSys || !NavData)
	{
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [FAIL] System=NAV Test=Connectivity Expected=Recast Actual=none Cause=no NavData"));
		return;
	}
	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NAV] NavData=%s Recast=%s Tiles=%d"),
		*NavData->GetName(), Recast ? TEXT("yes") : TEXT("no"), Recast ? Recast->GetNumActiveTiles() : -1);

	ANexusTeamStart* Spawn = nullptr;
	for (TActorIterator<ANexusTeamStart> It(World); It; ++It)
	{
		if (It->Team == ENexusTeam::Attackers)
		{
			Spawn = *It;
			break;
		}
	}

	const FNexusVelasqoLayout Layout = FNexusMapCatalog::BuildLayout(FNexusMapCatalog::DetectFromWorld(World));
	const FNexusSiteDef* SiteA = nullptr;
	const FNexusSiteDef* SiteB = nullptr;
	for (const FNexusSiteDef& S : Layout.Sites)
	{
		if (S.SiteId == ENexusSiteId::A) { SiteA = &S; }
		if (S.SiteId == ENexusSiteId::B) { SiteB = &S; }
	}

	auto Elev = [](const FVector& Meters) { return NexusMeters(Meters) + FVector(0.f, 0.f, 80.f); };

	TArray<FNexusNavProbe> Named;
	if (Spawn)
	{
		Named.Add(MakeProbe(NavSys, Recast, TEXT("Spawn"), Spawn->GetActorLocation() + FVector(0.f, 0.f, 50.f)));
	}
	if (SiteA)
	{
		Named.Add(MakeProbe(NavSys, Recast, TEXT("A_Center"), Elev(SiteA->CenterMeters)));
		for (int32 i = 0; i < SiteA->EntranceCentersMeters.Num() && i < 3; ++i)
		{
			Named.Add(MakeProbe(NavSys, Recast, FString::Printf(TEXT("A_Entrance_%d"), i + 1), Elev(SiteA->EntranceCentersMeters[i])));
		}
	}
	if (SiteB)
	{
		Named.Add(MakeProbe(NavSys, Recast, TEXT("B_Center"), Elev(SiteB->CenterMeters)));
		for (int32 i = 0; i < SiteB->EntranceCentersMeters.Num() && i < 3; ++i)
		{
			Named.Add(MakeProbe(NavSys, Recast, FString::Printf(TEXT("B_Entrance_%d"), i + 1), Elev(SiteB->EntranceCentersMeters[i])));
		}
	}

	FNexusNavProbe* PSpawn = Named.FindByPredicate([](const FNexusNavProbe& P) { return P.Name == TEXT("Spawn"); });
	FNexusNavProbe* PA = Named.FindByPredicate([](const FNexusNavProbe& P) { return P.Name == TEXT("A_Center"); });
	FNexusNavProbe* PB = Named.FindByPredicate([](const FNexusNavProbe& P) { return P.Name == TEXT("B_Center"); });
	FNexusNavProbe* PA1 = Named.FindByPredicate([](const FNexusNavProbe& P) { return P.Name == TEXT("A_Entrance_1"); });
	FNexusNavProbe* PB1 = Named.FindByPredicate([](const FNexusNavProbe& P) { return P.Name == TEXT("B_Entrance_1"); });

	auto LogNamed = [&](const TCHAR* Test, FNexusNavProbe* From, FNexusNavProbe* To)
	{
		if (!From || !To)
		{
			UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [FAIL] System=NAV Test=%s Expected=probes Actual=missing Cause=layout point missing"), Test);
			return;
		}
		const FString Cell = PathCell(NavSys, NavData, Recast, *From, *To);
		const bool bOk = Cell.StartsWith(TEXT("CONNECTED"));
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [%s] System=NAV Test=%s Expected=CONNECTED Actual=%s Cause=%s"),
			bOk ? TEXT("PASS") : TEXT("FAIL"), Test, *Cell,
			bOk ? TEXT("-") : TEXT("see PATH dump above — no mesh/layout change"));
	};

	LogNamed(TEXT("Spawn -> Site A"), PSpawn, PA1 ? PA1 : PA);
	LogNamed(TEXT("Spawn -> Site B"), PSpawn, PB1 ? PB1 : PB);
	LogNamed(TEXT("A -> B"), PA, PB);
	LogNamed(TEXT("B -> A"), PB, PA);

	TArray<FNexusNavProbe> Matrix;
	const TCHAR* Labels[] = { TEXT("A1"), TEXT("A2"), TEXT("A3"), TEXT("B1"), TEXT("B2"), TEXT("B3") };
	const TCHAR* Sources[] = { TEXT("A_Entrance_1"), TEXT("A_Entrance_2"), TEXT("A_Entrance_3"), TEXT("B_Entrance_1"), TEXT("B_Entrance_2"), TEXT("B_Entrance_3") };
	for (int32 i = 0; i < 6; ++i)
	{
		if (FNexusNavProbe* Found = Named.FindByPredicate([&](const FNexusNavProbe& P) { return P.Name == Sources[i]; }))
		{
			FNexusNavProbe Copy = *Found;
			Copy.Name = Labels[i];
			Matrix.Add(Copy);
		}
	}

	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NAV] MATRIX header     A1            A2            A3            B1            B2            B3"));
	int32 Connected = 0, Partial = 0, Failed = 0, Cells = 0;
	for (int32 R = 0; R < Matrix.Num(); ++R)
	{
		FString Row = FString::Printf(TEXT("[NEXUS NAV] MATRIX %s"), *Matrix[R].Name);
		Row = Row.RightPad(22);
		for (int32 C = 0; C < Matrix.Num(); ++C)
		{
			if (R == C)
			{
				Row += TEXT(" SELF          ");
				continue;
			}
			const FString Cell = PathCell(NavSys, NavData, Recast, Matrix[R], Matrix[C]);
			++Cells;
			if (Cell.StartsWith(TEXT("CONNECTED"))) { ++Connected; }
			else if (Cell.StartsWith(TEXT("PARTIAL"))) { ++Partial; }
			else { ++Failed; }
			Row += FString::Printf(TEXT(" %-13s"), *Cell.Left(13));
		}
		UE_LOG(LogNEXUS, Log, TEXT("%s"), *Row);
	}

	const bool bMatrixUseful = Cells > 0;
	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [%s] System=NAV Test=ConnectivityMatrix Expected=cells logged Actual=CONNECTED=%d PARTIAL=%d FAILED=%d Cause=diagnostic only — NavMesh not modified"),
		bMatrixUseful ? TEXT("PASS") : TEXT("FAIL"), Connected, Partial, Failed);

	if (PA && PB && PA->Cluster != INVALID_NAVNODEREF && PB->Cluster != INVALID_NAVNODEREF && PA->Cluster != PB->Cluster)
	{
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NAV] CAUSE=E/F CLUSTER MISMATCH A_Center cluster=%llu B_Center cluster=%llu EXPECTED=same walkable cluster ACTUAL=disjoint Recast clusters LOCATION=A(%.0f,%.0f) B(%.0f,%.0f)"),
			PA->Cluster, PB->Cluster, PA->Proj.X, PA->Proj.Y, PB->Proj.X, PB->Proj.Y);
	}
}
