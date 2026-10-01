#pragma once

#include "CoreMinimal.h"
#include "NexusTypes.h"

struct FNexusVelasqoLayout
{
	TArray<FNexusBoxDef> Boxes;
	TArray<FNexusSpawnDef> Spawns;
	TArray<FNexusCalloutDef> Callouts;
	TArray<FNexusRouteDef> Routes;
	TArray<FNexusDoorDef> Doors;
	TArray<FNexusCoverDef> Covers;
	TArray<FNexusSiteDef> Sites;

	static FNexusVelasqoLayout Build();
};
