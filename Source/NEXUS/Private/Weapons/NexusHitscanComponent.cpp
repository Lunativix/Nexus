#include "Weapons/NexusHitscanComponent.h"
#include "Weapons/NexusPenetration.h"
#include "NexusGameplaySettings.h"
#include "NexusCombatTypes.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "Interaction/NexusDoor.h"
#include "NexusCharacter.h"
#include "NexusGameMode.h"
#include "NexusPlayerController.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "NEXUS.h"

UNexusHitscanComponent::UNexusHitscanComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UNexusHitscanComponent::Fire(const FVector& Start, const FVector& Direction, AActor* InstigatorActor)
{
	FNexusWeaponDefinition Fallback;
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	Fallback.DamageHead = S->TestRifleHeadDamage;
	Fallback.DamageTorso = S->TestRifleBodyDamage;
	Fallback.DamageArms = S->TestRifleBodyDamage * 0.85f;
	Fallback.DamageLegs = S->TestRifleBodyDamage * 0.7f;
	Fallback.Penetration = S->TestRiflePenetrationPower;
	Fallback.MinDamageMultiplier = 0.7f;
	Fallback.MaxDamageRange = 1500.f;
	Fallback.MinDamageRange = 4000.f;
	FireWithWeapon(Start, Direction, InstigatorActor, Fallback, 0.f);
}

FNexusHitscanResult UNexusHitscanComponent::FireWithResult(const FVector& Start, const FVector& Direction, AActor* InstigatorActor)
{
	FNexusWeaponDefinition Fallback;
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	Fallback.DamageHead = S->TestRifleHeadDamage;
	Fallback.DamageTorso = S->TestRifleBodyDamage;
	Fallback.DamageArms = S->TestRifleBodyDamage * 0.85f;
	Fallback.DamageLegs = S->TestRifleBodyDamage * 0.7f;
	Fallback.Penetration = S->TestRiflePenetrationPower;
	return FireWithWeapon(Start, Direction, InstigatorActor, Fallback, 0.f);
}

static float ZoneDamage(const FNexusWeaponDefinition& W, ENexusHitZone Zone)
{
	switch (Zone)
	{
	case ENexusHitZone::Head: return W.DamageHead;
	case ENexusHitZone::LeftArm:
	case ENexusHitZone::RightArm: return W.DamageArms;
	case ENexusHitZone::LeftLeg:
	case ENexusHitZone::RightLeg: return W.DamageLegs;
	default: return W.DamageTorso;
	}
}

FNexusHitscanResult UNexusHitscanComponent::FireWithWeapon(const FVector& Start, const FVector& Direction, AActor* InstigatorActor, const FNexusWeaponDefinition& Weapon, float SpreadDeg)
{
	FNexusHitscanResult Result;
	UWorld* World = GetWorld();
	if (!World || !InstigatorActor || !InstigatorActor->HasAuthority())
	{
		return Result;
	}

	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	FVector Dir = Direction.GetSafeNormal();
	if (SpreadDeg > 0.f)
	{
		Dir = FMath::VRandCone(Dir, FMath::DegreesToRadians(SpreadDeg));
	}

	float Damage = Weapon.DamageTorso * FMath::Max(Weapon.Penetration, 0.1f);
	const float Initial = Damage;
	FVector TraceStart = Start;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NexusHitscan), true, InstigatorActor);

	float DepthAccumM = 0.f;
	const int32 MaxSurf = FMath::Max(1, Settings->MaxPenetrationSurfaces);
	const bool bDbg = Settings->bWeaponDebug;

	for (int32 Surf = 0; Surf < MaxSurf && Damage > 1.f; ++Surf)
	{
		const FVector TraceEnd = TraceStart + Dir * 50000.f;
		FHitResult Hit;
		if (!World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			break;
		}
		if (bDbg)
		{
			DrawDebugLine(World, TraceStart, Hit.ImpactPoint, FColor::Red, false, 1.5f, 0, 1.f);
		}

		AActor* HitActor = Hit.GetActor();
		if (ANexusCharacter* Enemy = Cast<ANexusCharacter>(HitActor))
		{
			if (ANexusGameMode* GM = World->GetAuthGameMode<ANexusGameMode>())
			{
				if (!GM->CanDealDamage(InstigatorActor, Enemy))
				{
					break;
				}
			}
			const FVector Local = Enemy->GetActorTransform().InverseTransformPosition(Hit.ImpactPoint);
			const ENexusHitZone Zone = NexusHitZoneFromLocal(Local);
			const float Dist = FVector::Dist(Start, Hit.ImpactPoint);
			float Falloff = 1.f;
			if (Dist > Weapon.MaxDamageRange)
			{
				const float T = FMath::Clamp((Dist - Weapon.MaxDamageRange) / FMath::Max(1.f, Weapon.MinDamageRange - Weapon.MaxDamageRange), 0.f, 1.f);
				Falloff = FMath::Lerp(1.f, Weapon.MinDamageMultiplier, T);
			}
			const float RemainFrac = (Initial > 0.f) ? (Damage / Initial) : 1.f;
			const float Applied = ZoneDamage(Weapon, Zone) * RemainFrac * Falloff;
			Enemy->ReceiveNexusDamage(Applied, InstigatorActor, Zone, Weapon.ID, Surf > 0);
			if (APawn* InstPawn = Cast<APawn>(InstigatorActor))
			{
				if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(InstPawn->GetController()))
				{
					PC->ClientHitFeedback(Zone == ENexusHitZone::Head);
				}
			}
			Result.DamageDealt = Applied;
			Result.HitCharacter = Enemy;
			Result.SurfacesPenetrated = Surf;
			Result.bHeadshot = (Zone == ENexusHitZone::Head);
			Result.HitZone = Zone;
			Result.DistanceUu = Dist;
			if (bDbg || Settings->bDamageDebug || Settings->bHitDebug)
			{
				UE_LOG(LogNexusWeapon, Log, TEXT("HIT shooter=%s target=%s start=%s end=%s zone=%d dist=%.0f dmg=%.1f walls=%d"),
					*GetNameSafe(InstigatorActor), *Enemy->GetName(), *Start.ToCompactString(), *Hit.ImpactPoint.ToCompactString(),
					(int32)Zone, Dist, Applied, Surf);
			}
			if (Settings->bHitboxDebug)
			{
				DrawDebugCapsule(World, Enemy->GetActorLocation(), 88.f, 34.f, Enemy->GetActorQuat(), FColor::Green, false, 1.5f, 0, 1.f);
			}
			break;
		}

		ENexusMaterialType Mat = ENexusMaterialType::Concrete;
		float ThicknessM = 0.2f;

		if (ANexusDestructibleSurface* SurfActor = Cast<ANexusDestructibleSurface>(HitActor))
		{
			Mat = SurfActor->MaterialType;
			ThicknessM = FMath::Max(SurfActor->Thickness / 100.f, 0.05f);
			SurfActor->ApplyDamage(Damage);
			Params.AddIgnoredActor(HitActor);
		}
		else if (ANexusDoor* Door = Cast<ANexusDoor>(HitActor))
		{
			Mat = ENexusMaterialType::Wood;
			ThicknessM = 0.12f;
			Door->ApplyDamage(Damage);
			Params.AddIgnoredActor(HitActor);
		}
		else if (UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Hit.GetComponent()))
		{
			const FString Name = ISM->GetName();
			const int32 Tag = Name.Find(TEXT("ISM_"));
			if (Tag != INDEX_NONE)
			{
				const int32 Idx = FCString::Atoi(*Name.Mid(Tag + 4));
				Mat = static_cast<ENexusMaterialType>(FMath::Clamp(Idx, 0, 6));
			}
			FTransform InstXform;
			if (Hit.Item != INDEX_NONE && ISM->GetInstanceTransform(Hit.Item, InstXform, true))
			{
				const FVector Sc = InstXform.GetScale3D();
				ThicknessM = FMath::Max(0.05f, FMath::Min3(FMath::Abs(Sc.X), FMath::Abs(Sc.Y), FMath::Abs(Sc.Z)));
			}
		}

		if (Mat == ENexusMaterialType::Structural)
		{
			Result.bStoppedByStructural = true;
			Result.SurfacesPenetrated = Surf + 1;
			return Result;
		}

		DepthAccumM += ThicknessM;
		if (DepthAccumM > Settings->MaxPenetrationDepthMeters)
		{
			Result.SurfacesPenetrated = Surf + 1;
			return Result;
		}

		Damage = UNexusPenetrationStatics::ApplySurfaceToDamage(HitActor, Damage, ThicknessM, Mat);
		Damage *= (1.f - Settings->ExtraDamageDecayPerSurface);
		Result.SurfacesPenetrated = Surf + 1;
		if (Settings->bWallbangDebug)
		{
			UE_LOG(LogNexusWeapon, Log, TEXT("WALLBANG mat=%d thick=%.2fm remain=%.1f surf=%d actor=%s"),
				(int32)Mat, ThicknessM, Damage, Surf + 1, *GetNameSafe(HitActor));
		}
		if (Damage <= 0.f)
		{
			break;
		}
		TraceStart = Hit.ImpactPoint + Dir * FMath::Max(ThicknessM * 100.f + 4.f, 8.f);
	}

	return Result;
}
