#include "NexusGameState.h"
#include "Art/NexusEnvironmentDirector.h"
#include "Net/UnrealNetwork.h"

ANexusGameState::ANexusGameState()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
}

void ANexusGameState::BeginPlay()
{
	Super::BeginPlay();
	ANexusEnvironmentDirector::Ensure(GetWorld());
}

void ANexusGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANexusGameState, Phase);
	DOREPLIFETIME(ANexusGameState, PhaseTimeRemaining);
	DOREPLIFETIME(ANexusGameState, PlantedSite);
	DOREPLIFETIME(ANexusGameState, BombWorldLocation);
	DOREPLIFETIME(ANexusGameState, BombTimeRemaining);
	DOREPLIFETIME(ANexusGameState, bBombPlanted);
	DOREPLIFETIME(ANexusGameState, SpawnProtectionRemaining);
	DOREPLIFETIME(ANexusGameState, VanguardRounds);
	DOREPLIFETIME(ANexusGameState, SentinelRounds);
	DOREPLIFETIME(ANexusGameState, RoundNumber);
	DOREPLIFETIME(ANexusGameState, VanguardLossStreak);
	DOREPLIFETIME(ANexusGameState, SentinelLossStreak);
	DOREPLIFETIME(ANexusGameState, bOvertime);
	DOREPLIFETIME(ANexusGameState, bSidesSwapped);
	DOREPLIFETIME(ANexusGameState, LastRoundWinner);
	DOREPLIFETIME(ANexusGameState, KillFeed);
	DOREPLIFETIME(ANexusGameState, bBombDropped);
	DOREPLIFETIME(ANexusGameState, DroppedBombLocation);
	DOREPLIFETIME(ANexusGameState, LastRoundEndReason);
	DOREPLIFETIME(ANexusGameState, MatchWinner);
	DOREPLIFETIME(ANexusGameState, DeviceState);
}

ENexusFaction ANexusGameState::FactionForRole(ENexusTeam RoundRole) const
{
	const bool bVanguardAttacks = !bSidesSwapped;
	if (RoundRole == ENexusTeam::Attackers)
	{
		return bVanguardAttacks ? ENexusFaction::Vanguard : ENexusFaction::Sentinel;
	}
	if (RoundRole == ENexusTeam::Defenders)
	{
		return bVanguardAttacks ? ENexusFaction::Sentinel : ENexusFaction::Vanguard;
	}
	return ENexusFaction::Unassigned;
}
