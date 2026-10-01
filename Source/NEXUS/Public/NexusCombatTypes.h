#pragma once

#include "CoreMinimal.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.generated.h"

UENUM(BlueprintType)
enum class ENexusHitZone : uint8
{
	Head,
	Torso,
	LeftArm,
	RightArm,
	LeftLeg,
	RightLeg
};

UENUM(BlueprintType)
enum class ENexusWeaponCategory : uint8
{
	Pistol,
	SMG,
	Rifle,
	Sniper,
	Shotgun,
	LMG,
	Melee
};

UENUM(BlueprintType)
enum class ENexusWeaponSlot : uint8
{
	Primary,
	Secondary,
	Melee,
	Gadget
};

UENUM(BlueprintType)
enum class ENexusArmorType : uint8
{
	None,
	Light,
	Heavy
};

UENUM(BlueprintType)
enum class ENexusWeightClass : uint8
{
	Light,
	Standard,
	Heavy
};

UENUM(BlueprintType)
enum class ENexusOperatorId : uint8
{
	None,
	Aze,
	Brutus,
	Nyx,
	Vanta,
	Titan,
	Warden,
	Kraken,
	Echo,
	Bulwark,
	Hawk
};

UENUM(BlueprintType)
enum class ENexusGadgetId : uint8
{
	None,
	Drone,
	OpticalBeacon,
	BreachCharge,
	PenetratingCharge,
	PersonalJammer,
	SilentCharge,
	Smoke,
	Incendiary,
	BallisticCover,
	APAmmo,
	Camera,
	MotionSensor,
	ElectricCharge,
	RapidReinforcement,
	Jammer,
	EMPMine,
	BallisticBarrier,
	Reinforcement,
	OpticalDetector,
	ThermalScope
};

UENUM(BlueprintType)
enum class ENexusUltimateId : uint8
{
	None,
	BlackEye,
	BreachHammer,
	GhostWalk,
	Screen,
	APLoad,
	ControlRoom,
	Lockdown,
	Blackout,
	Fortify,
	Hunter
};

UENUM(BlueprintType)
enum class ENexusBotState : uint8
{
	Idle,
	Buy,
	MoveToObjective,
	Hold,
	Investigate,
	Engage,
	Retreat,
	Plant,
	Defuse,
	Rotate,
	Search,
	PostPlant,
	Retake,
	Dead,
	Spectate,
	Attack,
	Defend,
	Recon,
	Push,
	Support
};

UENUM(BlueprintType)
enum class ENexusBotRole : uint8
{
	Anchor,
	Roamer,
	Support
};

UENUM(BlueprintType)
enum class ENexusBotDifficulty : uint8
{
	Easy,
	Normal,
	Hard
};

USTRUCT(BlueprintType)
struct FNexusBotSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	ENexusBotDifficulty Difficulty = ENexusBotDifficulty::Normal;
};

USTRUCT(BlueprintType)
struct FNexusMatchConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 RegulationRoundsToWin = 13;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 RegulationMaxRounds = 24;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	bool bOvertimeEnabled = true;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 OTRoundsPerHalf = 3;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 OTWinTarget = 16;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 OTSeriesWin = 2;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 OTMaxRounds = 30;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float BuyTime = 20.f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float FreezeTime = 5.f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float RoundTime = 180.f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float PostRoundTime = 5.f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float SideSwitchTime = 4.f;
};

USTRUCT(BlueprintType)
struct FNexusEconomyConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 StartingCredits = 800;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 MaximumCredits = 9000;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 RoundWin = 3000;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 RoundLoss = 1900;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 Kill = 200;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 Assist = 100;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 Plant = 300;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 Defuse = 300;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 LossStreak1 = 1900;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 LossStreak2 = 2200;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 LossStreak3 = 2500;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 LossStreak4Plus = 2800;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 UltKill = 1;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 UltAssist = 1;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 UltPlant = 2;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 UltDefuse = 2;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	int32 UltRoundParticipation = 1;
};

USTRUCT(BlueprintType)
struct FNexusArmorDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusArmorType Type = ENexusArmorType::None;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 Price = 0;

	/** Current / max absorb pool. Not extra HP. */
	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float ArmorValue = 0.f;

	/** Fraction of incoming body damage absorbed before HP (0-1). */
	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float ArmorProtection = 0.f;

	/** Multiplier applied to remaining HP damage after absorb. Head uses 1.0. */
	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float ArmorDamageMultiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FNexusWeaponDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusWeaponCategory Category = ENexusWeaponCategory::Rifle;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusWeaponSlot Slot = ENexusWeaponSlot::Primary;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 Price = 0;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 ReserveAmmo = 90;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float FireRateRPM = 600.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float DamageHead = 100.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float DamageTorso = 33.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float DamageArms = 27.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float DamageLegs = 24.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float Penetration = 1.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float VerticalRecoil = 0.35f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float HorizontalRecoil = 0.12f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float RecoilRecovery = 8.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float FirstShotAccuracy = 0.02f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float HipFireSpread = 0.8f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float ADSSpread = 0.15f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float ReloadTime = 2.2f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float EmptyReloadTime = 2.8f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float EquipTime = 0.6f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float MovementPenalty = 0.08f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float MinDamageMultiplier = 0.7f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float MaxDamageRange = 1500.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float MinDamageRange = 4000.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 PelletCount = 1;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	bool bRequiresADS = false;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	bool bAllowADS = true;
};

USTRUCT(BlueprintType)
struct FNexusGadgetDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusGadgetId ID = ENexusGadgetId::None;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 Charges = 2;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float Duration = 10.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float RadiusMeters = 5.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float RangeMeters = 20.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float PlaceTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	bool bDestructible = true;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	FString CounterHint;
};

USTRUCT(BlueprintType)
struct FNexusOperatorDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusOperatorId ID = ENexusOperatorId::None;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	FText Name;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusTeam RecommendedTeam = ENexusTeam::Attackers;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusWeightClass WeightClass = ENexusWeightClass::Standard;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	TArray<FName> PrimaryWeapons;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	TArray<FName> SecondaryWeapons;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusGadgetId Gadget1 = ENexusGadgetId::None;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusGadgetId Gadget2 = ENexusGadgetId::None;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	ENexusUltimateId Ultimate = ENexusUltimateId::None;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	int32 UltimateCost = 7;

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	float MoveSpeedMeters = 5.5f;
};

USTRUCT(BlueprintType)
struct FNexusKillFeedEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FString Killer;

	UPROPERTY()
	FString Victim;

	UPROPERTY()
	FName Weapon;

	UPROPERTY()
	bool bHeadshot = false;

	UPROPERTY()
	bool bWallbang = false;

	UPROPERTY()
	bool bGadget = false;
};

inline ENexusHitZone NexusHitZoneFromLocal(const FVector& Local)
{
	if (Local.Z > 55.f)
	{
		return ENexusHitZone::Head;
	}
	if (Local.Z < -10.f)
	{
		return (Local.Y >= 0.f) ? ENexusHitZone::RightLeg : ENexusHitZone::LeftLeg;
	}
	if (FMath::Abs(Local.Y) > 22.f)
	{
		return (Local.Y >= 0.f) ? ENexusHitZone::RightArm : ENexusHitZone::LeftArm;
	}
	return ENexusHitZone::Torso;
}
