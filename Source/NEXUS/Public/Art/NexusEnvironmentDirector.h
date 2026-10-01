#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maps/NexusMapCatalog.h"
#include "NexusEnvironmentDirector.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMesh;

UCLASS()
class NEXUS_API ANexusEnvironmentDirector : public AActor
{
	GENERATED_BODY()

public:
	ANexusEnvironmentDirector();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "NEXUS")
	void ApplyArtPass();

	static void Ensure(UWorld* World);

	int32 CountVisualInstances() const;

protected:
	void ApplyGameplayMaterials();
	void ApplyLightingAndFog();
	void SpawnVisualModules();
	void SpawnMarketProps(const struct FNexusVelasqoLayout& Layout);
	void SpawnWarehouseProps(const struct FNexusVelasqoLayout& Layout);
	void SpawnCityProduction(const struct FNexusVelasqoLayout& Layout);
	void SpawnRooftopKit(const struct FNexusVelasqoLayout& Layout);
	void SpawnAlleyDressing(const struct FNexusVelasqoLayout& Layout);
	void SpawnPlazaAndFountain(const struct FNexusVelasqoLayout& Layout);
	void SpawnVehiclesAndMilitary(const struct FNexusVelasqoLayout& Layout);
	void SpawnFoliageAndSignage(const struct FNexusVelasqoLayout& Layout);
	void SpawnBackdropSkyline();
	void SpawnPracticalLights();
	void ConcealGreyboxVisuals();
	void RebuildArchitecture(const struct FNexusVelasqoLayout& Layout);
	void SkinBox(const struct FNexusBoxDef& Box, int32& Windows, int32& Modules);
	void SpawnPavementGrid();
	void SpawnFountainArchitecture();
	void DressGameplayDoors();
	void PlaceWindowKit(const FVector& CenterMeters, bool bAlongY, float OutSign, int32& Windows, bool bDeepFacade = false);
	void PlaceRoofKit(const FVector& RoofMeters, int32& Count);
	void PlaceRoofParapet(const FVector& CenterMeters, const FVector& SizeMeters);
	void PlacePitchedRoof(const FVector& CenterMeters, const FVector& SizeMeters);
	void SkinBrokenFacade(const FVector& WallCenter, float Len, float Thick, float Height, float Bottom,
		bool bThinX, float OutSign, UInstancedStaticMeshComponent* Body, int32& Windows, int32& Modules, bool bTwoStory, bool bSkipOpenings = false);
	void PlaceColumn(const FVector& CenterMeters, const FVector& SizeMeters);
	void PlaceVehicleKit(const FVector& CenterMeters, const FVector& SizeMeters, const FRotator& Rot = FRotator::ZeroRotator);
	void PlaceContainerKit(const FVector& CenterMeters, const FVector& SizeMeters);
	void PlaceTankKit(const FVector& CenterMeters, const FVector& SizeMeters);
	void PlaceBackdropTower(const FVector& CenterMeters, const FVector& SizeMeters, bool bGlass);
	void DressGameplayCover();
	void SpawnCaravanseraiDress(const struct FNexusVelasqoLayout& Layout);
	void SpawnBenchmarkBlock(const struct FNexusVelasqoLayout& Layout);
	void SpawnSkylineDress(const struct FNexusVelasqoLayout& Layout);
	void SpawnOutpostDress(const struct FNexusVelasqoLayout& Layout);
	void SpawnVelasqoGddSpots(const struct FNexusVelasqoLayout& Layout);
	void PlaceMosqueKit(const FVector& CenterMeters);
	void PlaceHelipadKit(const FVector& CenterMeters);
	void PlaceHeliKit(const FVector& CenterMeters, const FRotator& Rot = FRotator::ZeroRotator);
	void PlaceWatchtowerKit(const FVector& CenterMeters);
	bool IsBenchmarkBlock(const FVector& Meters) const;
	bool IsGameplaySensitive(const struct FNexusVelasqoLayout& Layout, const FVector& Meters) const;
	bool IsNearSpawn(const struct FNexusVelasqoLayout& Layout, const FVector& Meters) const;
	bool CanDress(const struct FNexusVelasqoLayout& Layout, const FVector& Meters) const;
	void AddVisualInstance(UInstancedStaticMeshComponent* ISM, const FVector& Loc, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator);
	void ConfigureVisualISM(UInstancedStaticMeshComponent* ISM);

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualWindows;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualTrim;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualProps;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualGround;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualMarketCloth;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualMarketGoods;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualWarehouse;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualPipes;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualRoofs;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualVehicles;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualFoliage;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualSigns;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualBackdrop;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualWater;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualFacades;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualMilitary;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UInstancedStaticMeshComponent> VisualDomes;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> ConeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> PlaneMesh;

	UPROPERTY()
	bool bArtApplied = false;

	ENexusMapId ActiveMap = ENexusMapId::Velasqo;
};
