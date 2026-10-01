#pragma once

#include "CoreMinimal.h"

struct NEXUS_API FNexusVelasqoValidation
{
	static void ValidateMap(UWorld* World);
	static void FullTest(UWorld* World);
	static void WalkTest(UWorld* World);
	static bool TeleportToLandmark(UWorld* World, const FString& Landmark);
	static void Audit(UWorld* World);
};
