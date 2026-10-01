#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.h"
#include "NexusGameState.generated.h"

UCLASS()
class NEXUS_API ANexusGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANexusGameState();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusRoundPhase Phase = ENexusRoundPhase::WaitingForPlayers;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	float PhaseTimeRemaining = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusSiteId PlantedSite = ENexusSiteId::None;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	FVector BombWorldLocation = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	float BombTimeRemaining = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bBombPlanted = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	float SpawnProtectionRemaining = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 VanguardRounds = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 SentinelRounds = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 RoundNumber = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 VanguardLossStreak = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 SentinelLossStreak = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bOvertime = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bSidesSwapped = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusTeam LastRoundWinner = ENexusTeam::Spectator;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	TArray<FNexusKillFeedEntry> KillFeed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bBombDropped = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	FVector DroppedBombLocation = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusRoundEndReason LastRoundEndReason = ENexusRoundEndReason::None;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusFaction MatchWinner = ENexusFaction::Unassigned;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusDeviceState DeviceState = ENexusDeviceState::Carried;

	bool IsSpawnProtectionActive() const { return SpawnProtectionRemaining > 0.f; }
	bool IsBuyPhase() const { return Phase == ENexusRoundPhase::BuyPhase || Phase == ENexusRoundPhase::Freeze; }
	ENexusFaction FactionForRole(ENexusTeam RoundRole) const;
};
