#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusTypes.h"
#include "NexusInteractable.h"
#include "NexusDoor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNavModifierComponent;

UCLASS(Blueprintable, BlueprintType)
class NEXUS_API ANexusDoor : public AActor, public INexusInteractable
{
	GENERATED_BODY()

public:
	ANexusDoor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UBoxComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEXUS")
	ENexusDoorType DoorType = ENexusDoorType::Standard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Open, Category = "NEXUS")
	bool bOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Health, Category = "NEXUS")
	float Health = 500.f;

	UFUNCTION(BlueprintCallable, Category = "NEXUS")
	void ServerToggle();

	UFUNCTION(BlueprintCallable, Category = "NEXUS")
	void ApplyDamage(float Amount);

	UFUNCTION()
	void OnRep_Open();

	UFUNCTION()
	void OnRep_Health();

	void ApplyVisualState();
	void ResetForNewRound();

	UPROPERTY(ReplicatedUsing = OnRep_DoorState, BlueprintReadOnly, Category = "NEXUS")
	ENexusDoorState DoorState = ENexusDoorState::Closed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "NEXUS")
	bool bLocked = false;

	UFUNCTION()
	void OnRep_DoorState();

	virtual bool CanNexusInteract(ANexusCharacter* Character) const override;
	virtual void ExecuteNexusInteract(ANexusCharacter* Character) override;

	ENexusDoorState ComputeState() const;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
