#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.h"
#include "NexusGameMode.generated.h"

class ANexusTeamStart;
class ANexusCharacter;
class ANexusPlayerState;

UCLASS()
class NEXUS_API ANexusGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANexusGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	bool CanDealDamage(AActor* InstigatorActor, AActor* Victim) const;
	void NotifyBombPlanted(ENexusSiteId Site, const FVector& Location, ANexusPlayerState* Planter = nullptr);
	void NotifyBombDefused(ANexusPlayerState* Defuser = nullptr);
	void NotifyPlayerDied(ANexusCharacter* Character, AActor* Killer, FName Weapon, bool bHeadshot, bool bWallbang);

	void StartMatchFlow();
	void StartRound();
	void Rematch();
	void FillBotsToFiveVFive();
	void AbortAllObjectives();
	void ServerBuy(ANexusPlayerState* PS, FName ItemId);
	void ServerSelectOperator(ANexusPlayerState* PS, ENexusOperatorId Id);
	void PushKillFeed(const FNexusKillFeedEntry& Entry);
	void AwardCredits(ANexusPlayerState* PS, int32 Amount);
	void AwardUlt(ANexusPlayerState* PS, int32 Amount);
	void DebugForceScore(int32 VanguardRounds, int32 SentinelRounds);
	void DebugForceOvertimeFromTie();

protected:
	void EnsureGreybox();
	void EnsureBots();
	void SpawnBot(ENexusTeam Team);
	void AssignTeam(AController* Controller, ENexusTeam Team);
	int32 CountTeam(ENexusTeam Team) const;
	ANexusTeamStart* FindFreeSpawn(ENexusTeam Team) const;
	void TickRound(float DeltaSeconds);
	void EndRound(ENexusTeam Winner, ENexusRoundEndReason Reason);
	void SetMatchPhase(ENexusRoundPhase NewPhase, float Duration, const TCHAR* Cause);
	void CleanupRoundActors();
	void AwardRoundWin(ENexusTeam RoleWinner);
	void ApplyRoundEconomy(ENexusTeam Winner);
	void MaybeSideSwitch();
	void SwapAllTeams();
	bool IsMatchOver() const;
	bool IsInSpawnSafeZone(AActor* Actor) const;
	void CheckElimination();
	void ResetPawnsForRound();
	int32 LossBonusForStreak(int32 Streak) const;

	bool bRoundStarted = false;
	bool bMatchLive = false;
};
