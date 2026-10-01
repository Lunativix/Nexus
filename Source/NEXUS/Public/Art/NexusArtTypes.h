#pragma once

#include "CoreMinimal.h"
#include "NexusTypes.h"
#include "NexusArtTypes.generated.h"

UENUM(BlueprintType)
enum class ENexusTimeOfDay : uint8
{
	Morning,
	Day,
	Evening,
	Night
};

UENUM(BlueprintType)
enum class ENexusArtZone : uint8
{
	None,
	Market,
	OldQuarter,
	BlueHouse,
	EastHouse,
	Caravanserai,
	SiteA,
	SiteB,
	SouthAlleys,
	Rooftops,
	Plaza,
	Warehouse,
	Gallery
};

UENUM(BlueprintType)
enum class ENexusArtSurface : uint8
{
	OldStone,
	Plaster,
	PaintedConcrete,
	DirtyConcrete,
	Sandstone,
	Wood,
	Metal,
	RustyMetal,
	Glass,
	Fabric,
	Asphalt,
	RoadDust,
	Ceramic,
	Pavement,
	Foliage,
	Water
};

UENUM(BlueprintType)
enum class ENexusFootstepSurface : uint8
{
	Concrete,
	Stone,
	Wood,
	Metal,
	Sand,
	Glass
};

USTRUCT(BlueprintType)
struct FNexusFogPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float FogDensity = 0.012f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float FogHeightFalloff = 0.22f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	FLinearColor FogInscattering = FLinearColor(0.62f, 0.42f, 0.24f);

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float FogStartDistance = 900.f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float FogMaxOpacity = 0.42f;
};

USTRUCT(BlueprintType)
struct FNexusTimeOfDayPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	FRotator SunRotation = FRotator(-22.f, 228.f, 0.f);

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	FLinearColor SunColor = FLinearColor(1.f, 0.68f, 0.38f);

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float SunIntensity = 7.5f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	float SkyIntensity = 0.85f;

	UPROPERTY(EditAnywhere, Config, Category = "NEXUS")
	FLinearColor SkyColor = FLinearColor(0.55f, 0.48f, 0.62f);
};

inline FLinearColor NexusArtSurfaceColor(ENexusArtSurface Surface)
{
	switch (Surface)
	{
	case ENexusArtSurface::OldStone: return FLinearColor(0.38f, 0.32f, 0.24f);
	case ENexusArtSurface::Plaster: return FLinearColor(0.82f, 0.74f, 0.58f);
	case ENexusArtSurface::PaintedConcrete: return FLinearColor(0.62f, 0.63f, 0.58f);
	case ENexusArtSurface::DirtyConcrete: return FLinearColor(0.28f, 0.27f, 0.24f);
	case ENexusArtSurface::Sandstone: return FLinearColor(0.64f, 0.50f, 0.32f);
	case ENexusArtSurface::Wood: return FLinearColor(0.28f, 0.16f, 0.08f);
	case ENexusArtSurface::Metal: return FLinearColor(0.18f, 0.19f, 0.20f);
	case ENexusArtSurface::RustyMetal: return FLinearColor(0.36f, 0.18f, 0.08f);
	case ENexusArtSurface::Glass: return FLinearColor(0.22f, 0.38f, 0.42f);
	case ENexusArtSurface::Fabric: return FLinearColor(0.55f, 0.18f, 0.12f);
	case ENexusArtSurface::Asphalt: return FLinearColor(0.035f, 0.034f, 0.032f);
	case ENexusArtSurface::RoadDust: return FLinearColor(0.48f, 0.38f, 0.22f);
	case ENexusArtSurface::Ceramic: return FLinearColor(0.55f, 0.28f, 0.20f);
	case ENexusArtSurface::Pavement: return FLinearColor(0.46f, 0.40f, 0.32f);
	case ENexusArtSurface::Foliage: return FLinearColor(0.14f, 0.26f, 0.10f);
	case ENexusArtSurface::Water: return FLinearColor(0.12f, 0.28f, 0.34f);
	default: return FLinearColor(0.45f, 0.40f, 0.32f);
	}
}

inline ENexusArtSurface NexusArtSurfaceFromGameplay(ENexusMaterialType Type)
{
	switch (Type)
	{
	case ENexusMaterialType::Wood: return ENexusArtSurface::Wood;
	case ENexusMaterialType::Plaster: return ENexusArtSurface::Plaster;
	case ENexusMaterialType::LightStone: return ENexusArtSurface::Sandstone;
	case ENexusMaterialType::ThickStone: return ENexusArtSurface::OldStone;
	case ENexusMaterialType::Concrete: return ENexusArtSurface::DirtyConcrete;
	case ENexusMaterialType::Structural: return ENexusArtSurface::Metal;
	case ENexusMaterialType::Glass: return ENexusArtSurface::Glass;
	default: return ENexusArtSurface::PaintedConcrete;
	}
}
