#pragma once

#include "CoreMinimal.h"

struct NEXUS_API FNexusValidationResult
{
	FString Name;
	bool bPass = false;
	FString Detail;
};

struct NEXUS_API FNexusLevelValidator
{
	static TArray<FNexusValidationResult> Run(UWorld* World);
};
