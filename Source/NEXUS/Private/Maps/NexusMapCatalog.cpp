#include "Maps/NexusMapCatalog.h"
#include "Engine/World.h"

ENexusMapId FNexusMapCatalog::DetectFromWorld(const UWorld* World)
{
	if (!World)
	{
		return ENexusMapId::Velasqo;
	}
	const FString Name = World->GetMapName(); // includes PIE prefix, safe with Contains
	if (Name.Contains(TEXT("Skyline")))
	{
		return ENexusMapId::Skyline;
	}
	if (Name.Contains(TEXT("Outpost")))
	{
		return ENexusMapId::Outpost;
	}
	return ENexusMapId::Velasqo;
}

FNexusVelasqoLayout FNexusMapCatalog::BuildLayout(ENexusMapId MapId)
{
	switch (MapId)
	{
	case ENexusMapId::Skyline: return NexusBuildSkylineLayout();
	case ENexusMapId::Outpost: return NexusBuildOutpostLayout();
	default: return FNexusVelasqoLayout::Build();
	}
}

const TCHAR* FNexusMapCatalog::MapName(ENexusMapId MapId)
{
	switch (MapId)
	{
	case ENexusMapId::Skyline: return TEXT("SKYLINE");
	case ENexusMapId::Outpost: return TEXT("OUTPOST");
	default: return TEXT("VELASQO");
	}
}

float FNexusMapCatalog::NavBoundsScale(ENexusMapId MapId)
{
	switch (MapId)
	{
	case ENexusMapId::Skyline: return 65.f;
	case ENexusMapId::Outpost: return 70.f;
	default: return 60.f;
	}
}

float FNexusMapCatalog::PlayableHalfUU(ENexusMapId MapId)
{
	return NavBoundsScale(MapId) * 100.f;
}

const TCHAR* FNexusMapCatalog::PackagePath(ENexusMapId MapId)
{
	switch (MapId)
	{
	case ENexusMapId::Skyline: return TEXT("/Game/NEXUS/Maps/Skyline/LV_Skyline");
	case ENexusMapId::Outpost: return TEXT("/Game/NEXUS/Maps/Outpost/LV_Outpost");
	default: return TEXT("/Game/NEXUS/Maps/Velasqo/LV_Velasqo_Greybox");
	}
}
