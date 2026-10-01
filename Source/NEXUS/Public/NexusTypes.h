#pragma once

#include "CoreMinimal.h"
#include "NexusTypes.generated.h"

/** 1 meter = 100 Unreal units. Layout data is authored in meters. */
inline FVector NexusMeters(float X, float Y, float Z)
{
	return FVector(X, Y, Z) * 100.f;
}

inline FVector NexusMeters(const FVector& Meters)
{
	return Meters * 100.f;
}

inline float NexusMeters(float Meters)
{
	return Meters * 100.f;
}

UENUM(BlueprintType)
enum class ENexusFaction : uint8
{
	Unassigned,
	Vanguard,
	Sentinel
};

UENUM(BlueprintType)
enum class ENexusRoundEndReason : uint8
{
	None,
	TeamElimination,
	BombDetonated,
	BombDefused,
	TimeExpired
};

UENUM(BlueprintType)
enum class ENexusDeviceState : uint8
{
	Carried,
	Dropped,
	Planting,
	Planted,
	Defusing,
	Defused,
	Detonated
};

UENUM(BlueprintType)
enum class ENexusRoundPhase : uint8
{
	Warmup,
	Freeze,
	Live,
	Planted UMETA(DisplayName = "BombPlanted"),
	PostPlant,
	PostRound UMETA(DisplayName = "RoundEnd"),
	Reset,
	WaitingForPlayers,
	MatchStarting,
	BuyPhase,
	SideSwitch,
	Overtime,
	MatchEnd
};

UENUM(BlueprintType)
enum class ENexusDoorState : uint8
{
	Closed,
	Open,
	Destroyed,
	Locked
};

USTRUCT(BlueprintType)
struct FNexusBombConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float PlantTime = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float DefuseTime = 7.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float BombTimer = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float PlantRadiusMeters = 1.5f;
};

UENUM(BlueprintType)
enum class ENexusMaterialType : uint8
{
	Wood,
	Plaster,
	LightStone,
	ThickStone,
	Concrete,
	Structural,
	Glass
};

UENUM(BlueprintType)
enum class ENexusCoverType : uint8
{
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class ENexusDoorType : uint8
{
	Standard,
	Double,
	Destructible,
	Reinforced
};

UENUM(BlueprintType)
enum class ENexusSiteId : uint8
{
	None,
	A,
	B
};

UENUM(BlueprintType)
enum class ENexusTeam : uint8
{
	Spectator UMETA(DisplayName = "Spectator"),
	Attackers UMETA(DisplayName = "Attackers"),
	Defenders UMETA(DisplayName = "Defenders")
};

UENUM(BlueprintType)
enum class ENexusRouteId : uint8
{
	None,
	RouteA,
	RouteB,
	RouteC,
	RotateShortAB,
	RotateLongAB,
	RotateShortBA,
	RotateLongBA
};

UENUM(BlueprintType)
enum class ENexusBoxUsage : uint8
{
	Structure,
	Floor,
	Stairs,
	Cover,
	Destructible,
	Detail
};

/** Bitmask for nexus.LevelDebug */
enum class ENexusDebugChannel : uint32
{
	None = 0,
	Spawns = 1 << 0,
	Sites = 1 << 1,
	PlantZones = 1 << 2,
	Routes = 1 << 3,
	Callouts = 1 << 4,
	DestructibleWalls = 1 << 5,
	PenetrationMaterials = 1 << 6,
	RotationPaths = 1 << 7,
	LineOfSight = 1 << 8,
	NavMesh = 1 << 9,
	All = 0xFFFFFFFFu
};

USTRUCT(BlueprintType)
struct FNexusPenetrationProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusMaterialType Material = ENexusMaterialType::Wood;

	/** 0–1 damage reduction applied when a round first enters this surface. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DamageReduction = 0.15f;

	/** Extra reduction per meter of thickness. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float ThicknessReductionPerMeter = 0.25f;

	/** Remaining damage fraction below this stops the bullet. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float StopThreshold = 0.05f;
};

USTRUCT(BlueprintType)
struct FNexusBoxDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName Id;

	/** Center in meters. X+ east, Y+ north, Z up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector CenterMeters = FVector::ZeroVector;

	/** Full size in meters. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector SizeMeters = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusMaterialType Material = ENexusMaterialType::Concrete;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusBoxUsage Usage = ENexusBoxUsage::Structure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	bool bCanBeDestroyed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float Health = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float ThicknessMeters = 0.2f;
};

USTRUCT(BlueprintType)
struct FNexusSpawnDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	int32 SpawnID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusTeam Team = ENexusTeam::Attackers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector LocationMeters = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusRouteId PreferredRoute = ENexusRouteId::RouteA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float SafeZoneRadiusMeters = 4.f;
};

USTRUCT(BlueprintType)
struct FNexusCalloutDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName CalloutId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector CenterMeters = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector ExtentMeters = FVector(5.f, 5.f, 4.f);
};

USTRUCT(BlueprintType)
struct FNexusRouteDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusRouteId RouteId = ENexusRouteId::RouteA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	TArray<FVector> PointsMeters;
};

USTRUCT(BlueprintType)
struct FNexusDoorDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector LocationMeters = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusDoorType Type = ENexusDoorType::Standard;
};

USTRUCT(BlueprintType)
struct FNexusCoverDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector LocationMeters = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusCoverType Type = ENexusCoverType::Low;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float WidthMeters = 1.2f;
};

USTRUCT(BlueprintType)
struct FNexusPlantZoneDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusSiteId Site = ENexusSiteId::A;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName ZoneId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector CenterMeters = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float RadiusMeters = 1.5f;
};

USTRUCT(BlueprintType)
struct FNexusSiteDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusSiteId SiteId = ENexusSiteId::A;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector CenterMeters = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FVector SizeMeters = FVector(20.f, 18.f, 6.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	TArray<FNexusPlantZoneDef> PlantZones;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	TArray<FVector> EntranceCentersMeters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	TArray<FVector> DefensivePositionsMeters;
};
