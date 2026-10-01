#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NexusCombatTypes.h"
#include "NexusPlayerController.generated.h"

UCLASS()
class NEXUS_API ANexusPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANexusPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	FText LastCallout;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	float CalloutTimeRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	float HitMarkerTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	bool bHitMarkerHead = false;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	bool bShowScoreboard = false;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	bool bShowBuyMenu = false;

	UPROPERTY(BlueprintReadOnly, Category = "NEXUS")
	bool bSpectating = false;

	void ShowCallout(const FText& Callout);
	void EnterSpectator();
	void CycleSpectator(int32 Dir);

	UFUNCTION(Server, Reliable)
	void ServerBuyItem(FName ItemId);

	UFUNCTION(Server, Reliable)
	void ServerPickOperator(uint8 OperatorId);

	UFUNCTION(Exec)
	void Buy(const FString& ItemId);

	UFUNCTION(Exec)
	void NexusValidateLevel();
	UFUNCTION(Exec)
	void NexusLevelDebug(int32 Mask = -1);
	UFUNCTION(Exec)
	void NexusTestPlant(const FString& Site);
	UFUNCTION(Exec)
	void NexusTestDefuse();
	UFUNCTION(Exec)
	void NexusTestWallbang();
	UFUNCTION(Exec)
	void NexusTestRound();
	UFUNCTION(Exec)
	void NexusTestBots();
	UFUNCTION(Exec)
	void NexusTestNavigation();
	UFUNCTION(Exec)
	void NexusTestMatch();
	UFUNCTION(Exec)
	void NexusForceScore(int32 VanguardRounds = 12, int32 SentinelRounds = 12);
	UFUNCTION(Exec)
	void NexusForceOvertime();
	UFUNCTION(Exec)
	void NexusNavConnectivity();
	UFUNCTION(Exec)
	void NexusRematch();
	UFUNCTION(Exec)
	void NexusGiveCredits(int32 Amount = 800);
	UFUNCTION(Exec)
	void NexusShowEconomy();
	UFUNCTION(Exec)
	void NexusNetSim(int32 Milliseconds = 0);
	UFUNCTION(Exec)
	void NexusTestOperators();
	UFUNCTION(Exec)
	void NexusValidateSpawns();
	UFUNCTION(Exec)
	void NexusNetworkTest();
	UFUNCTION(Exec)
	void NexusStartBotMatch();
	UFUNCTION(Exec)
	void NexusAutoMatchTest();
	UFUNCTION(Exec)
	void NexusValidateVelasqo();
	UFUNCTION(Exec)
	void NexusVelasqoWalkTest();
	UFUNCTION(Exec)
	void NexusVelasqoTeleport(const FString& Landmark);
	UFUNCTION(Exec)
	void NexusVelasqoFullTest();
	UFUNCTION(Exec)
	void NexusOpenMap(const FString& MapName);

	UFUNCTION(Client, Unreliable)
	void ClientHitFeedback(bool bHeadshot);

	virtual void PlayerTick(float DeltaTime) override;

protected:
	void OnScoreboardPressed();
	void OnScoreboardReleased();
	void OnBuyMenu();
	void OnSpecNext();
	void OnSpecPrev();
};
