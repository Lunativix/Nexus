#include "NexusCharacter.h"
#include "NexusPlayerState.h"
#include "NexusGameMode.h"
#include "NexusGameState.h"
#include "NexusGameplaySettings.h"
#include "NexusPlayerController.h"
#include "Weapons/NexusHitscanComponent.h"
#include "Gadgets/NexusGadgetActor.h"
#include "Objectives/NexusBombSite.h"
#include "Interaction/NexusDoor.h"
#include "Interaction/NexusCalloutVolume.h"
#include "Interaction/NexusInteractable.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/NexusPenetration.h"
#include "NEXUS.h"
#include "Art/NexusMaterialLibrary.h"
#include "Art/NexusArtSettings.h"
#include "Art/NexusAudio.h"

ANexusCharacter::ANexusCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	SetReplicateMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.f);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	BodyMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.7f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube.Succeeded())
	{
		BodyMesh->SetStaticMesh(Cube.Object);
	}
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Hitscan = CreateDefaultSubobject<UNexusHitscanComponent>(TEXT("Hitscan"));

	ViewmodelRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ViewmodelRoot"));
	ViewmodelRoot->SetupAttachment(FirstPersonCamera);
	ViewmodelRoot->SetRelativeLocation(FVector(22.f, 10.f, -12.f));

	auto MakeVM = [&](const TCHAR* Name) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* M = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		M->SetupAttachment(ViewmodelRoot);
		M->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		M->SetCastShadow(false);
		M->SetOnlyOwnerSee(true);
		M->bOwnerNoSee = false;
		return M;
	};
	ViewWeaponMesh = MakeVM(TEXT("ViewWeapon"));
	ViewBarrelMesh = MakeVM(TEXT("ViewBarrel"));
	ViewMagMesh = MakeVM(TEXT("ViewMag"));
	LeftHandMesh = MakeVM(TEXT("LeftHand"));
	RightHandMesh = MakeVM(TEXT("RightHand"));
	MuzzleFlashMesh = MakeVM(TEXT("MuzzleFlash"));
	MuzzleFlashMesh->SetHiddenInGame(true);

	WorldWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldWeapon"));
	WorldWeaponMesh->SetupAttachment(BodyMesh);
	WorldWeaponMesh->SetRelativeLocation(FVector(28.f, 18.f, 8.f));
	WorldWeaponMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	WorldWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WorldWeaponMesh->SetCastShadow(false);
	WorldWeaponMesh->SetOwnerNoSee(true);

	auto MakeGear = [&](const TCHAR* Name, const FVector& Loc, const FVector& Scale) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* M = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		M->SetupAttachment(BodyMesh);
		M->SetRelativeLocation(Loc);
		M->SetRelativeScale3D(Scale);
		M->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		M->SetCastShadow(false);
		M->SetOwnerNoSee(true);
		if (Cube.Succeeded())
		{
			M->SetStaticMesh(Cube.Object);
		}
		return M;
	};
	HelmetMesh = MakeGear(TEXT("Helmet"), FVector(0.f, 0.f, 55.f), FVector(0.42f, 0.38f, 0.22f));
	VestMesh = MakeGear(TEXT("Vest"), FVector(0.f, 0.f, 8.f), FVector(0.62f, 0.5f, 0.55f));
	PackMesh = MakeGear(TEXT("Pack"), FVector(-18.f, 0.f, 6.f), FVector(0.22f, 0.32f, 0.4f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cyl(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (Cube.Succeeded())
	{
		ViewWeaponMesh->SetStaticMesh(Cube.Object);
		ViewMagMesh->SetStaticMesh(Cube.Object);
		LeftHandMesh->SetStaticMesh(Cube.Object);
		RightHandMesh->SetStaticMesh(Cube.Object);
		MuzzleFlashMesh->SetStaticMesh(Cube.Object);
		WorldWeaponMesh->SetStaticMesh(Cube.Object);
	}
	if (Cyl.Succeeded())
	{
		ViewBarrelMesh->SetStaticMesh(Cyl.Object);
	}

	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetOwnerNoSee(true);
	BodyMesh->SetOwnerNoSee(true);

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ANexusCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = GetDefault<UNexusGameplaySettings>()->MaxHealth;
	GetCharacterMovement()->MaxWalkSpeed = GetDefault<UNexusGameplaySettings>()->WalkSpeedUu;
	if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
	{
		if (UMaterialInstanceDynamic* MID = BodyMesh->CreateDynamicMaterialInstance(0, Base))
		{
			const FLinearColor Color = (GetTeam() == ENexusTeam::Defenders)
				? FLinearColor(0.2f, 0.35f, 0.8f)
				: FLinearColor(0.75f, 0.25f, 0.15f);
			MID->SetVectorParameterValue(TEXT("Color"), Color);
		}
	}
	SetupViewmodel();
	ApplyWeaponVisuals();
}

void ANexusCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ApplyWeaponVisuals();
	if (IsLocallyControlled())
	{
		UpdateViewmodel(DeltaSeconds);
		const float Speed = GetVelocity().Size2D();
		if (Speed > 80.f && GetWorld()->GetTimeSeconds() - LastFootstepTime > (Speed > 350.f ? 0.32f : 0.42f))
		{
			LastFootstepTime = GetWorld()->GetTimeSeconds();
			UNexusAudio::PlayFootstep(this, GetActorLocation(), ENexusFootstepSurface::Concrete);
		}
	}
	ApplyOperatorLook();
}

void ANexusCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	ApplyWeaponVisuals();
}

void ANexusCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANexusCharacter, Health);
	DOREPLIFETIME(ANexusCharacter, bIsPlanting);
	DOREPLIFETIME(ANexusCharacter, bIsDefusing);
	DOREPLIFETIME(ANexusCharacter, ObjectiveProgress);
	DOREPLIFETIME(ANexusCharacter, CurrentCallout);
	DOREPLIFETIME(ANexusCharacter, bSpawnProtected);
	DOREPLIFETIME(ANexusCharacter, bADS);
	DOREPLIFETIME(ANexusCharacter, bReloading);
	DOREPLIFETIME(ANexusCharacter, bSwitching);
}

void ANexusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ANexusCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ANexusCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ANexusCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ANexusCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ANexusCharacter::OnFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ANexusCharacter::OnFireReleased);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ANexusCharacter::OnInteractPressed);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Released, this, &ANexusCharacter::OnInteractReleased);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ANexusCharacter::OnCrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Use"), IE_Pressed, this, &ANexusCharacter::OnUsePressed);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ANexusCharacter::OnReloadPressed);
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Pressed, this, &ANexusCharacter::OnADSPressed);
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Released, this, &ANexusCharacter::OnADSReleased);
	PlayerInputComponent->BindAction(TEXT("Weapon1"), IE_Pressed, this, &ANexusCharacter::OnPrimary);
	PlayerInputComponent->BindAction(TEXT("Weapon2"), IE_Pressed, this, &ANexusCharacter::OnSecondary);
	PlayerInputComponent->BindAction(TEXT("Weapon3"), IE_Pressed, this, &ANexusCharacter::OnMelee);
	PlayerInputComponent->BindAction(TEXT("Gadget1"), IE_Pressed, this, &ANexusCharacter::OnGadget1);
	PlayerInputComponent->BindAction(TEXT("Gadget2"), IE_Pressed, this, &ANexusCharacter::OnGadget2);
	PlayerInputComponent->BindAction(TEXT("Ultimate"), IE_Pressed, this, &ANexusCharacter::OnUltimate);
	PlayerInputComponent->BindAction(TEXT("DropBomb"), IE_Pressed, this, &ANexusCharacter::OnDropBomb);
}

void ANexusCharacter::MoveForward(float Value)
{
	if (!IsAlive() || bIsPlanting || bIsDefusing) { return; }
	const ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	if (GS && (GS->Phase == ENexusRoundPhase::Freeze || GS->Phase == ENexusRoundPhase::BuyPhase)) { return; }
	AddMovementInput(GetActorForwardVector(), Value);
}

void ANexusCharacter::MoveRight(float Value)
{
	if (!IsAlive() || bIsPlanting || bIsDefusing) { return; }
	const ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	if (GS && (GS->Phase == ENexusRoundPhase::Freeze || GS->Phase == ENexusRoundPhase::BuyPhase)) { return; }
	AddMovementInput(GetActorRightVector(), Value);
}

void ANexusCharacter::Turn(float Value) { AddControllerYawInput(Value); }
void ANexusCharacter::LookUp(float Value) { AddControllerPitchInput(Value); }

void ANexusCharacter::OnFirePressed()
{
	if (IsLocallyControlled())
	{
		ViewKickPitch += 2.4f;
		UNexusAudio::PlayFire(this, GetActorLocation());
	}
	ServerStartFire();
}
void ANexusCharacter::OnFireReleased() { ServerStopFire(); }
void ANexusCharacter::OnInteractPressed() { ServerInteractPressed(); }
void ANexusCharacter::OnInteractReleased() { ServerInteractReleased(); }

void ANexusCharacter::OnCrouchPressed()
{
	if (bIsCrouched) { UnCrouch(); }
	else { Crouch(); }
}

void ANexusCharacter::OnUsePressed() { ServerToggleDoor(); }
void ANexusCharacter::OnReloadPressed() { ServerReload(); }
void ANexusCharacter::OnADSPressed()
{
	if (const FNexusWeaponDefinition* W = CurrentWeapon())
	{
		if (!W->bAllowADS)
		{
			return;
		}
	}
	if (IsLocallyControlled() && FirstPersonCamera)
	{
		bADS = true;
		OnRep_ADS();
	}
	ServerSetADS(true);
}
void ANexusCharacter::OnADSReleased()
{
	if (IsLocallyControlled() && FirstPersonCamera)
	{
		bADS = false;
		OnRep_ADS();
	}
	ServerSetADS(false);
}
void ANexusCharacter::OnPrimary() { ServerSwitchWeapon(0); }
void ANexusCharacter::OnSecondary() { ServerSwitchWeapon(1); }
void ANexusCharacter::OnMelee() { ServerSwitchWeapon(2); }
void ANexusCharacter::OnGadget1() { ServerUseGadget(0); }
void ANexusCharacter::OnGadget2() { ServerUseGadget(1); }
void ANexusCharacter::OnUltimate() { ServerUseUltimate(); }
void ANexusCharacter::OnDropBomb() { ServerDropBomb(); }

void ANexusCharacter::ServerStartFire_Implementation()
{
	if (!CanShoot())
	{
		return;
	}
	bWantsFire = true;
	FireShot();
	if (const FNexusWeaponDefinition* W = CurrentWeapon())
	{
		const float Interval = 60.f / FMath::Max(W->FireRateRPM, 1.f);
		GetWorldTimerManager().SetTimer(FireTimer, this, &ANexusCharacter::FireShot, Interval, true);
	}
}

void ANexusCharacter::ServerStopFire_Implementation()
{
	bWantsFire = false;
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void ANexusCharacter::FireShot()
{
	if (!bWantsFire || !CanShoot())
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
		return;
	}
	ANexusPlayerState* PS = GetNexusPS();
	const FNexusWeaponDefinition* W = CurrentWeapon();
	if (!PS || !W)
	{
		return;
	}
	if (W->Category == ENexusWeaponCategory::Sniper && W->bRequiresADS && !bADS)
	{
		return;
	}
	if (!HasAuthority())
	{
		return;
	}
	const float Now = GetWorld()->GetTimeSeconds();
	const float MinInterval = (60.f / FMath::Max(W->FireRateRPM, 1.f)) * 0.82f;
	if (Now - LastShotWorldTime < MinInterval)
	{
		UE_LOG(LogNexusWeapon, Verbose, TEXT("Rejected double-fire %s dt=%.3f"), *W->ID.ToString(), Now - LastShotWorldTime);
		return;
	}
	LastShotWorldTime = Now;
	if (PS->MagAmmo <= 0)
	{
		ServerReload();
		return;
	}
	PS->MagAmmo = FMath::Max(0, PS->MagAmmo - 1);
	RecoilShots += 1;
	const float Spread = bADS ? W->ADSSpread : W->HipFireSpread;
	const float RecoilKick = W->VerticalRecoil + RecoilShots * 0.04f;
	FVector Dir = FirstPersonCamera->GetForwardVector();
	Dir = Dir.RotateAngleAxis(-RecoilKick, FirstPersonCamera->GetRightVector());
	Dir = Dir.RotateAngleAxis(W->HorizontalRecoil * ((RecoilShots % 2) ? 1.f : -1.f), FVector::UpVector);
	const int32 Pellets = FMath::Max(1, W->PelletCount);
	for (int32 P = 0; P < Pellets; ++P)
	{
		Hitscan->FireWithWeapon(FirstPersonCamera->GetComponentLocation(), Dir, this, *W, Spread);
	}
	MulticastWeaponFired();
}

void ANexusCharacter::ServerToggleDoor_Implementation()
{
	if (Health <= 0.f || bIsPlanting || bIsDefusing)
	{
		return;
	}
	const float Dist = GetDefault<UNexusGameplaySettings>()->InteractDistanceUu;
	if (AActor* Target = NexusInteraction::FindBestInteractable(this, Dist))
	{
		if (INexusInteractable* Interact = Cast<INexusInteractable>(Target))
		{
			Interact->ExecuteNexusInteract(this);
		}
	}
}

void ANexusCharacter::ServerInteractPressed_Implementation()
{
	if (Health <= 0.f || bIsPlanting || bIsDefusing)
	{
		return;
	}
	bWantsInteract = true;
	ANexusBombSite* Site = FindSiteAtFeet();
	ANexusGameState* GS = GetWorld()->GetGameState<ANexusGameState>();
	if (GS && GS->bBombDropped && GetTeam() == ENexusTeam::Attackers
		&& FVector::Dist(GetActorLocation(), GS->DroppedBombLocation) < 180.f)
	{
		if (ANexusPlayerState* PS = GetNexusPS())
		{
			PS->bHasBomb = true;
			GS->bBombDropped = false;
		}
		return;
	}
	if (!Site || !GS)
	{
		return;
	}
	PlantStartLocation = GetActorLocation();
	if (CanStartPlant(Site, GS))
	{
		bIsPlanting = true;
		ObjectiveProgress = 0.f;
		UNexusAudio::PlayPlant(this, GetActorLocation());
		GetWorldTimerManager().SetTimer(ObjectiveTimer, this, &ANexusCharacter::TickObjective, 0.05f, true);
	}
	else if (CanStartDefuse(Site, GS))
	{
		bIsDefusing = true;
		ObjectiveProgress = 0.f;
		GetWorldTimerManager().SetTimer(ObjectiveTimer, this, &ANexusCharacter::TickObjective, 0.05f, true);
	}
}

void ANexusCharacter::ServerInteractReleased_Implementation()
{
	bWantsInteract = false;
	AbortObjective();
}

bool ANexusCharacter::CanStartPlant(ANexusBombSite* Site, const ANexusGameState* GS) const
{
	if (!Site || !GS || Health <= 0.f)
	{
		return false;
	}
	if (GS->Phase != ENexusRoundPhase::Live)
	{
		return false;
	}
	return GetTeam() == ENexusTeam::Attackers && HasBomb() && !GS->bBombPlanted && Site->IsInPlantZone(GetActorLocation());
}

bool ANexusCharacter::CanStartDefuse(ANexusBombSite* Site, const ANexusGameState* GS) const
{
	if (!Site || !GS || Health <= 0.f || !GS->bBombPlanted)
	{
		return false;
	}
	if (GS->Phase != ENexusRoundPhase::Planted && GS->Phase != ENexusRoundPhase::PostPlant)
	{
		return false;
	}
	return GetTeam() == ENexusTeam::Defenders && Site->SiteID == GS->PlantedSite
		&& FVector::Dist2D(GetActorLocation(), GS->BombWorldLocation) <= Site->PlantRadius;
}

ANexusBombSite* ANexusCharacter::FindSiteAtFeet() const
{
	ANexusBombSite* Best = nullptr;
	float BestDist = 2000.f;
	for (TActorIterator<ANexusBombSite> It(GetWorld()); It; ++It)
	{
		const float Dist = FVector::Dist2D(GetActorLocation(), It->GetActorLocation());
		if (Dist < BestDist)
		{
			BestDist = Dist;
			Best = *It;
		}
	}
	return Best;
}

void ANexusCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (HasAuthority())
	{
		EnableSpawnProtection();
		Health = GetDefault<UNexusGameplaySettings>()->MaxHealth;
		ApplyMoveSpeedFromOperator();
		if (ANexusPlayerState* PS = GetNexusPS())
		{
			if (const FNexusWeaponDefinition* W = GetDefault<UNexusGameplaySettings>()->FindWeapon(PS->EquippedWeapon))
			{
				PS->MagAmmo = W->MagazineSize;
				if (PS->ReserveAmmo <= 0) { PS->ReserveAmmo = W->ReserveAmmo; }
			}
		}
	}
}

void ANexusCharacter::TickObjective()
{
	if (!HasAuthority() || (!bIsPlanting && !bIsDefusing))
	{
		GetWorldTimerManager().ClearTimer(ObjectiveTimer);
		return;
	}
	const float DeltaSeconds = 0.05f;
	ANexusGameState* GS = GetWorld()->GetGameState<ANexusGameState>();
	if (!GS || GS->Phase == ENexusRoundPhase::PostRound || GS->Phase == ENexusRoundPhase::Reset)
	{
		AbortObjective();
		return;
	}
	if (FVector::Dist2D(GetActorLocation(), PlantStartLocation) > 25.f || GetVelocity().Size2D() > 20.f)
	{
		AbortObjective();
		return;
	}

	ANexusBombSite* Site = FindSiteAtFeet();
	ANexusGameMode* GM = GetWorld()->GetAuthGameMode<ANexusGameMode>();
	if (!Site || !GM)
	{
		AbortObjective();
		return;
	}

	if (bIsPlanting)
	{
		if (!Site->IsInPlantZone(GetActorLocation()) || !HasBomb() || Health <= 0.f)
		{
			AbortObjective();
			return;
		}
		ObjectiveProgress += DeltaSeconds / FMath::Max(Site->PlantTime, 0.01f);
		if (ObjectiveProgress >= 1.f)
		{
			bIsPlanting = false;
			ObjectiveProgress = 1.f;
			GetWorldTimerManager().ClearTimer(ObjectiveTimer);
			if (ANexusPlayerState* PS = GetPlayerState<ANexusPlayerState>())
			{
				PS->bHasBomb = false;
			}
			GM->NotifyBombPlanted(Site->SiteID, GetActorLocation(), GetPlayerState<ANexusPlayerState>());
		}
	}
	else if (bIsDefusing)
	{
		if (!GS->bBombPlanted || Health <= 0.f || FVector::Dist2D(GetActorLocation(), GS->BombWorldLocation) > Site->PlantRadius)
		{
			AbortObjective();
			return;
		}
		ObjectiveProgress += DeltaSeconds / FMath::Max(Site->DefuseTime, 0.01f);
		if (ObjectiveProgress >= 1.f)
		{
			bIsDefusing = false;
			ObjectiveProgress = 1.f;
			GetWorldTimerManager().ClearTimer(ObjectiveTimer);
			GM->NotifyBombDefused(GetPlayerState<ANexusPlayerState>());
		}
	}
}

void ANexusCharacter::AbortObjective()
{
	bIsPlanting = false;
	bIsDefusing = false;
	ObjectiveProgress = 0.f;
	GetWorldTimerManager().ClearTimer(ObjectiveTimer);
}

void ANexusCharacter::EnableSpawnProtection()
{
	bSpawnProtected = true;
	const float Duration = GetDefault<UNexusGameplaySettings>()->SpawnProtectionSeconds;
	GetWorldTimerManager().SetTimer(SpawnProtectTimer, this, &ANexusCharacter::ClearSpawnProtection, Duration, false);
}

void ANexusCharacter::ClearSpawnProtection()
{
	bSpawnProtected = false;
}

void ANexusCharacter::SetCallout(const FText& InCallout)
{
	if (CurrentCallout.EqualTo(InCallout))
	{
		return;
	}
	CurrentCallout = InCallout;
	if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetController()))
	{
		PC->ShowCallout(InCallout);
	}
}

void ANexusCharacter::OnRep_ADS()
{
	if (!FirstPersonCamera || !IsLocallyControlled())
	{
		return;
	}
	float Fov = 90.f;
	if (bADS)
	{
		Fov = 70.f;
		if (const FNexusWeaponDefinition* W = CurrentWeapon())
		{
			if (W->Category == ENexusWeaponCategory::Sniper)
			{
				Fov = 52.f;
			}
		}
	}
	FirstPersonCamera->SetFieldOfView(Fov);
}

void ANexusCharacter::OnRep_Callout()
{
	if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetController()))
	{
		PC->ShowCallout(CurrentCallout);
	}
}

void ANexusCharacter::ReceiveNexusDamage(float Amount, AActor* DamageCauser, ENexusHitZone Zone, FName Weapon, bool bWallbang)
{
	if (!HasAuthority() || Health <= 0.f || bSpawnProtected)
	{
		return;
	}
	if (ANexusGameMode* GM = GetWorld()->GetAuthGameMode<ANexusGameMode>())
	{
		if (!GM->CanDealDamage(DamageCauser, this))
		{
			return;
		}
	}

	float Incoming = Amount;
	if (ANexusPlayerState* PS = GetNexusPS())
	{
		const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
		const FNexusArmorDefinition Armor = S->GetArmorDef(PS->ArmorType);
		if (PS->ArmorValue > 0.f && Zone != ENexusHitZone::Head)
		{
			const float Absorb = Incoming * Armor.ArmorProtection;
			PS->ArmorValue = FMath::Max(0.f, PS->ArmorValue - Absorb);
			Incoming = (Incoming - Absorb) * Armor.ArmorDamageMultiplier;
		}
	}

	Health = FMath::Max(0.f, Health - Incoming);
	if (ANexusCharacter* CauserChar = Cast<ANexusCharacter>(DamageCauser))
	{
		if (ANexusPlayerState* CPS = CauserChar->GetNexusPS())
		{
			CPS->DamageDealt += FMath::RoundToInt(Incoming);
		}
	}
	if ((bIsPlanting || bIsDefusing) && Incoming >= GetDefault<UNexusGameplaySettings>()->PlantInterruptDamage)
	{
		AbortObjective();
	}
	if (Health <= 0.f)
	{
		AbortObjective();
		if (ANexusGameMode* GM = GetWorld()->GetAuthGameMode<ANexusGameMode>())
		{
			GM->NotifyPlayerDied(this, DamageCauser, Weapon, Zone == ENexusHitZone::Head, bWallbang);
		}
		GetCharacterMovement()->DisableMovement();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetController()))
		{
			PC->EnterSpectator();
		}
	}
}

ENexusTeam ANexusCharacter::GetTeam() const
{
	if (const ANexusPlayerState* PS = GetPlayerState<ANexusPlayerState>())
	{
		return PS->Team;
	}
	return ENexusTeam::Spectator;
}

bool ANexusCharacter::HasBomb() const
{
	if (const ANexusPlayerState* PS = GetPlayerState<ANexusPlayerState>())
	{
		return PS->bHasBomb;
	}
	return false;
}

ANexusPlayerState* ANexusCharacter::GetNexusPS() const
{
	return GetPlayerState<ANexusPlayerState>();
}

bool ANexusCharacter::CanShoot() const
{
	if (!IsAlive() || bIsPlanting || bIsDefusing || bReloading || bSwitching)
	{
		return false;
	}
	const ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	if (!GS)
	{
		return false;
	}
	return GS->Phase == ENexusRoundPhase::Live || GS->Phase == ENexusRoundPhase::Planted || GS->Phase == ENexusRoundPhase::PostPlant;
}

const FNexusWeaponDefinition* ANexusCharacter::CurrentWeapon() const
{
	if (const ANexusPlayerState* PS = GetNexusPS())
	{
		return GetDefault<UNexusGameplaySettings>()->FindWeapon(PS->EquippedWeapon);
	}
	return nullptr;
}

void ANexusCharacter::ApplyMoveSpeedFromOperator()
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	float SpeedM = 5.5f;
	if (const ANexusPlayerState* PS = GetNexusPS())
	{
		if (const FNexusOperatorDefinition* Op = S->FindOperator(PS->OperatorId))
		{
			SpeedM = Op->MoveSpeedMeters;
		}
	}
	float Penalty = 0.f;
	if (const FNexusWeaponDefinition* W = CurrentWeapon())
	{
		Penalty = W->MovementPenalty;
		if (bADS) { Penalty += 0.12f; }
	}
	GetCharacterMovement()->MaxWalkSpeed = SpeedM * 100.f * (1.f - Penalty);
}

void ANexusCharacter::ServerReload_Implementation()
{
	ANexusPlayerState* PS = GetNexusPS();
	const FNexusWeaponDefinition* W = CurrentWeapon();
	if (!HasAuthority() || !PS || !W || bReloading || !IsAlive())
	{
		return;
	}
	if (PS->MagAmmo >= W->MagazineSize || PS->ReserveAmmo <= 0)
	{
		return;
	}
	bReloading = true;
	bWantsFire = false;
	GetWorldTimerManager().ClearTimer(FireTimer);
	const float Time = (PS->MagAmmo <= 0) ? W->EmptyReloadTime : W->ReloadTime;
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ANexusCharacter::FinishReload, Time, false);
}

void ANexusCharacter::FinishReload()
{
	bReloading = false;
	ANexusPlayerState* PS = GetNexusPS();
	const FNexusWeaponDefinition* W = CurrentWeapon();
	if (!PS || !W)
	{
		return;
	}
	const int32 Need = W->MagazineSize - PS->MagAmmo;
	const int32 Take = FMath::Min(Need, PS->ReserveAmmo);
	PS->MagAmmo += Take;
	PS->ReserveAmmo -= Take;
	RecoilShots = 0;
	UNexusAudio::PlayReload(this, GetActorLocation());
}

void ANexusCharacter::ServerSetADS_Implementation(bool bNewADS)
{
	const FNexusWeaponDefinition* W = CurrentWeapon();
	if (W && !W->bAllowADS && bNewADS)
	{
		return;
	}
	bADS = bNewADS;
	OnRep_ADS();
	ApplyMoveSpeedFromOperator();
}

void ANexusCharacter::ServerSwitchWeapon_Implementation(uint8 Slot)
{
	ANexusPlayerState* PS = GetNexusPS();
	if (!PS || bSwitching || !IsAlive())
	{
		return;
	}
	FName Next = PS->EquippedWeapon;
	if (Slot == 0 && !PS->PrimaryWeapon.IsNone()) { Next = PS->PrimaryWeapon; }
	else if (Slot == 1) { Next = PS->SecondaryWeapon; }
	else if (Slot == 2) { Next = TEXT("Knife"); }
	if (Next == PS->EquippedWeapon)
	{
		return;
	}
	bSwitching = true;
	bWantsFire = false;
	GetWorldTimerManager().ClearTimer(FireTimer);
	PS->EquippedWeapon = Next;
	if (const FNexusWeaponDefinition* W = GetDefault<UNexusGameplaySettings>()->FindWeapon(Next))
	{
		PS->MagAmmo = W->MagazineSize;
		if (PS->ReserveAmmo < W->MagazineSize) { PS->ReserveAmmo = W->ReserveAmmo; }
	}
	GetWorldTimerManager().SetTimer(SwitchTimer, this, &ANexusCharacter::FinishSwitch, GetDefault<UNexusGameplaySettings>()->WeaponSwitchTime, false);
}

void ANexusCharacter::FinishSwitch()
{
	bSwitching = false;
	ApplyMoveSpeedFromOperator();
}

void ANexusCharacter::DeployGadget(ENexusGadgetId Id)
{
	const FNexusGadgetDefinition* Def = GetDefault<UNexusGameplaySettings>()->FindGadget(Id);
	if (!Def)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FVector Loc = GetActorLocation() + GetActorForwardVector() * 80.f;
	if (ANexusGadgetActor* G = GetWorld()->SpawnActor<ANexusGadgetActor>(Loc, GetActorRotation(), Params))
	{
		G->Arm(Id, GetTeam(), Def->Duration, Def->RadiusMeters);
	}
}

void ANexusCharacter::ServerUseGadget_Implementation(uint8 Index)
{
	ANexusPlayerState* PS = GetNexusPS();
	const FNexusOperatorDefinition* Op = PS ? GetDefault<UNexusGameplaySettings>()->FindOperator(PS->OperatorId) : nullptr;
	if (!PS || !Op || !CanShoot())
	{
		return;
	}
	if (Index == 0 && PS->Gadget1Charges > 0)
	{
		PS->Gadget1Charges -= 1;
		DeployGadget(Op->Gadget1);
	}
	else if (Index == 1 && PS->Gadget2Charges > 0)
	{
		PS->Gadget2Charges -= 1;
		DeployGadget(Op->Gadget2);
	}
}

void ANexusCharacter::ServerUseUltimate_Implementation()
{
	ANexusPlayerState* PS = GetNexusPS();
	const FNexusOperatorDefinition* Op = PS ? GetDefault<UNexusGameplaySettings>()->FindOperator(PS->OperatorId) : nullptr;
	if (!PS || !Op || !CanShoot() || PS->UltimatePoints < Op->UltimateCost)
	{
		return;
	}
	PS->UltimatePoints -= Op->UltimateCost;
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ANexusGadgetActor* G = GetWorld()->SpawnActor<ANexusGadgetActor>(GetActorLocation(), GetActorRotation(), Params))
	{
		G->Arm(Op->Gadget1, GetTeam(), 12.f, 8.f);
		G->bMarking = true;
	}
}

void ANexusCharacter::ServerDropBomb_Implementation()
{
	ANexusPlayerState* PS = GetNexusPS();
	ANexusGameState* GS = GetWorld()->GetGameState<ANexusGameState>();
	if (!PS || !GS || !PS->bHasBomb || !HasAuthority())
	{
		return;
	}
	PS->bHasBomb = false;
	GS->bBombDropped = true;
	GS->DroppedBombLocation = GetActorLocation();
}

void ANexusCharacter::MulticastWeaponFired_Implementation()
{
	ViewKickPitch = FMath::Min(ViewKickPitch + 3.2f, 10.f);
	if (!IsLocallyControlled())
	{
		UNexusAudio::PlayFire(this, GetActorLocation());
	}
	if (!MuzzleFlashMesh)
	{
		return;
	}
	const FNexusWeaponDefinition* W = CurrentWeapon();
	if (W && W->Category == ENexusWeaponCategory::Melee)
	{
		return;
	}
	MuzzleFlashMesh->SetHiddenInGame(false);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(MuzzleTimer, this, &ANexusCharacter::HideMuzzle, 0.05f, false);
	}
}

void ANexusCharacter::HideMuzzle()
{
	if (MuzzleFlashMesh)
	{
		MuzzleFlashMesh->SetHiddenInGame(true);
	}
}

FVector ANexusCharacter::ViewmodelHipOffset() const
{
	return FVector(22.f, 10.f, -12.f);
}

FVector ANexusCharacter::ViewmodelADSOffset() const
{
	if (const FNexusWeaponDefinition* W = CurrentWeapon())
	{
		if (W->Category == ENexusWeaponCategory::Sniper)
		{
			return FVector(34.f, 0.4f, -3.2f);
		}
		if (W->Category == ENexusWeaponCategory::Pistol)
		{
			return FVector(26.f, 1.2f, -5.5f);
		}
	}
	return FVector(30.f, 1.6f, -5.f);
}

void ANexusCharacter::SetupViewmodel()
{
	UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (!Base)
	{
		return;
	}
	auto Tint = [Base](UStaticMeshComponent* Comp, const FLinearColor& Color)
	{
		if (!Comp)
		{
			return;
		}
		if (UMaterialInstanceDynamic* MID = Comp->CreateDynamicMaterialInstance(0, Base))
		{
			MID->SetVectorParameterValue(TEXT("Color"), Color);
		}
	};
	Tint(ViewWeaponMesh, FLinearColor(0.12f, 0.13f, 0.14f));
	Tint(ViewBarrelMesh, FLinearColor(0.08f, 0.08f, 0.09f));
	Tint(ViewMagMesh, FLinearColor(0.18f, 0.16f, 0.12f));
	Tint(LeftHandMesh, FLinearColor(0.72f, 0.55f, 0.42f));
	Tint(RightHandMesh, FLinearColor(0.72f, 0.55f, 0.42f));
	Tint(MuzzleFlashMesh, FLinearColor(1.f, 0.72f, 0.18f));
	Tint(WorldWeaponMesh, FLinearColor(0.12f, 0.13f, 0.14f));
	UNexusMaterialLibrary::ApplyToPrimitive(ViewWeaponMesh, ENexusArtSurface::Metal);
	UNexusMaterialLibrary::ApplyToPrimitive(ViewBarrelMesh, ENexusArtSurface::Metal);
	UNexusMaterialLibrary::ApplyToPrimitive(LeftHandMesh, ENexusArtSurface::Fabric);
	UNexusMaterialLibrary::ApplyToPrimitive(RightHandMesh, ENexusArtSurface::Fabric);
}

void ANexusCharacter::ApplyWeaponVisuals()
{
	const FNexusWeaponDefinition* W = CurrentWeapon();
	const FName Id = W ? W->ID : NAME_None;
	if (Id == ShownWeapon && ShownWeapon != NAME_None)
	{
		return;
	}
	ShownWeapon = Id;
	const ENexusWeaponCategory Cat = W ? W->Category : ENexusWeaponCategory::Pistol;

	FVector BodyScale(0.09f, 0.28f, 0.12f);
	FVector BodyLoc(0.f, 0.f, 0.f);
	FVector BarrelScale(0.035f, 0.035f, 0.42f);
	FVector BarrelLoc(22.f, 0.f, 2.f);
	FVector MagScale(0.05f, 0.04f, 0.14f);
	FVector MagLoc(-4.f, 0.f, -10.f);
	FVector WorldScale(0.08f, 0.22f, 0.1f);
	bool bShowMag = true;
	bool bShowBarrel = true;

	switch (Cat)
	{
	case ENexusWeaponCategory::Pistol:
		BodyScale = FVector(0.07f, 0.16f, 0.1f);
		BarrelScale = FVector(0.028f, 0.028f, 0.18f);
		BarrelLoc = FVector(12.f, 0.f, 1.f);
		MagScale = FVector(0.04f, 0.035f, 0.1f);
		WorldScale = FVector(0.06f, 0.12f, 0.08f);
		break;
	case ENexusWeaponCategory::SMG:
		BodyScale = FVector(0.08f, 0.22f, 0.11f);
		BarrelScale = FVector(0.03f, 0.03f, 0.28f);
		BarrelLoc = FVector(16.f, 0.f, 2.f);
		break;
	case ENexusWeaponCategory::Rifle:
		BodyScale = FVector(0.09f, 0.3f, 0.12f);
		BarrelScale = FVector(0.032f, 0.032f, 0.48f);
		BarrelLoc = FVector(24.f, 0.f, 2.f);
		break;
	case ENexusWeaponCategory::Sniper:
		BodyScale = FVector(0.08f, 0.34f, 0.1f);
		BarrelScale = FVector(0.028f, 0.028f, 0.72f);
		BarrelLoc = FVector(34.f, 0.f, 2.f);
		WorldScale = FVector(0.07f, 0.32f, 0.08f);
		break;
	case ENexusWeaponCategory::Shotgun:
		BodyScale = FVector(0.1f, 0.26f, 0.12f);
		BarrelScale = FVector(0.05f, 0.05f, 0.4f);
		BarrelLoc = FVector(20.f, 0.f, 2.f);
		MagScale = FVector(0.04f, 0.12f, 0.05f);
		MagLoc = FVector(-2.f, 0.f, -6.f);
		break;
	case ENexusWeaponCategory::LMG:
		BodyScale = FVector(0.12f, 0.34f, 0.14f);
		BarrelScale = FVector(0.04f, 0.04f, 0.5f);
		BarrelLoc = FVector(26.f, 0.f, 3.f);
		MagScale = FVector(0.1f, 0.08f, 0.16f);
		MagLoc = FVector(-2.f, 0.f, -12.f);
		WorldScale = FVector(0.1f, 0.28f, 0.12f);
		break;
	case ENexusWeaponCategory::Melee:
		BodyScale = FVector(0.04f, 0.04f, 0.08f);
		BarrelScale = FVector(0.02f, 0.05f, 0.32f);
		BarrelLoc = FVector(10.f, 0.f, 0.f);
		bShowMag = false;
		WorldScale = FVector(0.03f, 0.04f, 0.22f);
		break;
	default:
		break;
	}

	if (ViewWeaponMesh)
	{
		ViewWeaponMesh->SetRelativeLocation(BodyLoc);
		ViewWeaponMesh->SetRelativeScale3D(BodyScale);
	}
	if (ViewBarrelMesh)
	{
		ViewBarrelMesh->SetRelativeLocation(BarrelLoc);
		ViewBarrelMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		ViewBarrelMesh->SetRelativeScale3D(BarrelScale);
		ViewBarrelMesh->SetHiddenInGame(!bShowBarrel);
	}
	if (ViewMagMesh)
	{
		ViewMagMesh->SetRelativeLocation(MagLoc);
		ViewMagMesh->SetRelativeScale3D(MagScale);
		ViewMagMesh->SetHiddenInGame(!bShowMag);
	}
	if (LeftHandMesh)
	{
		LeftHandMesh->SetRelativeLocation(FVector(-6.f, 7.f, -4.f));
		LeftHandMesh->SetRelativeScale3D(FVector(0.06f, 0.045f, 0.1f));
	}
	if (RightHandMesh)
	{
		RightHandMesh->SetRelativeLocation(FVector(-2.f, -8.f, -3.f));
		RightHandMesh->SetRelativeScale3D(FVector(0.06f, 0.045f, 0.1f));
	}
	if (MuzzleFlashMesh)
	{
		MuzzleFlashMesh->SetRelativeLocation(BarrelLoc + FVector(BarrelScale.Z * 50.f * 0.5f, 0.f, 0.f));
		MuzzleFlashMesh->SetRelativeScale3D(FVector(0.08f, 0.08f, 0.08f));
		MuzzleFlashMesh->SetHiddenInGame(true);
	}
	if (WorldWeaponMesh)
	{
		WorldWeaponMesh->SetRelativeScale3D(WorldScale);
	}
}

void ANexusCharacter::UpdateViewmodel(float DeltaSeconds)
{
	ApplyWeaponVisuals();
	if (!ViewmodelRoot)
	{
		return;
	}

	ViewSwayTime += DeltaSeconds;
	const UNexusArtSettings* Art = GetDefault<UNexusArtSettings>();
	const float BobScale = (Art && Art->bEnableHeadBob) ? Art->HeadBobScale : 0.f;
	const float SwayScale = (Art && Art->bEnableWeaponSway) ? Art->WeaponSwayScale : 0.f;
	const float KickScale = Art ? Art->RecoilCameraScale : 1.f;
	const FVector Vel = GetVelocity();
	const float Speed = Vel.Size2D();
	const float Bob = (Speed > 20.f) ? FMath::Sin(ViewSwayTime * 10.f) * 1.1f * BobScale : 0.f;
	const float BobZ = (Speed > 20.f) ? FMath::Abs(FMath::Cos(ViewSwayTime * 10.f)) * 0.8f * BobScale : 0.f;

	FVector Target = ViewmodelHipOffset();
	if (bADS)
	{
		Target = ViewmodelADSOffset();
	}
	if (bReloading)
	{
		Target += FVector(-4.f, 6.f, -8.f + FMath::Sin(ViewSwayTime * 12.f) * 2.f);
	}
	if (bSwitching)
	{
		Target += FVector(0.f, 0.f, -16.f);
	}
	Target += FVector(0.f, Bob * 0.4f * SwayScale, BobZ - ViewKickPitch * 0.35f * KickScale);

	const FVector Cur = ViewmodelRoot->GetRelativeLocation();
	ViewmodelRoot->SetRelativeLocation(FMath::VInterpTo(Cur, Target, DeltaSeconds, bADS ? 16.f : 12.f));

	FRotator Rot = FRotator(-ViewKickPitch * 1.8f, bADS ? 0.f : 2.5f, bReloading ? 18.f : 0.f);
	ViewmodelRoot->SetRelativeRotation(FMath::RInterpTo(ViewmodelRoot->GetRelativeRotation(), Rot, DeltaSeconds, 14.f));

	ViewKickPitch = FMath::FInterpTo(ViewKickPitch, 0.f, DeltaSeconds, 10.f);
}

void ANexusCharacter::ApplyOperatorLook()
{
	const ANexusPlayerState* PS = GetNexusPS();
	const ENexusOperatorId Op = PS ? PS->OperatorId : ENexusOperatorId::None;
	if (Op == ShownOperator)
	{
		return;
	}
	ShownOperator = Op;
	FLinearColor Accent(0.22f, 0.24f, 0.22f);
	FVector HelmetScale(0.42f, 0.38f, 0.22f);
	FVector PackScale(0.22f, 0.32f, 0.4f);
	switch (Op)
	{
	case ENexusOperatorId::Aze: Accent = FLinearColor(0.18f, 0.32f, 0.28f); HelmetScale = FVector(0.4f, 0.36f, 0.18f); break;
	case ENexusOperatorId::Brutus: Accent = FLinearColor(0.28f, 0.16f, 0.12f); HelmetScale = FVector(0.48f, 0.44f, 0.28f); PackScale = FVector(0.3f, 0.4f, 0.5f); break;
	case ENexusOperatorId::Nyx: Accent = FLinearColor(0.12f, 0.12f, 0.16f); HelmetScale = FVector(0.38f, 0.4f, 0.16f); break;
	case ENexusOperatorId::Vanta: Accent = FLinearColor(0.1f, 0.12f, 0.14f); break;
	case ENexusOperatorId::Titan: Accent = FLinearColor(0.32f, 0.30f, 0.22f); HelmetScale = FVector(0.5f, 0.46f, 0.3f); PackScale = FVector(0.34f, 0.42f, 0.55f); break;
	case ENexusOperatorId::Warden: Accent = FLinearColor(0.22f, 0.24f, 0.18f); PackScale = FVector(0.18f, 0.28f, 0.36f); break;
	case ENexusOperatorId::Kraken: Accent = FLinearColor(0.14f, 0.2f, 0.24f); break;
	case ENexusOperatorId::Echo: Accent = FLinearColor(0.2f, 0.22f, 0.28f); HelmetScale = FVector(0.44f, 0.5f, 0.2f); break;
	case ENexusOperatorId::Bulwark: Accent = FLinearColor(0.26f, 0.26f, 0.22f); PackScale = FVector(0.36f, 0.45f, 0.58f); break;
	case ENexusOperatorId::Hawk: Accent = FLinearColor(0.24f, 0.22f, 0.16f); HelmetScale = FVector(0.36f, 0.4f, 0.14f); break;
	default: break;
	}
	if (HelmetMesh)
	{
		HelmetMesh->SetRelativeScale3D(HelmetScale);
		UNexusMaterialLibrary::ApplyToPrimitive(HelmetMesh, ENexusArtSurface::Metal);
	}
	if (VestMesh)
	{
		UNexusMaterialLibrary::ApplyToPrimitive(VestMesh, ENexusArtSurface::Fabric);
	}
	if (PackMesh)
	{
		PackMesh->SetRelativeScale3D(PackScale);
		UNexusMaterialLibrary::ApplyToPrimitive(PackMesh, ENexusArtSurface::DirtyConcrete);
	}
	if (UMaterialInstanceDynamic* BodyMID = BodyMesh ? Cast<UMaterialInstanceDynamic>(BodyMesh->GetMaterial(0)) : nullptr)
	{
		BodyMID->SetVectorParameterValue(TEXT("Color"), Accent * 0.5f + FLinearColor(0.2f, 0.22f, 0.2f));
	}
}
