#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Art/NexusArtTypes.h"
#include "NexusAudio.generated.h"

UCLASS()
class NEXUS_API UNexusAudio : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void PlayFire(UObject* WorldContext, const FVector& Location);
	static void PlayReload(UObject* WorldContext, const FVector& Location);
	static void PlayFootstep(UObject* WorldContext, const FVector& Location, ENexusFootstepSurface Surface);
	static void PlayHitConfirm(UObject* WorldContext, bool bHeadshot);
	static void PlayPlant(UObject* WorldContext, const FVector& Location);
	static void PlayRoundSting(UObject* WorldContext, bool bStart);

	static float OccludedVolume(UWorld* World, const FVector& From, const FVector& To, float InVolume);
};
