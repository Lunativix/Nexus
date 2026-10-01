#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.h"
#include "NexusPlayerState.generated.h"

UCLASS()
class NEXUS_API ANexusPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANexusPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusTeam Team = ENexusTeam::Spectator;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusFaction Faction = ENexusFaction::Unassigned;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bHasBomb = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bIsBotPlayer = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Credits = 800;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusOperatorId OperatorId = ENexusOperatorId::Aze;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	ENexusArmorType ArmorType = ENexusArmorType::None;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	float ArmorValue = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	FName PrimaryWeapon = NAME_None;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	FName SecondaryWeapon = TEXT("USP");

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	FName EquippedWeapon = TEXT("USP");

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 MagAmmo = 12;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 ReserveAmmo = 24;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Kills = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Deaths = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Assists = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 UltimatePoints = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Gadget1Charges = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Gadget2Charges = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bDiedThisRound = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bBoughtThisRound = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 DamageDealt = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Headshots = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Plants = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	int32 Defuses = 0;

	void ServerAddCredits(int32 Amount);
	bool ServerTrySpend(int32 Amount);
	void GrantLoadoutForNewRound(bool bKeepPurchases);
	void ApplyOperatorDefaults();
};
