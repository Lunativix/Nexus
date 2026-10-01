#pragma once

#include "CoreMinimal.h"

class ANavMeshBoundsVolume;

/** Rebuild the cube UModel on a NavMeshBoundsVolume without changing location/scale. */
void NexusEnsureNavMeshBoundsBrush(ANavMeshBoundsVolume* Volume);
