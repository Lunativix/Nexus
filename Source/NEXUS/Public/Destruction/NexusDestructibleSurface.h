#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusDestructibleSurface.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class NEXUS_API ANexusDestructibleSurface : public AActor
{
	GENERATED_BODY()

public:
	ANexusDestructibleSurface();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float Health = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusMaterialType MaterialType = ENexusMaterialType::Plaster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float Thickness = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float PenetrationModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	float ExplosionModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	bool bCanBeDestroyed = true;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float ReplicatedHealth = 500.f;

	float InitialHealth = 500.f;

	UFUNCTION(BlueprintCallable, Category = "NEXUS")
	void ApplyDamage(float Amount);

	UFUNCTION()
	void OnRep_Health();

	void RefreshVisual();
	void ResetForNewRound();
};
