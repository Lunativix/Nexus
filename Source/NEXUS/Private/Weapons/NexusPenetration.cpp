#include "Weapons/NexusPenetration.h"
#include "NexusGameplaySettings.h"

FLinearColor UNexusPenetrationStatics::GetMaterialColor(ENexusMaterialType Type)
{
	switch (Type)
	{
	case ENexusMaterialType::Wood: return FLinearColor(0.45f, 0.28f, 0.12f);
	case ENexusMaterialType::Plaster: return FLinearColor(0.82f, 0.80f, 0.72f);
	case ENexusMaterialType::LightStone: return FLinearColor(0.62f, 0.60f, 0.52f);
	case ENexusMaterialType::ThickStone: return FLinearColor(0.38f, 0.36f, 0.32f);
	case ENexusMaterialType::Concrete: return FLinearColor(0.48f, 0.50f, 0.52f);
	case ENexusMaterialType::Structural: return FLinearColor(0.22f, 0.22f, 0.24f);
	case ENexusMaterialType::Glass: return FLinearColor(0.55f, 0.75f, 0.85f);
	default: return FLinearColor::Gray;
	}
}

float UNexusPenetrationStatics::GetDamageReduction(ENexusMaterialType Type, float ThicknessMeters)
{
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	const FNexusPenetrationProfile& Profile = Settings->GetProfile(Type);
	const float Reduction = Profile.DamageReduction + Profile.ThicknessReductionPerMeter * FMath::Max(ThicknessMeters, 0.f);
	return FMath::Clamp(Reduction, 0.f, 1.f);
}

float UNexusPenetrationStatics::ApplySurfaceToDamage(AActor* /*HitActor*/, float IncomingDamage, float ThicknessMeters, ENexusMaterialType Type)
{
	const UNexusGameplaySettings* Settings = GetDefault<UNexusGameplaySettings>();
	const FNexusPenetrationProfile& Profile = Settings->GetProfile(Type);
	const float Remaining = IncomingDamage * (1.f - GetDamageReduction(Type, ThicknessMeters));
	if (Remaining < IncomingDamage * Profile.StopThreshold)
	{
		return 0.f;
	}
	return Remaining;
}
