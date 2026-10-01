#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NexusInteractable.generated.h"

class ANexusCharacter;

UINTERFACE(MinimalAPI)
class UNexusInteractable : public UInterface
{
	GENERATED_BODY()
};

class NEXUS_API INexusInteractable
{
	GENERATED_BODY()

public:
	virtual bool CanNexusInteract(ANexusCharacter* Character) const;
	virtual void ExecuteNexusInteract(ANexusCharacter* Character);
	virtual FVector GetNexusInteractLocation() const;
};

namespace NexusInteraction
{
	bool HasLineOfSight(UWorld* World, const FVector& From, AActor* Target, AActor* Instigator);
	AActor* FindBestInteractable(ANexusCharacter* Character, float MaxDistance);
}
