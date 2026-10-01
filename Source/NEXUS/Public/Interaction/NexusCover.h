#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusCover.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class NEXUS_API ANexusCover : public AActor
{
	GENERATED_BODY()

public:
	ANexusCover();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusCoverType CoverType = ENexusCoverType::Low;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float WidthMeters = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusMaterialType Material = ENexusMaterialType::Concrete;
};
