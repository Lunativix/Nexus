#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NexusCombatTypes.h"
#include "NexusHitscanComponent.generated.h"

struct FNexusWeaponDefinition;

USTRUCT(BlueprintType)
struct FNexusHitscanResult
{
	GENERATED_BODY()

	UPROPERTY()
	float DamageDealt = 0.f;

	UPROPERTY()
	int32 SurfacesPenetrated = 0;

	UPROPERTY()
	TObjectPtr<AActor> HitCharacter = nullptr;

	UPROPERTY()
	bool bStoppedByStructural = false;

	UPROPERTY()
	bool bHeadshot = false;

	UPROPERTY()
	ENexusHitZone HitZone = ENexusHitZone::Torso;

	UPROPERTY()
	float DistanceUu = 0.f;
};

UCLASS(ClassGroup = (NEXUS), meta = (BlueprintSpawnableComponent))
class NEXUS_API UNexusHitscanComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNexusHitscanComponent();

	UFUNCTION(BlueprintCallable, Category = "NEXUS")
	void Fire(const FVector& Start, const FVector& Direction, AActor* InstigatorActor);

	FNexusHitscanResult FireWithResult(const FVector& Start, const FVector& Direction, AActor* InstigatorActor);
	FNexusHitscanResult FireWithWeapon(const FVector& Start, const FVector& Direction, AActor* InstigatorActor, const FNexusWeaponDefinition& Weapon, float SpreadDeg);
};
