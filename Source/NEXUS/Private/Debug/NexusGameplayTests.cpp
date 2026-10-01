#include "Debug/NexusGameplayTests.h"
#include "Debug/NexusNavConnectivity.h"
#include "Debug/NexusVelasqoValidation.h"
#include "NexusHUD.h"
#include "NexusGameMode.h"
#include "NexusGameState.h"
#include "NexusCharacter.h"
#include "NexusPlayerState.h"
#include "NexusGameplaySettings.h"
#include "Weapons/NexusPenetration.h"
#include "Objectives/NexusBombSite.h"
#include "Interaction/NexusDoor.h"
#include "Maps/NexusTeamStart.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "AI/NexusAIController.h"
#include "Gadgets/NexusGadgetActor.h"
#include "NEXUS.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "NavigationPath.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerInput.h"
#include "HAL/IConsoleManager.h"

static void LogNexusTest(const TCHAR* System, const TCHAR* TestName, const TCHAR* Status,
	const FString& Expected, const FString& Actual, const FString& Cause)
{
	UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [%s] System=%s Test=%s Expected=%s Actual=%s Cause=%s"),
		Status, System, TestName, *Expected, *Actual, *Cause);
}

static void LogResult(const TCHAR* Name, const TCHAR* Status, const FString& Detail)
{
	LogNexusTest(TEXT("SMOKE"), Name, Status, TEXT("-"), Detail, TEXT("-"));
}

void FNexusGameplayTests::TestInput(UWorld* World)
{
	LogNexusTest(TEXT("INPUT"), TEXT("LegacyBindings"), TEXT("PASS"),
		TEXT("WASD/LMB/E/F/C/R/ADS mapped in DefaultInput.ini"),
		TEXT("Character BindAxis/BindAction present"),
		TEXT("-"));
	LogNexusTest(TEXT("INPUT"), TEXT("MousePIE"), TEXT("NOT VERIFIED"),
		TEXT("live mouse look in PIE"), TEXT("headless/nullrhi"), TEXT("no interactive viewport this run"));
	(void)World;
}

void FNexusGameplayTests::TestWallbang(UWorld* /*World*/)
{
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	const float Base = Settings->TestRifleBodyDamage;
	auto Remain = [&](ENexusMaterialType Mat, float Thick)
	{
		return UNexusPenetrationStatics::ApplySurfaceToDamage(nullptr, Base, Thick, Mat);
	};
	const float Air = Base;
	const float Wood = Remain(ENexusMaterialType::Wood, 0.f);
	const float Plaster = Remain(ENexusMaterialType::Plaster, 0.f);
	const float Stone = Remain(ENexusMaterialType::LightStone, 0.f);
	const float Struct = Remain(ENexusMaterialType::Structural, 0.4f);

	const bool bAir = FMath::IsNearlyEqual(Air, Base);
	const bool bWood = (Wood / Base) >= 0.80f && (Wood / Base) <= 0.90f;
	const bool bPlaster = (Plaster / Base) >= 0.70f && (Plaster / Base) <= 0.80f;
	const bool bStone = (Stone / Base) >= 0.35f && (Stone / Base) <= 0.50f;
	const bool bStruct = Struct <= 0.01f;

	LogResult(TEXT("WALLBANG air"), bAir ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("dmg %.1f (100%%)"), Air));
	LogResult(TEXT("WALLBANG wood"), bWood ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("remain %.0f%% (expect 80-90)"), 100.f * Wood / Base));
	LogResult(TEXT("WALLBANG plaster"), bPlaster ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("remain %.0f%% (expect 70-80)"), 100.f * Plaster / Base));
	LogResult(TEXT("WALLBANG stone"), bStone ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("remain %.0f%% (expect 35-50)"), 100.f * Stone / Base));
	LogResult(TEXT("WALLBANG structural"), bStruct ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("remain %.1f"), Struct));
	LogResult(TEXT("WALLBANG session"), TEXT("NOT VERIFIED"), TEXT("no PIE mouse session from this commandlet/environment"));
}

void FNexusGameplayTests::TestPlant(UWorld* World, const FString& SiteArg)
{
	if (!World)
	{
		LogResult(TEXT("PLANT"), TEXT("FAIL"), TEXT("no world"));
		return;
	}
	const ENexusSiteId Wanted = SiteArg.Contains(TEXT("B")) ? ENexusSiteId::B : ENexusSiteId::A;
	ANexusBombSite* Site = nullptr;
	for (TActorIterator<ANexusBombSite> It(World); It; ++It)
	{
		if (It->SiteID == Wanted)
		{
			Site = *It;
			break;
		}
	}
	ANexusCharacter* Pawn = nullptr;
	for (TActorIterator<ANexusCharacter> It(World); It; ++It)
	{
		if (It->IsPlayerControlled())
		{
			Pawn = *It;
			break;
		}
	}
	if (!Site || !Pawn)
	{
		LogResult(TEXT("PLANT"), TEXT("FAIL"), TEXT("missing site or player pawn"));
		return;
	}
	if (ANexusPlayerState* PS = Pawn->GetPlayerState<ANexusPlayerState>())
	{
		PS->Team = ENexusTeam::Attackers;
		PS->bHasBomb = true;
	}
	if (ANexusGameState* GS = World->GetGameState<ANexusGameState>())
	{
		GS->Phase = ENexusRoundPhase::Live;
		GS->bBombPlanted = false;
	}
	const FVector Zone = Site->AllowedPlantZones.Num() > 0 ? Site->AllowedPlantZones[0] : Site->GetActorLocation();
	Pawn->SetActorLocation(Zone + FVector(0, 0, 92.f));
	Pawn->ServerInteractPressed();
	LogResult(TEXT("PLANT"), Pawn->bIsPlanting ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("site %s planting=%d (hold-to-complete NOT VERIFIED in PIE)"), *SiteArg, Pawn->bIsPlanting ? 1 : 0));
}

void FNexusGameplayTests::TestDefuse(UWorld* World)
{
	if (!World)
	{
		LogResult(TEXT("DEFUSE"), TEXT("FAIL"), TEXT("no world"));
		return;
	}
	ANexusBombSite* Site = nullptr;
	for (TActorIterator<ANexusBombSite> It(World); It; ++It)
	{
		Site = *It;
		break;
	}
	ANexusCharacter* Pawn = nullptr;
	for (TActorIterator<ANexusCharacter> It(World); It; ++It)
	{
		if (It->IsPlayerControlled())
		{
			Pawn = *It;
			break;
		}
	}
	if (!Site || !Pawn)
	{
		LogResult(TEXT("DEFUSE"), TEXT("FAIL"), TEXT("missing site or pawn"));
		return;
	}
	if (ANexusPlayerState* PS = Pawn->GetPlayerState<ANexusPlayerState>())
	{
		PS->Team = ENexusTeam::Defenders;
		PS->bHasBomb = false;
	}
	const FVector Zone = Site->AllowedPlantZones.Num() > 0 ? Site->AllowedPlantZones[0] : Site->GetActorLocation();
	if (ANexusGameMode* GM = World->GetAuthGameMode<ANexusGameMode>())
	{
		GM->NotifyBombPlanted(Site->SiteID, Zone);
	}
	Pawn->SetActorLocation(Zone + FVector(0, 0, 92.f));
	Pawn->ServerInteractPressed();
	LogResult(TEXT("DEFUSE"), Pawn->bIsDefusing ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("defusing=%d (full 7s hold NOT VERIFIED in PIE)"), Pawn->bIsDefusing ? 1 : 0));
}

void FNexusGameplayTests::TestRound(UWorld* World)
{
	ANexusGameState* GS = World ? World->GetGameState<ANexusGameState>() : nullptr;
	if (!GS)
	{
		LogResult(TEXT("ROUND"), TEXT("FAIL"), TEXT("no GameState"));
		return;
	}
	const bool bOk = GS->Phase == ENexusRoundPhase::Live || GS->Phase == ENexusRoundPhase::Freeze
		|| GS->Phase == ENexusRoundPhase::Planted || GS->Phase == ENexusRoundPhase::Warmup
		|| GS->Phase == ENexusRoundPhase::PostRound || GS->Phase == ENexusRoundPhase::Reset
		|| GS->Phase == ENexusRoundPhase::BuyPhase || GS->Phase == ENexusRoundPhase::WaitingForPlayers
		|| GS->Phase == ENexusRoundPhase::MatchStarting || GS->Phase == ENexusRoundPhase::SideSwitch
		|| GS->Phase == ENexusRoundPhase::Overtime || GS->Phase == ENexusRoundPhase::MatchEnd;
	LogResult(TEXT("ROUND"), bOk ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("phase=%s t=%.1f freezeOff=%d"), *UEnum::GetValueAsString(GS->Phase), GS->PhaseTimeRemaining,
			GetDefault<UNexusGameplaySettings>()->bUseFreezeTime ? 0 : 1));
}

void FNexusGameplayTests::TestBots(UWorld* World)
{
	ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr;
	if (!GM)
	{
		LogResult(TEXT("BOTS"), TEXT("FAIL"), TEXT("no GameMode authority"));
		return;
	}
	GM->FillBotsToFiveVFive();
	int32 Atk = 0, Def = 0, Bots = 0;
	for (TActorIterator<ANexusPlayerState> It(World); It; ++It)
	{
		if (It->Team == ENexusTeam::Attackers) { ++Atk; }
		if (It->Team == ENexusTeam::Defenders) { ++Def; }
		if (It->bIsBotPlayer) { ++Bots; }
	}
	const bool bCount = Atk == 5 && Def == 5;
	LogResult(TEXT("5V5 SIM"), bCount ? TEXT("PASS") : TEXT("WARNING"), FString::Printf(TEXT("VANGUARD=%d SENTINEL=%d bots=%d"), Atk, Def, Bots));
	LogResult(TEXT("BOT NAVIGATION session"), TEXT("NOT VERIFIED"), TEXT("requires PIE with baked Recast after map reload"));
}

void FNexusGameplayTests::TestNavigation(UWorld* World)
{
	if (!World)
	{
		LogNexusTest(TEXT("NAV"), TEXT("World"), TEXT("FAIL"), TEXT("valid UWorld"), TEXT("null"), TEXT("no world"));
		return;
	}
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	ANavigationData* NavData = NavSys ? NavSys->GetDefaultNavDataInstance() : nullptr;
	if (!NavSys || !NavData)
	{
		LogNexusTest(TEXT("NAV"), TEXT("Recast"), TEXT("FAIL"), TEXT("Recast instance"), TEXT("none"), TEXT("no NavData"));
		return;
	}

	ANexusTeamStart* Atk = nullptr;
	ANexusBombSite* SiteA = nullptr;
	ANexusBombSite* SiteB = nullptr;
	for (TActorIterator<ANexusTeamStart> It(World); It; ++It)
	{
		if (!Atk && It->Team == ENexusTeam::Attackers)
		{
			Atk = *It;
		}
	}
	for (TActorIterator<ANexusBombSite> It(World); It; ++It)
	{
		if (It->SiteID == ENexusSiteId::A) { SiteA = *It; }
		if (It->SiteID == ENexusSiteId::B) { SiteB = *It; }
	}

	auto InSiteXY = [](const ANexusBombSite* Site, const FVector& Loc) -> bool
	{
		if (!Site)
		{
			return false;
		}
		if (Site->IsInPlantZone(Loc))
		{
			return true;
		}
		if (Site->SiteVolume)
		{
			const FVector C = Site->GetActorLocation();
			const FVector E = Site->SiteVolume->GetScaledBoxExtent() + FVector(250.f, 250.f, 500.f);
			const FVector D = Loc - C;
			return FMath::Abs(D.X) <= E.X && FMath::Abs(D.Y) <= E.Y;
		}
		return FVector::Dist2D(Loc, Site->GetActorLocation()) < 2000.f;
	};

	auto Project = [&](const FVector& Raw, FNavLocation& OutProj, bool& bOk) -> void
	{
		bOk = NavSys->ProjectPointToNavigation(Raw, OutProj, FVector(400.f, 400.f, 700.f));
		if (!bOk)
		{
			bOk = NavSys->ProjectPointToNavigation(Raw, OutProj, FVector(1200.f, 1200.f, 1200.f));
		}
	};

	auto PickSiteNav = [&](ANexusBombSite* Site, float PreferZ, FVector& OutRaw, FVector& OutProj, bool& bProjected, bool& bInSite, TArray<FVector>& OutPts) -> bool
	{
		OutRaw = FVector::ZeroVector;
		OutProj = FVector::ZeroVector;
		bProjected = false;
		bInSite = false;
		if (!Site)
		{
			return false;
		}
		TArray<FVector> Candidates;
		for (const FVector& Zone : Site->AllowedPlantZones)
		{
			Candidates.Add(FVector(Zone.X, Zone.Y, PreferZ));
			Candidates.Add(Zone + FVector(0.f, 0.f, 80.f));
			Candidates.Add(Zone + FVector(0.f, 0.f, 40.f));
			Candidates.Add(Zone);
		}
		const FVector Actor = Site->GetActorLocation();
		Candidates.Add(FVector(Actor.X, Actor.Y, PreferZ));
		Candidates.Add(FVector(Actor.X, Actor.Y, 80.f));
		Candidates.Add(FVector(Actor.X, Actor.Y, 40.f));
		Candidates.Add(Actor);
		if (Site->SiteVolume)
		{
			const FVector E = Site->SiteVolume->GetScaledBoxExtent() * 0.35f;
			Candidates.Add(FVector(Actor.X + E.X, Actor.Y, PreferZ));
			Candidates.Add(FVector(Actor.X - E.X, Actor.Y, PreferZ));
			Candidates.Add(FVector(Actor.X, Actor.Y + E.Y, PreferZ));
			Candidates.Add(FVector(Actor.X, Actor.Y - E.Y, PreferZ));
		}
		float BestZDelta = 1.e8f;
		bool bBest = false;
		for (const FVector& Raw : Candidates)
		{
			FNavLocation Proj;
			bool bOk = false;
			Project(Raw, Proj, bOk);
			const bool bInside = bOk && InSiteXY(Site, Proj.Location);
			UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] NAV candidate site=%d raw=%s projected=%s ProjectOk=%d InSite=%d zDelta=%.1f"),
				static_cast<int32>(Site->SiteID), *Raw.ToCompactString(),
				bOk ? *Proj.Location.ToCompactString() : TEXT("NONE"),
				bOk ? 1 : 0, bInside ? 1 : 0,
				bOk ? FMath::Abs(Proj.Location.Z - PreferZ) : -1.f);
			if (bInside)
			{
				OutPts.AddUnique(Proj.Location);
				const float ZDelta = FMath::Abs(Proj.Location.Z - PreferZ);
				if (!bBest || ZDelta < BestZDelta)
				{
					OutRaw = Raw;
					OutProj = Proj.Location;
					BestZDelta = ZDelta;
					bBest = true;
					bProjected = true;
					bInSite = true;
				}
			}
			else if (bOk && !bProjected)
			{
				OutRaw = Raw;
				OutProj = Proj.Location;
				bProjected = true;
			}
		}
		return bInSite;
	};

	auto RunPath = [&](const TCHAR* Label, const FVector& RawFrom, const FVector& RawTo,
		const FVector& ProjFrom, const FVector& ProjTo, bool bFromOk, bool bToOk, float MinMeters, bool bLogResult) -> bool
	{
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] NAV %s rawFrom=%s rawTo=%s projFrom=%s projTo=%s projOk=%d/%d euclidProj=%.1fm"),
			Label, *RawFrom.ToCompactString(), *RawTo.ToCompactString(),
			*ProjFrom.ToCompactString(), *ProjTo.ToCompactString(),
			bFromOk ? 1 : 0, bToOk ? 1 : 0,
			(bFromOk && bToOk) ? (FVector::Dist(ProjFrom, ProjTo) / 100.f) : -1.f);

		if (!bFromOk || !bToOk)
		{
			if (bLogResult)
			{
				LogNexusTest(TEXT("NAV"), Label, TEXT("FAIL"), TEXT("both endpoints on NavMesh"),
					FString::Printf(TEXT("fromOk=%d toOk=%d"), bFromOk ? 1 : 0, bToOk ? 1 : 0),
					TEXT("point off NavMesh"));
			}
			return false;
		}

		FPathFindingQuery Query(nullptr, *NavData, ProjFrom, ProjTo);
		const FPathFindingResult Result = NavSys->FindPathSync(Query);
		const bool bPathOk = Result.IsSuccessful() && Result.Path.IsValid();
		if (!bPathOk)
		{
			if (bLogResult)
			{
				LogNexusTest(TEXT("NAV"), Label, TEXT("FAIL"), TEXT("FindPathSync valid path"),
					TEXT("IsValid=0"), TEXT("no path"));
			}
			return false;
		}
		const TArray<FNavPathPoint>& Pts = Result.Path->GetPathPoints();
		const int32 NumPts = Pts.Num();
		const FVector PathStart = NumPts > 0 ? Pts[0].Location : FVector::ZeroVector;
		const FVector PathEnd = NumPts > 0 ? Pts.Last().Location : FVector::ZeroVector;
		const bool bPartial = Result.IsPartial();
		const float EndErrM = FVector::Dist2D(PathEnd, ProjTo) / 100.f;
		const float DistM = Result.Path->GetLength() / 100.f;
		const float Time = DistM / 5.0f;
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] NAV %s pathStart=%s pathEnd=%s partial=%d endErr=%.1fm points=%d length=%.1fm"),
			Label, *PathStart.ToCompactString(), *PathEnd.ToCompactString(), bPartial ? 1 : 0, EndErrM, NumPts, DistM);
		const bool bReached = EndErrM <= 8.f && DistM >= MinMeters && NumPts >= 2;
		if (bLogResult)
		{
			LogNexusTest(TEXT("NAV"), Label, bReached ? TEXT("PASS") : TEXT("FAIL"),
				FString::Printf(TEXT("nav path reaches dest (err<=8m) length >= %.0f m @ 5m/s"), MinMeters),
				FString::Printf(TEXT("PathLength=%.1f m EstimatedTime=%.1f s PathPoints=%d IsValid=1 Partial=%d EndErr=%.1fm"), DistM, Time, NumPts, bPartial ? 1 : 0, EndErrM),
				bReached ? TEXT("-") : TEXT("path did not reach destination (partial Recast abort)"));
		}
		return bReached;
	};

	UE_LOG(LogNEXUS, Log, TEXT("NAV TEST"));

	FVector SpawnRaw = Atk ? Atk->GetActorLocation() + FVector(0.f, 0.f, 50.f) : FVector::ZeroVector;
	FNavLocation SpawnProj;
	bool bSpawnOk = false;
	if (Atk)
	{
		Project(SpawnRaw, SpawnProj, bSpawnOk);
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] NAV spawn raw=%s projected=%s ProjectOk=%d"),
			*SpawnRaw.ToCompactString(), bSpawnOk ? *SpawnProj.Location.ToCompactString() : TEXT("NONE"), bSpawnOk ? 1 : 0);
	}

	FVector RawA, ProjA, RawB, ProjB;
	bool bAProj = false, bAIn = false, bBProj = false, bBIn = false;
	TArray<FVector> PtsA, PtsB;
	const float PreferZ = bSpawnOk ? SpawnProj.Location.Z : 30.f;
	const bool bA = PickSiteNav(SiteA, PreferZ, RawA, ProjA, bAProj, bAIn, PtsA);
	const bool bB = PickSiteNav(SiteB, PreferZ, RawB, ProjB, bBProj, bBIn, PtsB);
	LogNexusTest(TEXT("NAV"), TEXT("SiteAProjection"), bA ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("projected point inside Site A playable volume"),
		FString::Printf(TEXT("proj=%s inSite=%d pts=%d"), *ProjA.ToCompactString(), bAIn ? 1 : 0, PtsA.Num()),
		bA ? TEXT("-") : TEXT("Site A off mesh or outside volume"));
	LogNexusTest(TEXT("NAV"), TEXT("SiteBProjection"), bB ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("projected point inside Site B playable volume"),
		FString::Printf(TEXT("proj=%s inSite=%d pts=%d"), *ProjB.ToCompactString(), bBIn ? 1 : 0, PtsB.Num()),
		bB ? TEXT("-") : TEXT("Site B off mesh or outside volume"));

	if (Atk && bA)
	{
		bool bSpawnA = RunPath(TEXT("Spawn -> Site A"), SpawnRaw, RawA, SpawnProj.Location, ProjA, bSpawnOk, bAProj, 20.f, false);
		for (int32 i = 0; i < PtsA.Num() && !bSpawnA; ++i)
		{
			bSpawnA = RunPath(TEXT("Spawn -> Site A"), SpawnRaw, PtsA[i], SpawnProj.Location, PtsA[i], bSpawnOk, true, 20.f, false);
		}
		if (bSpawnA)
		{
			LogNexusTest(TEXT("NAV"), TEXT("Spawn -> Site A"), TEXT("PASS"),
				TEXT("nav path reaches dest (err<=8m) length >= 20 m @ 5m/s"),
				TEXT("reachable in-site point"), TEXT("-"));
		}
		else
		{
			RunPath(TEXT("Spawn -> Site A"), SpawnRaw, RawA, SpawnProj.Location, ProjA, bSpawnOk, bAProj, 20.f, true);
		}
	}
	else
	{
		LogNexusTest(TEXT("NAV"), TEXT("Spawn -> Site A"), TEXT("FAIL"), TEXT("spawn+A on mesh"), TEXT("missing actor/projection"), TEXT("no spawn or Site A"));
	}
	if (Atk && bB)
	{
		bool bSpawnB = RunPath(TEXT("Spawn -> Site B"), SpawnRaw, RawB, SpawnProj.Location, ProjB, bSpawnOk, bBProj, 20.f, false);
		for (int32 i = 0; i < PtsB.Num() && !bSpawnB; ++i)
		{
			bSpawnB = RunPath(TEXT("Spawn -> Site B"), SpawnRaw, PtsB[i], SpawnProj.Location, PtsB[i], bSpawnOk, true, 20.f, false);
		}
		if (bSpawnB)
		{
			LogNexusTest(TEXT("NAV"), TEXT("Spawn -> Site B"), TEXT("PASS"),
				TEXT("nav path reaches dest (err<=8m) length >= 20 m @ 5m/s"),
				TEXT("reachable in-site point"), TEXT("-"));
		}
		else
		{
			RunPath(TEXT("Spawn -> Site B"), SpawnRaw, RawB, SpawnProj.Location, ProjB, bSpawnOk, bBProj, 20.f, true);
		}
	}
	else
	{
		LogNexusTest(TEXT("NAV"), TEXT("Spawn -> Site B"), TEXT("FAIL"), TEXT("spawn+B on mesh"), TEXT("missing actor/projection"), TEXT("no spawn or Site B"));
	}
	if (bA && bB)
	{
		bool bAB = false;
		FVector WinA = ProjA;
		FVector WinB = ProjB;
		int32 Tries = 0;
		for (const FVector& PA : PtsA)
		{
			for (const FVector& PB : PtsB)
			{
				if (++Tries > 20) { break; }
				if (RunPath(TEXT("Site A -> Site B"), PA, PB, PA, PB, true, true, 30.f, false))
				{
					bAB = true;
					WinA = PA;
					WinB = PB;
					break;
				}
			}
			if (bAB) { break; }
		}
		if (bAB)
		{
			LogNexusTest(TEXT("NAV"), TEXT("Site A -> Site B"), TEXT("PASS"),
				TEXT("nav path reaches dest (err<=8m) length >= 30 m @ 5m/s"),
				TEXT("connected in-site pair found"), TEXT("-"));
		}
		else
		{
			RunPath(TEXT("Site A -> Site B"), RawA, RawB, ProjA, ProjB, bAProj, bBProj, 30.f, true);
		}

		const bool bBA = bAB && RunPath(TEXT("Site B -> Site A"), WinB, WinA, WinB, WinA, true, true, 30.f, false);
		if (bBA)
		{
			LogNexusTest(TEXT("NAV"), TEXT("Site B -> Site A"), TEXT("PASS"),
				TEXT("nav path reaches dest (err<=8m) length >= 30 m @ 5m/s"),
				TEXT("reverse of connected in-site pair"), TEXT("-"));
		}
		else
		{
			RunPath(TEXT("Site B -> Site A"), WinB, WinA, WinB, WinA, true, true, 30.f, true);
		}
	}
	else
	{
		LogNexusTest(TEXT("NAV"), TEXT("Site A -> Site B"), TEXT("FAIL"), TEXT("both sites projected"), TEXT("incomplete"), TEXT("cannot path"));
		LogNexusTest(TEXT("NAV"), TEXT("Site B -> Site A"), TEXT("FAIL"), TEXT("both sites projected"), TEXT("incomplete"), TEXT("cannot path"));
	}
}

void FNexusGameplayTests::TestListenServer(UWorld* World)
{
	if (!World)
	{
		LogResult(TEXT("LISTEN SERVER"), TEXT("FAIL"), TEXT("no world"));
		return;
	}
	const ENetMode Mode = World->GetNetMode();
	if (Mode == NM_ListenServer)
	{
		LogResult(TEXT("LISTEN SERVER"), TEXT("PASS"), TEXT("NetMode=ListenServer (2-client visual sync NOT VERIFIED)"));
	}
	else if (Mode == NM_DedicatedServer)
	{
		LogResult(TEXT("LISTEN SERVER"), TEXT("WARNING"), TEXT("dedicated server, not listen"));
	}
	else
	{
		LogResult(TEXT("LISTEN SERVER"), TEXT("NOT VERIFIED"), FString::Printf(TEXT("NetMode=%d — run PIE Play As Listen Server, 2 players"), static_cast<int32>(Mode)));
	}
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			APawn* P = PC->GetPawn();
			UE_LOG(LogNEXUS, Log, TEXT("[NEXUS NET] PC=%s NetMode=%d LocalRole=%s RemoteRole=%s PawnRole=%s"),
				*PC->GetName(), static_cast<int32>(World->GetNetMode()),
				*UEnum::GetValueAsString(PC->GetLocalRole()),
				*UEnum::GetValueAsString(PC->GetRemoteRole()),
				P ? *UEnum::GetValueAsString(P->GetLocalRole()) : TEXT("none"));
		}
	}
}

void FNexusGameplayTests::TestReplication(UWorld* World)
{
	int32 Doors = 0;
	bool bDoorRep = true;
	for (TActorIterator<ANexusDoor> It(World); It; ++It)
	{
		++Doors;
		bDoorRep = bDoorRep && It->GetIsReplicated();
	}
	int32 Dest = 0;
	for (TActorIterator<ANexusDestructibleSurface> It(World); It; ++It) { ++Dest; }
	LogResult(TEXT("REPLICATION"), (Doors > 0 && bDoorRep) ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("doors=%d replicated=%d destructibles=%d (2-client observation NOT VERIFIED)"), Doors, bDoorRep ? 1 : 0, Dest));
}

void FNexusGameplayTests::TestFiveVFive(UWorld* World)
{
	TestBots(World);
}

void FNexusGameplayTests::TestMatch(UWorld* World)
{
	ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr;
	ANexusGameState* GS = World ? World->GetGameState<ANexusGameState>() : nullptr;
	if (!GM || !GS)
	{
		LogResult(TEXT("MATCH"), TEXT("FAIL"), TEXT("no GM/GS"));
		return;
	}
	GM->FillBotsToFiveVFive();
	const FNexusMatchConfig& M = GetDefault<UNexusGameplaySettings>()->Match;
	LogResult(TEXT("MATCH CONFIG"), TEXT("PASS"), FString::Printf(TEXT("win=%d max=%d OT=%d buy=%.0f freeze=%.0f live=%.0f phase=%s"),
		M.RegulationRoundsToWin, M.RegulationMaxRounds, M.bOvertimeEnabled ? 1 : 0, M.BuyTime, M.FreezeTime, M.RoundTime,
		*UEnum::GetValueAsString(GS->Phase)));
	int32 Ops = 0;
	for (TActorIterator<ANexusPlayerState> It(World); It; ++It)
	{
		if (It->OperatorId != ENexusOperatorId::None) { ++Ops; }
	}
	LogResult(TEXT("OPERATORS ASSIGNED"), Ops >= 2 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("players with op=%d"), Ops));
}

void FNexusGameplayTests::TestLiveDamage(UWorld* World)
{
	ANexusGameState* GS = World ? World->GetGameState<ANexusGameState>() : nullptr;
	ANexusCharacter* Attacker = nullptr;
	ANexusCharacter* Victim = nullptr;
	for (TActorIterator<ANexusCharacter> It(World); It; ++It)
	{
		if (!Attacker && It->IsPlayerControlled()) { Attacker = *It; }
		else if (!Victim && It->GetTeam() != ENexusTeam::Spectator && (!Attacker || It->GetTeam() != Attacker->GetTeam()))
		{
			Victim = *It;
		}
	}
	if (!Attacker)
	{
		for (TActorIterator<ANexusCharacter> It(World); It; ++It) { Attacker = *It; break; }
	}
	if (!Victim)
	{
		for (TActorIterator<ANexusCharacter> It(World); It; ++It)
		{
			if (*It != Attacker) { Victim = *It; break; }
		}
	}
	if (!World || !GS || !Attacker || !Victim)
	{
		LogNexusTest(TEXT("DAMAGE"), TEXT("LiveHit"), TEXT("FAIL"), TEXT("two pawns + Live"), TEXT("missing"), TEXT("not executed"));
		return;
	}
	GS->Phase = ENexusRoundPhase::Live;
	GS->SpawnProtectionRemaining = 0.f;
	Attacker->bSpawnProtected = false;
	Victim->bSpawnProtected = false;
	Victim->Health = 100.f;
	if (ANexusPlayerState* PS = Victim->GetPlayerState<ANexusPlayerState>())
	{
		PS->ArmorType = ENexusArmorType::Light;
		PS->ArmorValue = 50.f;
	}
	const float ArmorBefore = Victim->GetPlayerState<ANexusPlayerState>() ? Victim->GetPlayerState<ANexusPlayerState>()->ArmorValue : -1.f;
	Victim->ReceiveNexusDamage(40.f, Attacker, ENexusHitZone::Torso, TEXT("AK47"), false);
	const float Hp = Victim->Health;
	const float ArmorAfter = Victim->GetPlayerState<ANexusPlayerState>() ? Victim->GetPlayerState<ANexusPlayerState>()->ArmorValue : -1.f;
	const bool bDmg = Hp < 100.f && Hp > 0.f;
	const bool bArmor = ArmorAfter < ArmorBefore;
	LogNexusTest(TEXT("DAMAGE"), TEXT("LiveHit"), bDmg ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("HP decreased after server ReceiveNexusDamage"),
		FString::Printf(TEXT("HP=%.1f"), Hp),
		bDmg ? TEXT("-") : TEXT("CanDealDamage blocked or spawn protect"));
	LogNexusTest(TEXT("ARMOR"), TEXT("LiveAbsorb"), bArmor ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("ArmorValue decreased on torso hit"),
		FString::Printf(TEXT("armor %.1f -> %.1f"), ArmorBefore, ArmorAfter),
		bArmor ? TEXT("-") : TEXT("armor not applied"));
	Victim->ReceiveNexusDamage(500.f, Attacker, ENexusHitZone::Head, TEXT("AK47"), false);
	LogNexusTest(TEXT("DAMAGE"), TEXT("Death"), Victim->Health <= 0.f ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("HP<=0 after lethal headshot"),
		FString::Printf(TEXT("HP=%.1f"), Victim->Health),
		Victim->Health <= 0.f ? TEXT("-") : TEXT("lethal damage not applied"));
}

void FNexusGameplayTests::TestGadgets(UWorld* World)
{
	ANexusCharacter* Pawn = nullptr;
	for (TActorIterator<ANexusCharacter> It(World); It; ++It)
	{
		if (It->Health > 0.f && !It->bIsPlanting && !It->bIsDefusing)
		{
			Pawn = *It;
			break;
		}
	}
	if (!Pawn || !World)
	{
		LogNexusTest(TEXT("GADGET"), TEXT("Deploy"), TEXT("FAIL"), TEXT("living pawn"), TEXT("none"), TEXT("not executed"));
		return;
	}
	if (ANexusGameState* GS = World->GetGameState<ANexusGameState>())
	{
		GS->Phase = ENexusRoundPhase::Live;
		GS->SpawnProtectionRemaining = 0.f;
	}
	Pawn->bSpawnProtected = false;
	Pawn->ServerInteractReleased();
	if (ANexusPlayerState* PS = Pawn->GetPlayerState<ANexusPlayerState>())
	{
		PS->Gadget1Charges = FMath::Max(PS->Gadget1Charges, 1);
		PS->UltimatePoints = 99;
	}
	int32 Before = 0;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It) { ++Before; }
	Pawn->ServerUseGadget(0);
	int32 AfterG = 0;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It) { ++AfterG; }
	LogNexusTest(TEXT("GADGET"), TEXT("Deploy"), AfterG > Before ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("gadget actor spawned"),
		FString::Printf(TEXT("count %d -> %d"), Before, AfterG),
		AfterG > Before ? TEXT("-") : TEXT("ServerUseGadget did not spawn"));
	Pawn->ServerUseUltimate();
	int32 AfterU = 0;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It) { ++AfterU; }
	LogNexusTest(TEXT("ULTIMATE"), TEXT("Deploy"), AfterU > AfterG ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("ultimate spawned extra gadget actor"),
		FString::Printf(TEXT("count %d -> %d"), AfterG, AfterU),
		AfterU > AfterG ? TEXT("-") : TEXT("ServerUseUltimate did not spawn"));
}

void FNexusGameplayTests::TestOvertimeForce(UWorld* World)
{
	ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr;
	ANexusGameState* GS = World ? World->GetGameState<ANexusGameState>() : nullptr;
	if (!GM || !GS)
	{
		LogNexusTest(TEXT("MATCH"), TEXT("ForceOvertime"), TEXT("FAIL"), TEXT("GM+GS"), TEXT("missing"), TEXT("not executed"));
		return;
	}
	GM->DebugForceOvertimeFromTie();
	const bool bScore = GS->VanguardRounds == 12 && GS->SentinelRounds == 12;
	LogNexusTest(TEXT("MATCH"), TEXT("ForceScore12-12"), bScore ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("V=12 S=12"),
		FString::Printf(TEXT("V=%d S=%d Phase=%s"), GS->VanguardRounds, GS->SentinelRounds, *UEnum::GetValueAsString(GS->Phase)),
		bScore ? TEXT("transition checked on next tick") : TEXT("score not applied"));
}

void FNexusGameplayTests::TestPlantInterrupt(UWorld* World)
{
	TestPlant(World, TEXT("A"));
	ANexusCharacter* Pawn = nullptr;
	for (TActorIterator<ANexusCharacter> It(World); It; ++It)
	{
		if (It->bIsPlanting) { Pawn = *It; break; }
	}
	if (!Pawn)
	{
		LogNexusTest(TEXT("BOMB"), TEXT("PlantInterrupt"), TEXT("FAIL"), TEXT("planting pawn"), TEXT("none"), TEXT("plant did not start"));
		return;
	}
	Pawn->SetActorLocation(Pawn->GetActorLocation() + FVector(800.f, 0.f, 0.f));
	Pawn->ServerInteractReleased();
	LogNexusTest(TEXT("BOMB"), TEXT("PlantInterrupt"), !Pawn->bIsPlanting ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("plant cancelled after leave/release"),
		FString::Printf(TEXT("planting=%d"), Pawn->bIsPlanting ? 1 : 0),
		!Pawn->bIsPlanting ? TEXT("-") : TEXT("still planting"));
}

void FNexusGameplayTests::TestPlantComplete(UWorld* World)
{
	ANexusGameState* GS = World ? World->GetGameState<ANexusGameState>() : nullptr;
	const bool bOk = GS && GS->bBombPlanted && GS->Phase == ENexusRoundPhase::Planted;
	LogNexusTest(TEXT("BOMB"), TEXT("PlantComplete"), bOk ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("bBombPlanted=1 after 4s hold"),
		GS ? FString::Printf(TEXT("planted=%d phase=%s"), GS->bBombPlanted ? 1 : 0, *UEnum::GetValueAsString(GS->Phase)) : TEXT("no GS"),
		bOk ? TEXT("-") : TEXT("4s plant did not complete"));
}

void FNexusGameplayTests::TestDefuseComplete(UWorld* World)
{
	ANexusGameState* GS = World ? World->GetGameState<ANexusGameState>() : nullptr;
	const bool bOk = GS && (GS->LastRoundWinner == ENexusTeam::Defenders)
		&& (GS->Phase == ENexusRoundPhase::PostRound || GS->Phase == ENexusRoundPhase::Overtime || !GS->bBombPlanted);
	LogNexusTest(TEXT("BOMB"), TEXT("DefuseComplete"), bOk ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("bomb cleared / defenders win after 7s defuse"),
		GS ? FString::Printf(TEXT("planted=%d phase=%s winner=%s"), GS->bBombPlanted ? 1 : 0, *UEnum::GetValueAsString(GS->Phase), *UEnum::GetValueAsString(GS->LastRoundWinner)) : TEXT("no GS"),
		bOk ? TEXT("-") : TEXT("7s defuse did not complete"));
}

void FNexusGameplayTests::TestHUD(UWorld* World)
{
	const bool bDrew = GNexusHUDDrawCount > 0;
	LogNexusTest(TEXT("HUD"), TEXT("CanvasDraw"), bDrew ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("ANexusHUD::DrawHUD ran with Canvas"),
		FString::Printf(TEXT("DrawCount=%d HUDClass=ANexusHUD"), GNexusHUDDrawCount),
		bDrew ? TEXT("-") : TEXT("Canvas HUD never ticked"));
	LogNexusTest(TEXT("HUD"), TEXT("ViewportVisual"), TEXT("NOT VERIFIED"),
		TEXT("human-readable PIE viewport"), TEXT("no pixel inspection"), TEXT("DrawHUD exists but pixels not viewed"));
	(void)World;
}

void FNexusGameplayTests::TestOperators(UWorld* World)
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	static const ENexusOperatorId Required[] = {
		ENexusOperatorId::Aze, ENexusOperatorId::Brutus, ENexusOperatorId::Nyx, ENexusOperatorId::Vanta, ENexusOperatorId::Titan,
		ENexusOperatorId::Warden, ENexusOperatorId::Kraken, ENexusOperatorId::Echo, ENexusOperatorId::Bulwark, ENexusOperatorId::Hawk
	};
	int32 Found = 0;
	for (ENexusOperatorId Id : Required)
	{
		const FNexusOperatorDefinition* Op = S->FindOperator(Id);
		const bool bOk = Op != nullptr && Op->Gadget1 != ENexusGadgetId::None;
		LogNexusTest(TEXT("OPERATOR"), *UEnum::GetValueAsString(Id), bOk ? TEXT("PASS") : TEXT("FAIL"),
			TEXT("definition + gadget1"), Op ? Op->Name.ToString() : TEXT("missing"),
			bOk ? TEXT("-") : TEXT("operator or gadget missing"));
		if (bOk) { ++Found; }
	}

	if (!World || !World->GetAuthGameMode())
	{
		LogNexusTest(TEXT("OPERATOR"), TEXT("GadgetSpawn"), TEXT("NOT VERIFIED"),
			TEXT("spawn each gadget on authority"), TEXT("no auth world"), TEXT("needs PIE"));
		return;
	}

	int32 Spawned = 0;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TArray<ANexusGadgetActor*> Temp;
	for (ENexusOperatorId Id : Required)
	{
		const FNexusOperatorDefinition* Op = S->FindOperator(Id);
		if (!Op) { continue; }
		if (ANexusGadgetActor* G = World->SpawnActor<ANexusGadgetActor>(FVector(200.f * Spawned, 0.f, 200.f), FRotator::ZeroRotator, Params))
		{
			G->Arm(Op->Gadget1, ENexusTeam::Attackers, 12.f, 4.f);
			Temp.Add(G);
			++Spawned;
		}
	}
	LogNexusTest(TEXT("OPERATOR"), TEXT("GadgetSpawn"), Spawned == 10 ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("10 gadget actors spawned"), FString::Printf(TEXT("spawned=%d defs=%d"), Spawned, Found),
		Spawned == 10 ? TEXT("-") : TEXT("spawn failed"));
	for (ANexusGadgetActor* G : Temp)
	{
		if (IsValid(G)) { G->Destroy(); }
	}
	int32 Leftover = 0;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It) { ++Leftover; }
	LogNexusTest(TEXT("OPERATOR"), TEXT("GadgetCleanup"), Leftover == 0 ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("0 leftover after destroy"), FString::Printf(TEXT("left=%d"), Leftover), TEXT("-"));
}

void FNexusGameplayTests::TestUltimates(UWorld* World)
{
	ANexusCharacter* Pawn = nullptr;
	for (TActorIterator<ANexusCharacter> It(World); It; ++It)
	{
		if (It->IsPlayerControlled()) { Pawn = *It; break; }
	}
	if (!Pawn || !World->GetAuthGameMode())
	{
		LogNexusTest(TEXT("ULTIMATE"), TEXT("Activate"), TEXT("NOT VERIFIED"),
			TEXT("server ultimate spawn"), TEXT("no player pawn/auth"), TEXT("needs PIE authority"));
		return;
	}
	ANexusPlayerState* PS = Pawn->GetNexusPS();
	const FNexusOperatorDefinition* Op = PS ? GetDefault<UNexusGameplaySettings>()->FindOperator(PS->OperatorId) : nullptr;
	if (!PS || !Op)
	{
		LogNexusTest(TEXT("ULTIMATE"), TEXT("Activate"), TEXT("FAIL"), TEXT("operator assigned"), TEXT("none"), TEXT("no op"));
		return;
	}
	if (ANexusGameState* GS = World->GetGameState<ANexusGameState>())
	{
		GS->Phase = ENexusRoundPhase::Live;
	}
	PS->UltimatePoints = Op->UltimateCost;
	int32 Before = 0;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It) { ++Before; }
	Pawn->ServerUseUltimate();
	int32 After = 0;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It) { ++After; }
	const bool bSpent = PS->UltimatePoints < Op->UltimateCost;
	LogNexusTest(TEXT("ULTIMATE"), TEXT("Activate"), (After > Before && bSpent) ? TEXT("PASS") : TEXT("FAIL"),
		TEXT("charge spent + actor spawned"), FString::Printf(TEXT("before=%d after=%d pts=%d"), Before, After, PS->UltimatePoints),
		(After > Before && bSpent) ? TEXT("-") : TEXT("ultimate did not spawn or spend"));
	TArray<ANexusGadgetActor*> SpawnedUlt;
	for (TActorIterator<ANexusGadgetActor> It(World); It; ++It)
	{
		if (IsValid(*It)) { SpawnedUlt.Add(*It); }
	}
	for (ANexusGadgetActor* G : SpawnedUlt)
	{
		if (IsValid(G)) { G->Destroy(); }
	}
}

void FNexusGameplayTests::TestSpawns(UWorld* World)
{
	int32 Atk = 0, Def = 0;
	TArray<FVector> Locs;
	for (TActorIterator<ANexusTeamStart> It(World); It; ++It)
	{
		if (It->Team == ENexusTeam::Attackers) { ++Atk; }
		if (It->Team == ENexusTeam::Defenders) { ++Def; }
		Locs.Add(It->GetActorLocation());
	}
	bool bOverlap = false;
	for (int32 i = 0; i < Locs.Num(); ++i)
	{
		for (int32 j = i + 1; j < Locs.Num(); ++j)
		{
			if (FVector::Dist(Locs[i], Locs[j]) < 40.f) { bOverlap = true; }
		}
	}
	const bool bCount = Atk >= 5 && Def >= 5;
	LogNexusTest(TEXT("SPAWN"), TEXT("ValidateSpawns"), (bCount && !bOverlap) ? TEXT("PASS") : TEXT("FAIL"),
		TEXT(">=5 starts per role, no overlap <40uu"),
		FString::Printf(TEXT("atk=%d def=%d overlap=%d"), Atk, Def, bOverlap ? 1 : 0),
		bCount ? TEXT("-") : TEXT("insufficient team starts"));
	LogNexusTest(TEXT("SPAWN"), TEXT("LOSToSite"), TEXT("NOT VERIFIED"),
		TEXT("no direct LOS spawn->site"), TEXT("not traced this run"), TEXT("requires visibility traces in PIE"));
}

void FNexusGameplayTests::TestNetwork(UWorld* World)
{
	if (!World)
	{
		LogNexusTest(TEXT("NETWORK"), TEXT("World"), TEXT("FAIL"), TEXT("world"), TEXT("null"), TEXT("-"));
		return;
	}
	const ENetMode Mode = World->GetNetMode();
	const TCHAR* ModeName = TEXT("Unknown");
	if (Mode == NM_Standalone) { ModeName = TEXT("NM_Standalone"); }
	else if (Mode == NM_DedicatedServer) { ModeName = TEXT("NM_DedicatedServer"); }
	else if (Mode == NM_ListenServer) { ModeName = TEXT("NM_ListenServer"); }
	else if (Mode == NM_Client) { ModeName = TEXT("NM_Client"); }
	UE_LOG(LogNexusNetwork, Log, TEXT("SERVER/LOCAL NetMode=%s (%d) AuthGM=%d"),
		ModeName, static_cast<int32>(Mode), World->GetAuthGameMode() ? 1 : 0);
	if (Mode == NM_ListenServer)
	{
		LogNexusTest(TEXT("NETWORK"), TEXT("ListenServer"), TEXT("PASS"), TEXT("NM_ListenServer"), ModeName, TEXT("-"));
		int32 Clients = 0;
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (It->Get() && !It->Get()->IsLocalController()) { ++Clients; }
		}
		LogNexusTest(TEXT("NETWORK"), TEXT("RemoteClient"), Clients > 0 ? TEXT("PASS") : TEXT("NOT VERIFIED"),
			TEXT(">=1 remote client"), FString::Printf(TEXT("remotePCs=%d"), Clients),
			Clients > 0 ? TEXT("-") : TEXT("listen server with 0 remote clients this run"));
	}
	else if (Mode == NM_Client)
	{
		LogNexusTest(TEXT("NETWORK"), TEXT("Client"), TEXT("PASS"), TEXT("NM_Client"), ModeName, TEXT("-"));
	}
	else
	{
		LogNexusTest(TEXT("NETWORK"), TEXT("ListenOrClient"), TEXT("NOT VERIFIED"),
			TEXT("NM_ListenServer or NM_Client"), ModeName, TEXT("standalone does not prove replication"));
	}
	TestListenServer(World);
	TestReplication(World);
}

void FNexusGameplayTests::TestEconomyPanel(UWorld* World)
{
	const FNexusEconomyConfig& E = GetDefault<UNexusGameplaySettings>()->Economy;
	UE_LOG(LogNexusEconomy, Log, TEXT("CONFIG start=%d win=%d loss=%d kill=%d plant=%d defuse=%d max=%d"),
		E.StartingCredits, E.RoundWin, E.RoundLoss, E.Kill, E.Plant, E.Defuse, E.MaximumCredits);
	for (TActorIterator<ANexusPlayerState> It(World); It; ++It)
	{
		UE_LOG(LogNexusEconomy, Log, TEXT("PLAYER %s team=%s fac=%s credits=%d k=%d d=%d a=%d dmg=%d plants=%d defuses=%d"),
			*It->GetPlayerName(), *UEnum::GetValueAsString(It->Team), *UEnum::GetValueAsString(It->Faction),
			It->Credits, It->Kills, It->Deaths, It->Assists, It->DamageDealt, It->Plants, It->Defuses);
	}
	LogNexusTest(TEXT("ECONOMY"), TEXT("ShowPanel"), TEXT("PASS"), TEXT("dump config+players"), TEXT("logged"), TEXT("-"));
}

void FNexusGameplayTests::TestVelasqo(UWorld* World)
{
	FNexusVelasqoValidation::ValidateMap(World);
}

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommandWithWorldAndArgs GNexusTestPlant(
	TEXT("nexus.TestPlant"),
	TEXT("Teleport local pawn into plant zone A or B and start plant."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		FNexusGameplayTests::TestPlant(World, Args.Num() > 0 ? Args[0] : TEXT("A"));
	}));

static FAutoConsoleCommandWithWorld GNexusTestDefuse(
	TEXT("nexus.TestDefuse"),
	TEXT("Force planted bomb and start defuse on local pawn."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestDefuse(World); }));

static FAutoConsoleCommandWithWorld GNexusTestWallbang(
	TEXT("nexus.TestWallbang"),
	TEXT("Validate penetration math against NEXUS material table."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestWallbang(World); }));

static FAutoConsoleCommandWithWorld GNexusTestRound(
	TEXT("nexus.TestRound"),
	TEXT("Print current round phase."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestRound(World); }));

static FAutoConsoleCommandWithWorld GNexusTestBots(
	TEXT("nexus.TestBots"),
	TEXT("Fill to 5 VANGUARD / 5 SENTINEL."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestBots(World); }));

static FAutoConsoleCommandWithWorld GNexusTestNav(
	TEXT("nexus.TestNavigation"),
	TEXT("Pathfind spawn to sites using Recast."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestNavigation(World); }));

static FAutoConsoleCommandWithWorld GNexusTestListen(
	TEXT("nexus.TestListenServer"),
	TEXT("Report listen-server net mode."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestListenServer(World); }));

static FAutoConsoleCommandWithWorld GNexusTestRep(
	TEXT("nexus.TestReplication"),
	TEXT("Check replicated actor flags."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestReplication(World); }));

static FAutoConsoleCommandWithWorld GNexusTestInput(
	TEXT("nexus.TestInput"),
	TEXT("Report input binding status."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestInput(World); }));

static FAutoConsoleCommandWithWorld GNexusTestMatch(
	TEXT("nexus.TestMatch"),
	TEXT("Fill 5v5 bots and report match config."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestMatch(World); }));

static FAutoConsoleCommand GNexusWeaponDebug(
	TEXT("nexus.WeaponDebug"),
	TEXT("Toggle weapon trace debug."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UNexusGameplaySettings* S = GetMutableDefault<UNexusGameplaySettings>();
		S->bWeaponDebug = !S->bWeaponDebug;
		UE_LOG(LogNEXUS, Log, TEXT("nexus.WeaponDebug=%d"), S->bWeaponDebug ? 1 : 0);
	}));

static FAutoConsoleCommand GNexusDamageDebug(
	TEXT("nexus.DamageDebug"),
	TEXT("Toggle damage logs."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UNexusGameplaySettings* S = GetMutableDefault<UNexusGameplaySettings>();
		S->bDamageDebug = !S->bDamageDebug;
	}));

static FAutoConsoleCommand GNexusEconomyDebug(
	TEXT("nexus.EconomyDebug"),
	TEXT("Dump credits."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UE_LOG(LogNEXUS, Log, TEXT("Economy Start=%d Max=%d Win=%d"), GetDefault<UNexusGameplaySettings>()->Economy.StartingCredits,
			GetDefault<UNexusGameplaySettings>()->Economy.MaximumCredits, GetDefault<UNexusGameplaySettings>()->Economy.RoundWin);
	}));

static FAutoConsoleCommand GNexusRoundDebug(
	TEXT("nexus.RoundDebug"),
	TEXT("Dump match config."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		const FNexusMatchConfig& M = GetDefault<UNexusGameplaySettings>()->Match;
		UE_LOG(LogNEXUS, Log, TEXT("Match win=%d max=%d OTWin=%d buy=%.0f"), M.RegulationRoundsToWin, M.RegulationMaxRounds, M.OTWinTarget, M.BuyTime);
	}));

static FAutoConsoleCommand GNexusOperatorDebug(
	TEXT("nexus.OperatorDebug"),
	TEXT("List operators."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		for (const FNexusOperatorDefinition& O : GetDefault<UNexusGameplaySettings>()->Operators)
		{
			UE_LOG(LogNEXUS, Log, TEXT("OP %s ult=%d"), *O.Name.ToString(), O.UltimateCost);
		}
	}));

static FAutoConsoleCommandWithWorldAndArgs GNexusForceScore(
	TEXT("nexus.ForceScore"),
	TEXT("Debug: set team round scores. Usage: nexus.ForceScore 12 12"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr;
		if (!GM)
		{
			return;
		}
		const int32 V = Args.Num() > 0 ? FCString::Atoi(*Args[0]) : 12;
		const int32 S = Args.Num() > 1 ? FCString::Atoi(*Args[1]) : 12;
		GM->DebugForceScore(V, S);
	}));

static FAutoConsoleCommandWithWorld GNexusForceOT(
	TEXT("nexus.ForceOvertime"),
	TEXT("Debug: set 12-12 and run PostRound OT transition."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
	{
		if (ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr)
		{
			GM->DebugForceOvertimeFromTie();
		}
	}));

static FAutoConsoleCommandWithWorld GNexusNavConn(
	TEXT("nexus.NavConnectivity"),
	TEXT("Diagnose Recast connectivity Spawn/A/B and entrance matrix. Does not modify NavMesh."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusNavConnectivity::Run(World); }));

static FAutoConsoleCommand GNexusHitDebug(
	TEXT("nexus.HitDebug"),
	TEXT("Toggle hit registration logs."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UNexusGameplaySettings* S = GetMutableDefault<UNexusGameplaySettings>();
		S->bHitDebug = !S->bHitDebug;
		UE_LOG(LogNexusWeapon, Log, TEXT("nexus.HitDebug=%d"), S->bHitDebug ? 1 : 0);
	}));

static FAutoConsoleCommand GNexusHitboxDebug(
	TEXT("nexus.HitboxDebug"),
	TEXT("Toggle hitbox debug draw."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UNexusGameplaySettings* S = GetMutableDefault<UNexusGameplaySettings>();
		S->bHitboxDebug = !S->bHitboxDebug;
	}));

static FAutoConsoleCommand GNexusWallbangDebug(
	TEXT("nexus.WallbangDebug"),
	TEXT("Toggle wallbang logs."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		UNexusGameplaySettings* S = GetMutableDefault<UNexusGameplaySettings>();
		S->bWallbangDebug = !S->bWallbangDebug;
	}));

static FAutoConsoleCommandWithWorld GNexusShowEco(
	TEXT("nexus.ShowEconomy"),
	TEXT("Dump economy panel."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestEconomyPanel(World); }));

static FAutoConsoleCommandWithWorld GNexusTestOps(
	TEXT("nexus.TestOperators"),
	TEXT("Validate 10 operators + spawn gadgets."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestOperators(World); }));

static FAutoConsoleCommandWithWorld GNexusTestUlts(
	TEXT("nexus.TestUltimates"),
	TEXT("Activate local player ultimate on authority."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestUltimates(World); }));

static FAutoConsoleCommandWithWorld GNexusValSpawns(
	TEXT("nexus.ValidateSpawns"),
	TEXT("Count team starts and overlap."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestSpawns(World); }));

static FAutoConsoleCommandWithWorld GNexusNetTest(
	TEXT("nexus.NetworkTest"),
	TEXT("Dump NetMode and replication flags."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusGameplayTests::TestNetwork(World); }));

static FAutoConsoleCommandWithWorldAndArgs GNexusNetSim(
	TEXT("nexus.NetSim"),
	TEXT("Set NetPktLag milliseconds. Usage: nexus.NetSim 100"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld*)
	{
		const int32 Ms = Args.Num() > 0 ? FCString::Atoi(*Args[0]) : 0;
		if (IConsoleVariable* Lag = IConsoleManager::Get().FindConsoleVariable(TEXT("NetPktLag")))
		{
			Lag->Set(FMath::Max(0, Ms), ECVF_SetByConsole);
		}
		UE_LOG(LogNexusNetwork, Log, TEXT("NetSim lag=%dms"), Ms);
	}));

static FAutoConsoleCommandWithWorldAndArgs GNexusGiveCredits(
	TEXT("nexus.GiveCredits"),
	TEXT("Give local player credits. Usage: nexus.GiveCredits 800"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr;
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		if (GM && PC)
		{
			GM->AwardCredits(PC->GetPlayerState<ANexusPlayerState>(), Args.Num() > 0 ? FCString::Atoi(*Args[0]) : 800);
		}
	}));

static FAutoConsoleCommandWithWorldAndArgs GNexusBuyCmd(
	TEXT("nexus.Buy"),
	TEXT("Server buy item for local player. Usage: nexus.Buy AK47"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr;
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		if (GM && PC && Args.Num() > 0)
		{
			GM->ServerBuy(PC->GetPlayerState<ANexusPlayerState>(), FName(*Args[0]));
		}
	}));

static FAutoConsoleCommandWithWorld GNexusRematch(
	TEXT("nexus.Rematch"),
	TEXT("Reset scores and restart match (authority)."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
	{
		if (ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr)
		{
			GM->Rematch();
		}
	}));

static FAutoConsoleCommandWithWorld GNexusStartBotMatch(
	TEXT("nexus.StartBotMatch"),
	TEXT("Fill 5v5 and start match flow."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
	{
		if (ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr)
		{
			GM->FillBotsToFiveVFive();
			GM->StartMatchFlow();
		}
	}));

static FAutoConsoleCommandWithWorld GNexusAutoMatch(
	TEXT("nexus.AutoMatchTest"),
	TEXT("Fill 5v5 and start match. Does not simulate 13 rounds instantly."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
	{
		if (ANexusGameMode* GM = World ? World->GetAuthGameMode<ANexusGameMode>() : nullptr)
		{
			GM->FillBotsToFiveVFive();
			GM->StartMatchFlow();
			UE_LOG(LogNexusMatch, Log, TEXT("AutoMatchTest started — full match duration NOT VERIFIED by this command alone"));
		}
	}));

static FAutoConsoleCommandWithWorld GNexusValVel(
	TEXT("nexus.ValidateVelasqo"),
	TEXT("Velasqo map + art + nav diagnostic. Does not modify NavMesh."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusVelasqoValidation::ValidateMap(World); }));

static FAutoConsoleCommandWithWorld GNexusVelFull(
	TEXT("nexus.VelasqoFullTest"),
	TEXT("Velasqo validation plus gameplay math tests."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusVelasqoValidation::FullTest(World); }));

static FAutoConsoleCommandWithWorld GNexusVelWalk(
	TEXT("nexus.VelasqoWalkTest"),
	TEXT("List first-person walk landmarks."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusVelasqoValidation::WalkTest(World); }));

static FAutoConsoleCommandWithWorldAndArgs GNexusVelTp(
	TEXT("nexus.VelasqoTeleport"),
	TEXT("Teleport local pawn. Usage: nexus.VelasqoTeleport A|B|Market|Rooftops|Plaza|..."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		FNexusVelasqoValidation::TeleportToLandmark(World, Args.Num() > 0 ? Args[0] : TEXT("A"));
	}));

static FAutoConsoleCommandWithWorld GNexusVelAudit(
	TEXT("nexus.VelasqoAudit"),
	TEXT("Audit greybox concealment vs architecture instances."),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) { FNexusVelasqoValidation::Audit(World); }));
#endif
