#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.h"
#include "NexusGameplaySettings.generated.h"

/**
 * Single source of gameplay numbers for V0.1.
 * Edit in Project Settings → NEXUS. Do not duplicate these values in actors.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "NEXUS Gameplay"))
class NEXUS_API UNexusGameplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UNexusGameplaySettings();

	virtual FName GetCategoryName() const override { return FName(TEXT("NEXUS")); }

	UPROPERTY(EditAnywhere, Config, Category = "Match")
	int32 PlayersPerTeam = 5;

	UPROPERTY(EditAnywhere, Config, Category = "Match")
	float FreezeTimeSeconds = 30.f;

	/** Greybox tests skip freeze unless this is true. */
	UPROPERTY(EditAnywhere, Config, Category = "Match")
	bool bUseFreezeTime = false;

	UPROPERTY(EditAnywhere, Config, Category = "Match")
	float RoundTimeSeconds = 240.f;

	UPROPERTY(EditAnywhere, Config, Category = "Match")
	float SpawnProtectionSeconds = 5.f;

	/** Single bomb timing source. Do not duplicate these in actors. */
	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	FNexusBombConfig Bomb;

	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	float PlantTimeSeconds = 4.f;

	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	float DefuseTimeSeconds = 7.f;

	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	float BombTimerSeconds = 40.f;

	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	float PlantRadiusMeters = 1.5f;

	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	float PlantInterruptDamage = 15.f;

	UPROPERTY(EditAnywhere, Config, Category = "Bomb")
	float DefuseHoldGraceSeconds = 2.f;

	UPROPERTY(EditAnywhere, Config, Category = "Movement")
	float WalkSpeedUu = 500.f;

	UPROPERTY(EditAnywhere, Config, Category = "Movement")
	float RotationWalkSpeedUu = 500.f;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	float TestRifleBodyDamage = 33.f;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	float TestRifleHeadDamage = 110.f;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	float TestRifleRPM = 800.f;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	float TestRiflePenetrationPower = 1.f;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	int32 MaxPenetrationSurfaces = 3;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	float MaxPenetrationDepthMeters = 1.2f;

	UPROPERTY(EditAnywhere, Config, Category = "Weapon")
	float ExtraDamageDecayPerSurface = 0.15f;

	UPROPERTY(EditAnywhere, Config, Category = "Interaction")
	float InteractDistanceUu = 180.f;

	UPROPERTY(EditAnywhere, Config, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Config, Category = "Doors")
	float StandardDoorHP = 500.f;

	UPROPERTY(EditAnywhere, Config, Category = "Doors")
	float ReinforcedDoorHP = 1500.f;

	UPROPERTY(EditAnywhere, Config, Category = "Destruction")
	float LightPartitionHP = 500.f;

	UPROPERTY(EditAnywhere, Config, Category = "Destruction")
	float IntermediateHP = 1200.f;

	UPROPERTY(EditAnywhere, Config, Category = "Destruction")
	float ThickHP = 2500.f;

	UPROPERTY(EditAnywhere, Config, Category = "Penetration")
	TArray<FNexusPenetrationProfile> PenetrationTable;

	UPROPERTY(EditAnywhere, Config, Category = "Match")
	FNexusMatchConfig Match;

	UPROPERTY(EditAnywhere, Config, Category = "Economy")
	FNexusEconomyConfig Economy;

	UPROPERTY(EditAnywhere, Config, Category = "Armor")
	int32 LightArmorPrice = 500;

	UPROPERTY(EditAnywhere, Config, Category = "Armor")
	int32 HeavyArmorPrice = 650;

	UPROPERTY(EditAnywhere, Config, Category = "Armor")
	FNexusArmorDefinition LightArmor;

	UPROPERTY(EditAnywhere, Config, Category = "Armor")
	FNexusArmorDefinition HeavyArmor;

	UPROPERTY(EditAnywhere, Config, Category = "Combat")
	TArray<FNexusWeaponDefinition> Weapons;

	UPROPERTY(EditAnywhere, Config, Category = "Combat")
	TArray<FNexusOperatorDefinition> Operators;

	UPROPERTY(EditAnywhere, Config, Category = "Combat")
	TArray<FNexusGadgetDefinition> Gadgets;

	UPROPERTY(EditAnywhere, Config, Category = "Bots")
	FNexusBotSettings Bots;

	UPROPERTY(EditAnywhere, Config, Category = "Combat")
	float WeaponSwitchTime = 0.45f;

	UPROPERTY(EditAnywhere, Config, Category = "Combat")
	float KillFeedSeconds = 5.f;

	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bWeaponDebug = false;

	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bDamageDebug = false;

	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bHitDebug = false;

	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bHitboxDebug = false;

	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bWallbangDebug = false;

	const FNexusPenetrationProfile& GetProfile(ENexusMaterialType Type) const;
	const FNexusWeaponDefinition* FindWeapon(FName ID) const;
	const FNexusOperatorDefinition* FindOperator(ENexusOperatorId ID) const;
	const FNexusGadgetDefinition* FindGadget(ENexusGadgetId ID) const;
	FNexusArmorDefinition GetArmorDef(ENexusArmorType Type) const;
};
