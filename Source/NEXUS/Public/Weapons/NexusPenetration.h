#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NexusTypes.h"
#include "NexusPenetration.generated.h"

UCLASS()
class NEXUS_API UNexusPenetrationStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NEXUS|Weapons")
	static FLinearColor GetMaterialColor(ENexusMaterialType Type);

	UFUNCTION(BlueprintCallable, Category = "NEXUS|Weapons")
	static float GetDamageReduction(ENexusMaterialType Type, float ThicknessMeters);

	/** Returns remaining damage after traversing HitActor. 0 means the round stopped. */
	UFUNCTION(BlueprintCallable, Category = "NEXUS|Weapons")
	static float ApplySurfaceToDamage(AActor* HitActor, float IncomingDamage, float ThicknessMeters, ENexusMaterialType Type);
};
