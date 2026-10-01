#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusCalloutVolume.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class NEXUS_API ANexusCalloutVolume : public AActor
{
	GENERATED_BODY()

public:
	ANexusCalloutVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FName CalloutId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	FText DisplayName;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
