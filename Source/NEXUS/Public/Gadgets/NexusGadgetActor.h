#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusCombatTypes.h"
#include "NexusGadgetActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class NEXUS_API ANexusGadgetActor : public AActor
{
	GENERATED_BODY()

public:
	ANexusGadgetActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(Replicated)
	ENexusGadgetId GadgetId = ENexusGadgetId::None;

	UPROPERTY(Replicated)
	ENexusTeam OwnerTeam = ENexusTeam::Spectator;

	UPROPERTY(Replicated)
	float RadiusUu = 500.f;

	UPROPERTY(Replicated)
	bool bJamming = false;

	UPROPERTY(Replicated)
	bool bSmoke = false;

	UPROPERTY(Replicated)
	bool bMarking = false;

	UPROPERTY(VisibleAnywhere, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	void Arm(ENexusGadgetId InId, ENexusTeam Team, float Duration, float RadiusMeters);
	bool IsCounteredBy(const ANexusGadgetActor* Other) const;

protected:
	void Expire();
	FTimerHandle LifeTimer;
};
