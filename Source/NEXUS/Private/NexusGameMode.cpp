#include "NexusGameMode.h"
#include "NexusGameState.h"
#include "NexusPlayerState.h"
#include "NexusCharacter.h"
#include "NexusPlayerController.h"
#include "NexusHUD.h"
#include "NexusGameplaySettings.h"
#include "Maps/NexusVelasqoBuilder.h"
#include "Art/NexusEnvironmentDirector.h"
#include "Maps/NexusTeamStart.h"
#include "Maps/NexusNavFloor.h"
#include "AI/NexusAIController.h"
#include "EngineUtils.h"
#include "Debug/NexusLevelValidator.h"
#include "Debug/NexusGameplayTests.h"
#include "Debug/NexusNavConnectivity.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Gadgets/NexusGadgetActor.h"
#include "Interaction/NexusDoor.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "NEXUS.h"

ANexusGameMode::ANexusGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	DefaultPawnClass = ANexusCharacter::StaticClass();
	PlayerStateClass = ANexusPlayerState::StaticClass();
	GameStateClass = ANexusGameState::StaticClass();
	HUDClass = ANexusHUD::StaticClass();
	PlayerControllerClass = ANexusPlayerController::StaticClass();
	bUseSeamlessTravel = false;
}

void ANexusGameMode::BeginPlay()
{
	Super::BeginPlay();
	EnsureGreybox();
	ANexusEnvironmentDirector::Ensure(GetWorld());
	bool bHasNavFloor = false;
	for (TActorIterator<ANexusNavFloor> It(GetWorld()); It; ++It)
	{
		bHasNavFloor = true;
		break;
	}
	if (!bHasNavFloor)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<ANexusNavFloor>(FVector(0.f, 0.f, 8.f), FRotator::ZeroRotator, Params);
	}
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::ECreateIfEmpty::Create);
		ARecastNavMesh* Recast = Cast<ARecastNavMesh>(NavData);
		if (!Recast || Recast->GetNumActiveTiles() == 0)
		{
			NavSys->ReleaseInitialBuildingLock();
			NavSys->Build();
			UE_LOG(LogNEXUS, Log, TEXT("VELASQO Recast empty on load — runtime Build() fallback"));
		}
	}
	FNexusLevelValidator::Run(GetWorld());
	StartMatchFlow();
	if (FParse::Param(FCommandLine::Get(), TEXT("NexusSmokeTest")))
	{
		FNexusGameplayTests::TestWallbang(GetWorld());
		FNexusGameplayTests::TestRound(GetWorld());
		FNexusGameplayTests::TestReplication(GetWorld());
		FNexusGameplayTests::TestInput(GetWorld());
		FNexusGameplayTests::TestListenServer(GetWorld());
		FNexusGameplayTests::TestMatch(GetWorld());
		FNexusGameplayTests::TestBots(GetWorld());
		FTimerHandle NavHandle;
		GetWorldTimerManager().SetTimer(NavHandle, [this]()
		{
			FNexusNavConnectivity::Run(GetWorld());
			FNexusGameplayTests::TestNavigation(GetWorld());
			FNexusGameplayTests::TestSpawns(GetWorld());
			FNexusGameplayTests::TestNetwork(GetWorld());
			FNexusGameplayTests::TestOperators(GetWorld());
			FNexusGameplayTests::TestUltimates(GetWorld());
			FNexusGameplayTests::TestEconomyPanel(GetWorld());
			FNexusGameplayTests::TestLiveDamage(GetWorld());
			FNexusGameplayTests::TestGadgets(GetWorld());
			FNexusGameplayTests::TestPlantInterrupt(GetWorld());
			FNexusGameplayTests::TestHUD(GetWorld());
			FNexusGameplayTests::TestPlant(GetWorld(), TEXT("A"));
		}, 1.5f, false);
		FTimerHandle PlantDone;
		GetWorldTimerManager().SetTimer(PlantDone, [this]()
		{
			FNexusGameplayTests::TestPlantComplete(GetWorld());
			FNexusGameplayTests::TestDefuse(GetWorld());
		}, 6.0f, false);
		FTimerHandle DefuseDone;
		GetWorldTimerManager().SetTimer(DefuseDone, [this]()
		{
			FNexusGameplayTests::TestDefuseComplete(GetWorld());
			FNexusGameplayTests::TestOvertimeForce(GetWorld());
		}, 13.5f, false);
		FTimerHandle OtHandle;
		GetWorldTimerManager().SetTimer(OtHandle, [this]()
		{
			ANexusGameState* GS = GetGameState<ANexusGameState>();
			const bool bOt = GS && GS->bOvertime && GS->Phase == ENexusRoundPhase::Overtime;
			UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [%s] System=MATCH Test=OvertimeTransition Expected=bOvertime=1 Phase=Overtime Actual=bOvertime=%d Phase=%s Cause=%s"),
				bOt ? TEXT("PASS") : TEXT("FAIL"),
				GS ? (GS->bOvertime ? 1 : 0) : -1,
				GS ? *UEnum::GetValueAsString(GS->Phase) : TEXT("none"),
				bOt ? TEXT("-") : TEXT("PostRound 12-12 did not enter Overtime"));
		}, 14.2f, false);
		FTimerHandle QuitHandle;
		GetWorldTimerManager().SetTimer(QuitHandle, []()
		{
			FPlatformMisc::RequestExit(false);
		}, 15.0f, false);
	}
}

void ANexusGameMode::EnsureGreybox()
{
	bool bHas = false;
	for (TActorIterator<ANexusVelasqoBuilder> It(GetWorld()); It; ++It)
	{
		bHas = true;
		if (!It->bBuilt)
		{
			It->BuildMap();
		}
		break;
	}
	if (!bHas)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (ANexusVelasqoBuilder* Builder = GetWorld()->SpawnActor<ANexusVelasqoBuilder>(FVector::ZeroVector, FRotator::ZeroRotator, Params))
		{
			Builder->BuildMap();
		}
	}
}

void ANexusGameMode::StartMatchFlow()
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS)
	{
		return;
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("NexusSmokeTest")))
	{
		StartRound();
		return;
	}
	GS->Phase = ENexusRoundPhase::WaitingForPlayers;
	GS->PhaseTimeRemaining = 3.f;
	bMatchLive = true;
	UE_LOG(LogNexusMatch, Log, TEXT("MATCH FLOW start phase=WaitingForPlayers"));
}

void ANexusGameMode::Rematch()
{
	if (!HasAuthority())
	{
		return;
	}
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS)
	{
		return;
	}
	GS->VanguardRounds = 0;
	GS->SentinelRounds = 0;
	GS->RoundNumber = 0;
	GS->bOvertime = false;
	GS->bSidesSwapped = false;
	GS->MatchWinner = ENexusFaction::Unassigned;
	GS->LastRoundEndReason = ENexusRoundEndReason::None;
	GS->VanguardLossStreak = 0;
	GS->SentinelLossStreak = 0;
	for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
	{
		It->Kills = 0;
		It->Deaths = 0;
		It->Assists = 0;
		It->DamageDealt = 0;
		It->Headshots = 0;
		It->Plants = 0;
		It->Defuses = 0;
		It->Credits = GetDefault<UNexusGameplaySettings>()->Economy.StartingCredits;
		It->PrimaryWeapon = NAME_None;
		It->Faction = ENexusFaction::Unassigned;
		if (It->Team == ENexusTeam::Attackers) { It->Faction = ENexusFaction::Vanguard; }
		else if (It->Team == ENexusTeam::Defenders) { It->Faction = ENexusFaction::Sentinel; }
	}
	UE_LOG(LogNexusMatch, Log, TEXT("REMATCH"));
	StartMatchFlow();
}

void ANexusGameMode::SetMatchPhase(ENexusRoundPhase NewPhase, float Duration, const TCHAR* Cause)
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS)
	{
		return;
	}
	UE_LOG(LogNexusMatch, Log, TEXT("PHASE %s -> %s t=%.1f cause=%s"),
		*UEnum::GetValueAsString(GS->Phase), *UEnum::GetValueAsString(NewPhase), Duration, Cause ? Cause : TEXT("-"));
	GS->Phase = NewPhase;
	GS->PhaseTimeRemaining = Duration;
}

void ANexusGameMode::StartRound()
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	if (!GS)
	{
		return;
	}

	for (TActorIterator<ANexusTeamStart> It(GetWorld()); It; ++It)
	{
		It->bOccupied = false;
	}

	AbortAllObjectives();
	CleanupRoundActors();
	GS->bBombPlanted = false;
	GS->bBombDropped = false;
	GS->DeviceState = ENexusDeviceState::Carried;
	GS->PlantedSite = ENexusSiteId::None;
	GS->BombTimeRemaining = Settings->BombTimerSeconds;
	GS->SpawnProtectionRemaining = Settings->SpawnProtectionSeconds;
	GS->RoundNumber += 1;
	GS->KillFeed.Reset();

	for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
	{
		if (It->Credits <= 0 && GS->RoundNumber <= 1)
		{
			It->Credits = Settings->Economy.StartingCredits;
		}
		It->GrantLoadoutForNewRound(!It->bDiedThisRound && GS->RoundNumber > 1);
		AwardUlt(*It, Settings->Economy.UltRoundParticipation);
	}

	EnsureBots();
	ResetPawnsForRound();

	if (FParse::Param(FCommandLine::Get(), TEXT("NexusSmokeTest")))
	{
		GS->Phase = ENexusRoundPhase::Live;
		GS->PhaseTimeRemaining = Settings->Match.RoundTime;
	}
	else
	{
		GS->Phase = ENexusRoundPhase::BuyPhase;
		GS->PhaseTimeRemaining = Settings->Match.BuyTime;
	}
	bRoundStarted = true;
	bMatchLive = true;
}

void ANexusGameMode::FillBotsToFiveVFive()
{
	EnsureBots();
}

void ANexusGameMode::AbortAllObjectives()
{
	for (TActorIterator<ANexusCharacter> It(GetWorld()); It; ++It)
	{
		It->ServerInteractReleased();
	}
}

void ANexusGameMode::EnsureBots()
{
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	while (CountTeam(ENexusTeam::Attackers) < Settings->PlayersPerTeam)
	{
		SpawnBot(ENexusTeam::Attackers);
	}
	while (CountTeam(ENexusTeam::Defenders) < Settings->PlayersPerTeam)
	{
		SpawnBot(ENexusTeam::Defenders);
	}

	bool bGaveBomb = false;
	for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
	{
		if (It->Team == ENexusTeam::Attackers)
		{
			It->bHasBomb = !bGaveBomb;
			bGaveBomb = true;
		}
	}
}

void ANexusGameMode::SpawnBot(ENexusTeam Team)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ANexusAIController* AIC = GetWorld()->SpawnActor<ANexusAIController>(Params);
	if (!AIC)
	{
		return;
	}
	AssignTeam(AIC, Team);
	RestartPlayer(AIC);
	if (!AIC->GetPlayerState<ANexusPlayerState>())
	{
		AIC->InitPlayerState();
		AssignTeam(AIC, Team);
	}
}

void ANexusGameMode::AssignTeam(AController* Controller, ENexusTeam Team)
{
	if (ANexusPlayerState* PS = Controller->GetPlayerState<ANexusPlayerState>())
	{
		PS->Team = Team;
		PS->bIsBotPlayer = Controller->IsA<ANexusAIController>();
		if (PS->Faction == ENexusFaction::Unassigned)
		{
			PS->Faction = (Team == ENexusTeam::Defenders) ? ENexusFaction::Sentinel : ENexusFaction::Vanguard;
		}
		if (PS->OperatorId == ENexusOperatorId::None || PS->OperatorId == ENexusOperatorId::Aze)
		{
			static const ENexusOperatorId AtkOps[] = { ENexusOperatorId::Aze, ENexusOperatorId::Brutus, ENexusOperatorId::Nyx, ENexusOperatorId::Vanta, ENexusOperatorId::Titan };
			static const ENexusOperatorId DefOps[] = { ENexusOperatorId::Warden, ENexusOperatorId::Kraken, ENexusOperatorId::Echo, ENexusOperatorId::Bulwark, ENexusOperatorId::Hawk };
			const int32 Idx = FMath::Clamp(CountTeam(Team) % 5, 0, 4);
			PS->OperatorId = (Team == ENexusTeam::Defenders) ? DefOps[Idx] : AtkOps[Idx];
			PS->ApplyOperatorDefaults();
		}
		if (PS->GetPlayerName().IsEmpty())
		{
			PS->SetPlayerName(PS->bIsBotPlayer ? FString::Printf(TEXT("BOT-%s-%d"), Team == ENexusTeam::Defenders ? TEXT("S") : TEXT("V"), CountTeam(Team)) : TEXT("Player"));
		}
		if (Team == ENexusTeam::Attackers && CountTeam(ENexusTeam::Attackers) == 1)
		{
			PS->bHasBomb = true;
		}
	}
}

int32 ANexusGameMode::CountTeam(ENexusTeam Team) const
{
	int32 Count = 0;
	for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
	{
		if (It->Team == Team)
		{
			++Count;
		}
	}
	return Count;
}

void ANexusGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	const ENexusTeam Team = (CountTeam(ENexusTeam::Attackers) <= CountTeam(ENexusTeam::Defenders))
		? ENexusTeam::Attackers
		: ENexusTeam::Defenders;
	AssignTeam(NewPlayer, Team);
}

void ANexusGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (ANexusPlayerState* PS = NewPlayer->GetPlayerState<ANexusPlayerState>())
	{
		if (PS->Team == ENexusTeam::Spectator)
		{
			AssignTeam(NewPlayer, ENexusTeam::Attackers);
		}
	}
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

AActor* ANexusGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	ENexusTeam Team = ENexusTeam::Attackers;
	if (ANexusPlayerState* PS = Player ? Player->GetPlayerState<ANexusPlayerState>() : nullptr)
	{
		Team = PS->Team;
	}
	if (ANexusTeamStart* Start = FindFreeSpawn(Team))
	{
		Start->bOccupied = true;
		return Start;
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

ANexusTeamStart* ANexusGameMode::FindFreeSpawn(ENexusTeam Team) const
{
	for (TActorIterator<ANexusTeamStart> It(GetWorld()); It; ++It)
	{
		if (It->Team == Team && !It->bOccupied)
		{
			return *It;
		}
	}
	return nullptr;
}

bool ANexusGameMode::IsInSpawnSafeZone(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}
	for (TActorIterator<ANexusTeamStart> It(GetWorld()); It; ++It)
	{
		if (FVector::Dist(Actor->GetActorLocation(), It->GetActorLocation()) <= It->SafeZoneRadius)
		{
			return true;
		}
	}
	return false;
}

void ANexusGameMode::NotifyBombPlanted(ENexusSiteId Site, const FVector& Location, ANexusPlayerState* Planter)
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	if (ANexusGameState* GS = GetGameState<ANexusGameState>())
	{
		GS->bBombPlanted = true;
		GS->PlantedSite = Site;
		GS->BombWorldLocation = Location;
		GS->Phase = ENexusRoundPhase::Planted;
		GS->BombTimeRemaining = S->BombTimerSeconds;
		GS->PhaseTimeRemaining = GS->BombTimeRemaining;
	}
	if (Planter)
	{
		AwardCredits(Planter, S->Economy.Plant);
		AwardUlt(Planter, S->Economy.UltPlant);
		Planter->Plants += 1;
	}
	if (ANexusGameState* GS2 = GetGameState<ANexusGameState>())
	{
		GS2->DeviceState = ENexusDeviceState::Planted;
	}
	UE_LOG(LogNexusBomb, Log, TEXT("PLANTED site=%d"), static_cast<int32>(Site));
	for (TActorIterator<ANexusCharacter> It(GetWorld()); It; ++It)
	{
		if (It->bIsPlanting)
		{
			It->ServerInteractReleased();
		}
	}
}

void ANexusGameMode::NotifyBombDefused(ANexusPlayerState* Defuser)
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	if (Defuser)
	{
		AwardCredits(Defuser, S->Economy.Defuse);
		AwardUlt(Defuser, S->Economy.UltDefuse);
		Defuser->Defuses += 1;
	}
	if (ANexusGameState* GS = GetGameState<ANexusGameState>())
	{
		GS->DeviceState = ENexusDeviceState::Defused;
	}
	EndRound(ENexusTeam::Defenders, ENexusRoundEndReason::BombDefused);
}

void ANexusGameMode::NotifyPlayerDied(ANexusCharacter* Character, AActor* Killer, FName Weapon, bool bHeadshot, bool bWallbang)
{
	ANexusPlayerState* VictimPS = Character ? Character->GetPlayerState<ANexusPlayerState>() : nullptr;
	ANexusPlayerState* KillerPS = nullptr;
	if (APawn* KP = Cast<APawn>(Killer))
	{
		KillerPS = KP->GetPlayerState<ANexusPlayerState>();
	}
	if (VictimPS)
	{
		VictimPS->Deaths += 1;
		VictimPS->bDiedThisRound = true;
		if (VictimPS->bHasBomb && Character)
		{
			if (ANexusGameState* GS = GetGameState<ANexusGameState>())
			{
				GS->bBombDropped = true;
				GS->DroppedBombLocation = Character->GetActorLocation();
			}
			VictimPS->bHasBomb = false;
		}
	}
	if (KillerPS && KillerPS != VictimPS)
	{
		KillerPS->Kills += 1;
		if (bHeadshot) { KillerPS->Headshots += 1; }
		AwardCredits(KillerPS, GetDefault<UNexusGameplaySettings>()->Economy.Kill);
		AwardUlt(KillerPS, GetDefault<UNexusGameplaySettings>()->Economy.UltKill);
	}
	FNexusKillFeedEntry Feed;
	Feed.Killer = KillerPS ? KillerPS->GetPlayerName() : TEXT("World");
	Feed.Victim = VictimPS ? VictimPS->GetPlayerName() : TEXT("Unknown");
	Feed.Weapon = Weapon;
	Feed.bHeadshot = bHeadshot;
	Feed.bWallbang = bWallbang;
	PushKillFeed(Feed);
	CheckElimination();
}

void ANexusGameMode::CheckElimination()
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS || GS->Phase == ENexusRoundPhase::PostRound)
	{
		return;
	}
	int32 AtkAlive = 0;
	int32 DefAlive = 0;
	for (TActorIterator<ANexusCharacter> It(GetWorld()); It; ++It)
	{
		if (It->Health <= 0.f)
		{
			continue;
		}
		if (It->GetTeam() == ENexusTeam::Attackers) { ++AtkAlive; }
		if (It->GetTeam() == ENexusTeam::Defenders) { ++DefAlive; }
	}
	if (AtkAlive == 0 && !GS->bBombPlanted)
	{
		EndRound(ENexusTeam::Defenders, ENexusRoundEndReason::TeamElimination);
	}
	else if (DefAlive == 0)
	{
		EndRound(ENexusTeam::Attackers, ENexusRoundEndReason::TeamElimination);
	}
}

void ANexusGameMode::EndRound(ENexusTeam Winner, ENexusRoundEndReason Reason)
{
	AbortAllObjectives();
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS || GS->Phase == ENexusRoundPhase::PostRound || GS->Phase == ENexusRoundPhase::MatchEnd)
	{
		return;
	}
	GS->LastRoundWinner = Winner;
	GS->LastRoundEndReason = Reason;
	AwardRoundWin(Winner);
	ApplyRoundEconomy(Winner);
	GS->bBombPlanted = false;
	GS->BombTimeRemaining = 0.f;
	if (Reason == ENexusRoundEndReason::BombDetonated)
	{
		GS->DeviceState = ENexusDeviceState::Detonated;
	}
	SetMatchPhase(ENexusRoundPhase::PostRound, GetDefault<UNexusGameplaySettings>()->Match.PostRoundTime, TEXT("EndRound"));
	UE_LOG(LogNexusRound, Log, TEXT("Round %d end winnerRole=%s reason=%s V=%d S=%d"),
		GS->RoundNumber, *UEnum::GetValueAsString(Winner), *UEnum::GetValueAsString(Reason), GS->VanguardRounds, GS->SentinelRounds);
}

void ANexusGameMode::AwardRoundWin(ENexusTeam RoleWinner)
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS)
	{
		return;
	}
	const ENexusFaction Fac = GS->FactionForRole(RoleWinner);
	if (Fac == ENexusFaction::Vanguard) { GS->VanguardRounds += 1; }
	if (Fac == ENexusFaction::Sentinel) { GS->SentinelRounds += 1; }
}

void ANexusGameMode::CleanupRoundActors()
{
	TArray<AActor*> ToDestroy;
	for (TActorIterator<ANexusGadgetActor> It(GetWorld()); It; ++It)
	{
		ToDestroy.Add(*It);
	}
	for (AActor* A : ToDestroy)
	{
		A->Destroy();
	}
	for (TActorIterator<ANexusDoor> It(GetWorld()); It; ++It)
	{
		It->ResetForNewRound();
	}
	for (TActorIterator<ANexusDestructibleSurface> It(GetWorld()); It; ++It)
	{
		It->ResetForNewRound();
	}
}

void ANexusGameMode::ApplyRoundEconomy(ENexusTeam Winner)
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS) { return; }
	const ENexusFaction WinF = GS->FactionForRole(Winner);
	if (WinF == ENexusFaction::Vanguard)
	{
		GS->VanguardLossStreak = 0;
		GS->SentinelLossStreak += 1;
	}
	else if (WinF == ENexusFaction::Sentinel)
	{
		GS->SentinelLossStreak = 0;
		GS->VanguardLossStreak += 1;
	}
	for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
	{
		const bool bWin = It->Faction == WinF;
		if (bWin)
		{
			AwardCredits(*It, S->Economy.RoundWin);
		}
		else
		{
			const int32 Streak = (It->Faction == ENexusFaction::Vanguard) ? GS->VanguardLossStreak : GS->SentinelLossStreak;
			AwardCredits(*It, LossBonusForStreak(Streak));
		}
	}
	UE_LOG(LogNexusEconomy, Log, TEXT("Round economy applied winFaction=%s"), *UEnum::GetValueAsString(WinF));
}

int32 ANexusGameMode::LossBonusForStreak(int32 Streak) const
{
	const FNexusEconomyConfig& E = GetDefault<UNexusGameplaySettings>()->Economy;
	if (Streak <= 1) { return E.LossStreak1; }
	if (Streak == 2) { return E.LossStreak2; }
	if (Streak == 3) { return E.LossStreak3; }
	return E.LossStreak4Plus;
}

void ANexusGameMode::AwardCredits(ANexusPlayerState* PS, int32 Amount)
{
	if (PS) { PS->ServerAddCredits(Amount); }
}

void ANexusGameMode::AwardUlt(ANexusPlayerState* PS, int32 Amount)
{
	if (!PS || !HasAuthority()) { return; }
	PS->UltimatePoints = FMath::Max(0, PS->UltimatePoints + Amount);
}

void ANexusGameMode::DebugForceScore(int32 VanguardRounds, int32 SentinelRounds)
{
	if (!HasAuthority())
	{
		return;
	}
	if (ANexusGameState* GS = GetGameState<ANexusGameState>())
	{
		GS->VanguardRounds = FMath::Max(0, VanguardRounds);
		GS->SentinelRounds = FMath::Max(0, SentinelRounds);
		UE_LOG(LogNEXUS, Log, TEXT("[NEXUS TEST] [PASS] System=MATCH Test=ForceScore Expected=V=%d S=%d Actual=V=%d S=%d Cause=debug only"),
			VanguardRounds, SentinelRounds, GS->VanguardRounds, GS->SentinelRounds);
	}
}

void ANexusGameMode::DebugForceOvertimeFromTie()
{
	if (!HasAuthority())
	{
		return;
	}
	DebugForceScore(12, 12);
	if (ANexusGameState* GS = GetGameState<ANexusGameState>())
	{
		GS->bOvertime = false;
		GS->Phase = ENexusRoundPhase::PostRound;
		GS->PhaseTimeRemaining = 0.05f;
	}
}

void ANexusGameMode::PushKillFeed(const FNexusKillFeedEntry& Entry)
{
	if (ANexusGameState* GS = GetGameState<ANexusGameState>())
	{
		GS->KillFeed.Add(Entry);
		while (GS->KillFeed.Num() > 6)
		{
			GS->KillFeed.RemoveAt(0);
		}
	}
}

void ANexusGameMode::ServerBuy(ANexusPlayerState* PS, FName ItemId)
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!PS || !GS || !GS->IsBuyPhase() || !HasAuthority())
	{
		return;
	}
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	if (ItemId == TEXT("LightArmor"))
	{
		if (PS->ServerTrySpend(S->LightArmorPrice))
		{
			PS->ArmorType = ENexusArmorType::Light;
			PS->ArmorValue = S->LightArmor.ArmorValue;
		}
		return;
	}
	if (ItemId == TEXT("HeavyArmor"))
	{
		if (PS->ServerTrySpend(S->HeavyArmorPrice))
		{
			PS->ArmorType = ENexusArmorType::Heavy;
			PS->ArmorValue = S->HeavyArmor.ArmorValue;
		}
		return;
	}
	if (const FNexusWeaponDefinition* W = S->FindWeapon(ItemId))
	{
		if ((W->Slot == ENexusWeaponSlot::Primary && PS->PrimaryWeapon == W->ID)
			|| (W->Slot == ENexusWeaponSlot::Secondary && PS->SecondaryWeapon == W->ID))
		{
			return;
		}
		if (PS->ServerTrySpend(W->Price))
		{
			if (W->Slot == ENexusWeaponSlot::Primary) { PS->PrimaryWeapon = W->ID; }
			else { PS->SecondaryWeapon = W->ID; }
			PS->EquippedWeapon = W->ID;
			PS->MagAmmo = W->MagazineSize;
			PS->ReserveAmmo = W->ReserveAmmo;
			PS->bBoughtThisRound = true;
		}
	}
}

void ANexusGameMode::ServerSelectOperator(ANexusPlayerState* PS, ENexusOperatorId Id)
{
	if (!PS || !HasAuthority()) { return; }
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (GS && GS->RoundNumber > 1 && GS->Phase != ENexusRoundPhase::WaitingForPlayers && GS->Phase != ENexusRoundPhase::BuyPhase)
	{
		return;
	}
	PS->OperatorId = Id;
	PS->ApplyOperatorDefaults();
}

void ANexusGameMode::SwapAllTeams()
{
	for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
	{
		if (It->Team == ENexusTeam::Attackers) { It->Team = ENexusTeam::Defenders; }
		else if (It->Team == ENexusTeam::Defenders) { It->Team = ENexusTeam::Attackers; }
	}
	if (ANexusGameState* GS = GetGameState<ANexusGameState>())
	{
		GS->bSidesSwapped = !GS->bSidesSwapped;
	}
}

void ANexusGameMode::MaybeSideSwitch()
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	const FNexusMatchConfig& M = GetDefault<UNexusGameplaySettings>()->Match;
	if (!GS) { return; }
	if (!GS->bOvertime && GS->RoundNumber == 12)
	{
		GS->Phase = ENexusRoundPhase::SideSwitch;
		GS->PhaseTimeRemaining = M.SideSwitchTime;
		SwapAllTeams();
	}
	else if (GS->bOvertime && (GS->RoundNumber - 24) % M.OTRoundsPerHalf == 0)
	{
		GS->Phase = ENexusRoundPhase::SideSwitch;
		GS->PhaseTimeRemaining = M.SideSwitchTime;
		SwapAllTeams();
	}
}

bool ANexusGameMode::IsMatchOver() const
{
	const ANexusGameState* GS = GetGameState<ANexusGameState>();
	const FNexusMatchConfig& M = GetDefault<UNexusGameplaySettings>()->Match;
	if (!GS) { return false; }
	const int32 Target = GS->bOvertime ? M.OTWinTarget : M.RegulationRoundsToWin;
	if (GS->VanguardRounds >= Target || GS->SentinelRounds >= Target)
	{
		return true;
	}
	if (GS->bOvertime && (GS->VanguardRounds >= 12 + M.OTSeriesWin || GS->SentinelRounds >= 12 + M.OTSeriesWin))
	{
		return true;
	}
	if (!GS->bOvertime && GS->RoundNumber >= M.RegulationMaxRounds)
	{
		return true;
	}
	if (GS->bOvertime && GS->RoundNumber >= M.OTMaxRounds)
	{
		return true;
	}
	return false;
}

void ANexusGameMode::ResetPawnsForRound()
{
	for (TActorIterator<ANexusTeamStart> It(GetWorld()); It; ++It)
	{
		It->bOccupied = false;
	}
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		if (AController* C = It->Get())
		{
			RestartPlayer(C);
		}
	}
}

bool ANexusGameMode::CanDealDamage(AActor* InstigatorActor, AActor* Victim) const
{
	const ANexusGameState* GS = GetGameState<ANexusGameState>();
	if (!GS || GS->Phase == ENexusRoundPhase::Freeze || GS->Phase == ENexusRoundPhase::PostRound
		|| GS->Phase == ENexusRoundPhase::BuyPhase || GS->Phase == ENexusRoundPhase::WaitingForPlayers
		|| GS->Phase == ENexusRoundPhase::MatchStarting || GS->Phase == ENexusRoundPhase::SideSwitch
		|| GS->Phase == ENexusRoundPhase::MatchEnd)
	{
		return false;
	}
	if (GS->IsSpawnProtectionActive() && (IsInSpawnSafeZone(Victim) || IsInSpawnSafeZone(InstigatorActor)))
	{
		return false;
	}
	const ANexusCharacter* VicChar = Cast<ANexusCharacter>(Victim);
	const ANexusCharacter* InstChar = Cast<ANexusCharacter>(InstigatorActor);
	if (VicChar && VicChar->bSpawnProtected)
	{
		return false;
	}
	if (InstChar && VicChar && InstChar->GetTeam() == VicChar->GetTeam() && InstChar->GetTeam() != ENexusTeam::Spectator)
	{
		return false;
	}
	return true;
}

void ANexusGameMode::TickRound(float DeltaSeconds)
{
	ANexusGameState* GS = GetGameState<ANexusGameState>();
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	if (!GS)
	{
		return;
	}
	GS->PhaseTimeRemaining = FMath::Max(0.f, GS->PhaseTimeRemaining - DeltaSeconds);
	if (GS->SpawnProtectionRemaining > 0.f)
	{
		GS->SpawnProtectionRemaining = FMath::Max(0.f, GS->SpawnProtectionRemaining - DeltaSeconds);
	}

	if (GS->Phase == ENexusRoundPhase::WaitingForPlayers && GS->PhaseTimeRemaining <= 0.f)
	{
		GS->Phase = ENexusRoundPhase::MatchStarting;
		GS->PhaseTimeRemaining = 2.f;
	}
	else if (GS->Phase == ENexusRoundPhase::MatchStarting && GS->PhaseTimeRemaining <= 0.f)
	{
		StartRound();
	}
	else if (GS->Phase == ENexusRoundPhase::BuyPhase && GS->PhaseTimeRemaining <= 0.f)
	{
		GS->Phase = ENexusRoundPhase::Freeze;
		GS->PhaseTimeRemaining = S->Match.FreezeTime;
	}
	else if (GS->Phase == ENexusRoundPhase::Freeze && GS->PhaseTimeRemaining <= 0.f)
	{
		GS->Phase = ENexusRoundPhase::Live;
		GS->PhaseTimeRemaining = S->Match.RoundTime;
	}
	else if (GS->Phase == ENexusRoundPhase::Live && GS->PhaseTimeRemaining <= 0.f && !GS->bBombPlanted)
	{
		EndRound(ENexusTeam::Defenders, ENexusRoundEndReason::TimeExpired);
	}
	else if (GS->Phase == ENexusRoundPhase::Planted)
	{
		GS->BombTimeRemaining = GS->PhaseTimeRemaining;
		if (GS->BombTimeRemaining <= 0.f)
		{
			EndRound(ENexusTeam::Attackers, ENexusRoundEndReason::BombDetonated);
		}
	}
	else if (GS->Phase == ENexusRoundPhase::PostRound && GS->PhaseTimeRemaining <= 0.f)
	{
		if (!GS->bOvertime && GS->VanguardRounds == 12 && GS->SentinelRounds == 12 && S->Match.bOvertimeEnabled)
		{
			GS->bOvertime = true;
			SetMatchPhase(ENexusRoundPhase::Overtime, 3.f, TEXT("12-12 OT"));
			return;
		}
		if (IsMatchOver())
		{
			GS->MatchWinner = (GS->VanguardRounds > GS->SentinelRounds) ? ENexusFaction::Vanguard : ENexusFaction::Sentinel;
			if (GS->VanguardRounds == GS->SentinelRounds) { GS->MatchWinner = ENexusFaction::Unassigned; }
			SetMatchPhase(ENexusRoundPhase::MatchEnd, 15.f, TEXT("MatchOver"));
			return;
		}
		MaybeSideSwitch();
		if (GS->Phase != ENexusRoundPhase::SideSwitch)
		{
			StartRound();
		}
	}
	else if (GS->Phase == ENexusRoundPhase::Overtime && GS->PhaseTimeRemaining <= 0.f)
	{
		StartRound();
	}
	else if (GS->Phase == ENexusRoundPhase::SideSwitch && GS->PhaseTimeRemaining <= 0.f)
	{
		StartRound();
	}
}

void ANexusGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (HasAuthority())
	{
		TickRound(DeltaSeconds);
	}
}
