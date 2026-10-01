#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Art/NexusArtTypes.h"
#include "NexusMaterialLibrary.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;
class UPrimitiveComponent;

UCLASS()
class NEXUS_API UNexusMaterialLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static const TCHAR* MasterPath(const TCHAR* Kind);
	static const TCHAR* InstancePath(ENexusArtSurface Surface);

	static UMaterialInterface* LoadMaster(const TCHAR* Kind);
	static UMaterialInterface* LoadSurface(ENexusArtSurface Surface);
	static UMaterialInterface* SolidColorParent();
	static bool IsDebugOrMissingMaterial(UMaterialInterface* Material);
	static UMaterialInstanceDynamic* MakeSurfaceMID(UObject* Outer, ENexusArtSurface Surface);
	static void ApplyToPrimitive(UPrimitiveComponent* Comp, ENexusArtSurface Surface, int32 ElementIndex = 0);
	static void ApplyGameplayMaterial(UPrimitiveComponent* Comp, ENexusMaterialType Type);
};
