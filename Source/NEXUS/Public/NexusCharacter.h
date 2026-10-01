#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusTypes.h"
#include "NexusCombatTypes.h"
#include "NexusCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UStaticMeshComponent;
class UNexusHitscanComponent;
class ANexusBombSite;
class ANexusGameState;
class ANexusPlayerState;

UCLASS(Blueprintable)
class NEXUS_API ANexusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANexusCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UNexusHitscanComponent> Hitscan;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<USceneComponent> ViewmodelRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> ViewWeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> ViewBarrelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> ViewMagMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> LeftHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> RightHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> MuzzleFlashMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> WorldWeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> HelmetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> VestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NEXUS")
	TObjectPtr<UStaticMeshComponent> PackMesh;

	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyControllerChanged() override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	float Health = 100.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bIsPlanting = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bIsDefusing = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	float ObjectiveProgress = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_Callout)
	FText CurrentCallout;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bSpawnProtected = false;

	UPROPERTY(ReplicatedUsing = OnRep_ADS, BlueprintReadOnly, Category = "NEXUS")
	bool bADS = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bReloading = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "NEXUS")
	bool bSwitching = false;

	UFUNCTION()
	void OnRep_Callout();
	UFUNCTION()
	void OnRep_ADS();

	void ReceiveNexusDamage(float Amount, AActor* DamageCauser, ENexusHitZone Zone = ENexusHitZone::Torso, FName Weapon = NAME_None, bool bWallbang = false);
	void SetCallout(const FText& InCallout);

	UFUNCTION(Server, Reliable)
	void ServerStartFire();
	UFUNCTION(Server, Reliable)
	void ServerStopFire();
	UFUNCTION(Server, Reliable)
	void ServerInteractPressed();
	UFUNCTION(Server, Reliable)
	void ServerInteractReleased();
	UFUNCTION(Server, Reliable)
	void ServerToggleDoor();
	UFUNCTION(Server, Reliable)
	void ServerReload();
	UFUNCTION(Server, Reliable)
	void ServerSetADS(bool bNewADS);
	UFUNCTION(Server, Reliable)
	void ServerSwitchWeapon(uint8 Slot);
	UFUNCTION(Server, Reliable)
	void ServerUseGadget(uint8 Index);
	UFUNCTION(Server, Reliable)
	void ServerUseUltimate();
	UFUNCTION(Server, Reliable)
	void ServerDropBomb();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastWeaponFired();

	ENexusTeam GetTeam() const;
	bool HasBomb() const;
	bool IsAlive() const { return Health > 0.f; }
	ANexusPlayerState* GetNexusPS() const;
	void ApplyMoveSpeedFromOperator();

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void OnFirePressed();
	void OnFireReleased();
	void OnInteractPressed();
	void OnInteractReleased();
	void OnCrouchPressed();
	void OnUsePressed();
	void OnReloadPressed();
	void OnADSPressed();
	void OnADSReleased();
	void OnPrimary();
	void OnSecondary();
	void OnMelee();
	void OnGadget1();
	void OnGadget2();
	void OnUltimate();
	void OnDropBomb();

	void FireShot();
	void FinishReload();
	void FinishSwitch();
	void TickObjective();
	void AbortObjective();
	void EnableSpawnProtection();
	void ClearSpawnProtection();
	ANexusBombSite* FindSiteAtFeet() const;
	bool CanStartPlant(ANexusBombSite* Site, const ANexusGameState* GS) const;
	bool CanStartDefuse(ANexusBombSite* Site, const ANexusGameState* GS) const;
	bool CanShoot() const;
	const FNexusWeaponDefinition* CurrentWeapon() const;
	void DeployGadget(ENexusGadgetId Id);
	void SetupViewmodel();
	void ApplyWeaponVisuals();
	void UpdateViewmodel(float DeltaSeconds);
	void ApplyOperatorLook();
	void HideMuzzle();
	FVector ViewmodelHipOffset() const;
	FVector ViewmodelADSOffset() const;

	FTimerHandle FireTimer;
	FTimerHandle ObjectiveTimer;
	FTimerHandle SpawnProtectTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle SwitchTimer;
	FTimerHandle MuzzleTimer;
	bool bWantsFire = false;
	bool bWantsInteract = false;
	int32 RecoilShots = 0;
	FName ShownWeapon = NAME_None;
	float ViewKickPitch = 0.f;
	float ViewSwayTime = 0.f;
	float LastShotWorldTime = -100.f;
	float LastFootstepTime = 0.f;
	ENexusOperatorId ShownOperator = ENexusOperatorId::None;
	FVector PlantStartLocation = FVector::ZeroVector;
};
