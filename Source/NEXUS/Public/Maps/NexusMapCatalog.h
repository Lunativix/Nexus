#pragma once

#include "CoreMinimal.h"
#include "Maps/NexusVelasqoLayout.h"

class UWorld;

/** NEXUS V1.0 — the three maps from the GDD poster. */
enum class ENexusMapId : uint8
{
	Velasqo,
	Skyline,
	Outpost
};

struct NEXUS_API FNexusMapCatalog
{
	/** Detects the map from the world/package name. Defaults to Velasqo. */
	static ENexusMapId DetectFromWorld(const UWorld* World);

	/** Builds the greybox layout for a map. Velasqo is byte-identical to FNexusVelasqoLayout::Build(). */
	static FNexusVelasqoLayout BuildLayout(ENexusMapId MapId);

	static const TCHAR* MapName(ENexusMapId MapId);

	/** Half-extent of NavMeshBoundsVolume scale (brush 2 m). Velasqo 60 → 120 m. */
	static float NavBoundsScale(ENexusMapId MapId);

	/** Playable half-extent in Unreal units. Velasqo 6000, Skyline 6500, Outpost 7000. */
	static float PlayableHalfUU(ENexusMapId MapId);

	/** Content package path without .umap, e.g. /Game/NEXUS/Maps/Skyline/LV_Skyline */
	static const TCHAR* PackagePath(ENexusMapId MapId);
};

/** Implemented in NexusSkylineLayout.cpp / NexusOutpostLayout.cpp. */
FNexusVelasqoLayout NexusBuildSkylineLayout();
FNexusVelasqoLayout NexusBuildOutpostLayout();
