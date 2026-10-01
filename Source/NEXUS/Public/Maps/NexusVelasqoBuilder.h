#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusVelasqoBuilder.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMesh;

UCLASS(Blueprintable)
class NEXUS_API ANexusVelasqoBuilder : public AActor
{
	GENERATED_BODY()

public:
	ANexusVelasqoBuilder();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "NEXUS")
	void BuildMap();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "NEXUS")
	void ClearMap();

	UPROPERTY(EditAnywhere, Category = "NEXUS")
	bool bBuildOnBeginPlay = true;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	bool bBuilt = false;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TArray<TObjectPtr<UInstancedStaticMeshComponent>> MaterialMeshes;

protected:
	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UInstancedStaticMeshComponent* GetMeshFor(ENexusMaterialType Type);
	void SpawnGameplay(const struct FNexusVelasqoLayout& Layout);
};
