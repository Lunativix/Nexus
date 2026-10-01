#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.h"
#include "NexusAIController.generated.h"

class ANexusRouteSpline;
class ANexusBombSite;
class ANexusCharacter;

UCLASS()
class NEXUS_API ANexusAIController : public AAIController
{
	GENERATED_BODY()

public:
	ANexusAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	ENexusBotState BotState = ENexusBotState::Idle;

	UPROPERTY()
	ENexusBotRole BotRole = ENexusBotRole::Support;

protected:
	void PickGoal();
	void TryShoot();
	void TryObjective();
	void TryBuy();
	float AimError() const;
	float ReactionDelay() const;

	UPROPERTY()
	TObjectPtr<ANexusRouteSpline> CurrentRoute;

	float ThinkTimer = 0.f;
	int32 RoutePoint = 0;
	ENexusSiteId PlannedSite = ENexusSiteId::A;
};
