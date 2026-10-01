#include "NexusGameplaySettings.h"
#include <initializer_list>

static FNexusWeaponDefinition Wpn(FName ID, const TCHAR* Name, ENexusWeaponCategory Cat, ENexusWeaponSlot Slot,
	int32 Price, int32 Mag, int32 Reserve, float RPM,
	float Head, float Torso, float Arms, float Legs, float Pen = 1.f, int32 Pellets = 1)
{
	FNexusWeaponDefinition D;
	D.ID = ID;
	D.DisplayName = FText::FromString(Name);
	D.Category = Cat;
	D.Slot = Slot;
	D.Price = Price;
	D.MagazineSize = Mag;
	D.ReserveAmmo = Reserve;
	D.FireRateRPM = RPM;
	D.DamageHead = Head;
	D.DamageTorso = Torso;
	D.DamageArms = Arms;
	D.DamageLegs = Legs;
	D.Penetration = Pen;
	D.PelletCount = Pellets;
	D.bRequiresADS = (Cat == ENexusWeaponCategory::Sniper);
	D.bAllowADS = (Cat != ENexusWeaponCategory::Melee);
	if (Cat == ENexusWeaponCategory::Sniper)
	{
		D.FirstShotAccuracy = 0.0f;
		D.ADSSpread = 0.01f;
		D.HipFireSpread = 2.5f;
	}
	if (Cat == ENexusWeaponCategory::Shotgun)
	{
		D.FireRateRPM = FMath::Max(RPM, 80.f);
		D.HipFireSpread = 3.5f;
	}
	return D;
}

static FNexusGadgetDefinition Gad(ENexusGadgetId ID, const TCHAR* Name, int32 Charges, float Dur, float RadiusM, float RangeM, float Place, const TCHAR* Counter)
{
	FNexusGadgetDefinition G;
	G.ID = ID;
	G.DisplayName = FText::FromString(Name);
	G.Charges = Charges;
	G.Duration = Dur;
	G.RadiusMeters = RadiusM;
	G.RangeMeters = RangeM;
	G.PlaceTime = Place;
	G.CounterHint = Counter;
	return G;
}

static FNexusOperatorDefinition Op(ENexusOperatorId ID, const TCHAR* Name, ENexusTeam Team, ENexusWeightClass W,
	std::initializer_list<FName> Prim, std::initializer_list<FName> Sec,
	ENexusGadgetId G1, ENexusGadgetId G2, ENexusUltimateId Ult, int32 UltCost, float Speed)
{
	FNexusOperatorDefinition O;
	O.ID = ID;
	O.Name = FText::FromString(Name);
	O.RecommendedTeam = Team;
	O.WeightClass = W;
	O.PrimaryWeapons = TArray<FName>(Prim);
	O.SecondaryWeapons = TArray<FName>(Sec);
	O.Gadget1 = G1;
	O.Gadget2 = G2;
	O.Ultimate = Ult;
	O.UltimateCost = UltCost;
	O.MoveSpeedMeters = Speed;
	return O;
}

UNexusGameplaySettings::UNexusGameplaySettings()
{
	CategoryName = TEXT("NEXUS");

	Bomb.PlantTime = 4.f;
	Bomb.DefuseTime = 7.f;
	Bomb.BombTimer = 40.f;
	PlantTimeSeconds = Bomb.PlantTime;
	DefuseTimeSeconds = Bomb.DefuseTime;
	BombTimerSeconds = Bomb.BombTimer;
	RoundTimeSeconds = Match.RoundTime;
	FreezeTimeSeconds = Match.FreezeTime;
	bUseFreezeTime = true;

	LightArmor = { ENexusArmorType::Light, 500, 50.f, 0.35f, 0.82f };
	HeavyArmor = { ENexusArmorType::Heavy, 650, 100.f, 0.55f, 0.68f };
	LightArmorPrice = 500;
	HeavyArmorPrice = 650;

	PenetrationTable =
	{
		{ ENexusMaterialType::Wood, 0.15f, 0.20f, 0.05f },
		{ ENexusMaterialType::Plaster, 0.25f, 0.25f, 0.05f },
		{ ENexusMaterialType::LightStone, 0.58f, 0.40f, 0.08f },
		{ ENexusMaterialType::ThickStone, 0.70f, 0.50f, 0.10f },
		{ ENexusMaterialType::Concrete, 0.68f, 0.45f, 0.10f },
		{ ENexusMaterialType::Structural, 1.00f, 1.00f, 1.00f },
		{ ENexusMaterialType::Glass, 0.03f, 0.05f, 0.02f }
	};

	Weapons =
	{
		Wpn(TEXT("USP"), TEXT("USP-S"), ENexusWeaponCategory::Pistol, ENexusWeaponSlot::Secondary, 0, 12, 24, 352.f, 90, 34, 28, 22, 0.7f),
		Wpn(TEXT("Glock"), TEXT("Glock 19"), ENexusWeaponCategory::Pistol, ENexusWeaponSlot::Secondary, 500, 15, 45, 450.f, 82, 30, 25, 21, 0.65f),
		Wpn(TEXT("Deagle"), TEXT("Desert Eagle"), ENexusWeaponCategory::Pistol, ENexusWeaponSlot::Secondary, 800, 7, 21, 240.f, 160, 58, 48, 42, 0.9f),
		Wpn(TEXT("UMP"), TEXT("UMP-45"), ENexusWeaponCategory::SMG, ENexusWeaponSlot::Primary, 1200, 25, 75, 600.f, 100, 40, 32, 28, 0.75f),
		Wpn(TEXT("MP5K"), TEXT("MP5K"), ENexusWeaponCategory::SMG, ENexusWeaponSlot::Primary, 1400, 30, 90, 800.f, 90, 32, 26, 23, 0.7f),
		Wpn(TEXT("P90"), TEXT("P90"), ENexusWeaponCategory::SMG, ENexusWeaponSlot::Primary, 1600, 50, 100, 950.f, 78, 25, 21, 19, 0.65f),
		Wpn(TEXT("FAMAS"), TEXT("FAMAS"), ENexusWeaponCategory::Rifle, ENexusWeaponSlot::Primary, 2300, 25, 75, 900.f, 105, 31, 26, 23, 0.85f),
		Wpn(TEXT("AK47"), TEXT("AK-47"), ENexusWeaponCategory::Rifle, ENexusWeaponSlot::Primary, 2700, 30, 90, 600.f, 125, 40, 33, 30, 1.0f),
		Wpn(TEXT("M4A4"), TEXT("M4A4"), ENexusWeaponCategory::Rifle, ENexusWeaponSlot::Primary, 2900, 30, 90, 800.f, 110, 33, 27, 24, 0.95f),
		Wpn(TEXT("SCARH"), TEXT("SCAR-H"), ENexusWeaponCategory::Rifle, ENexusWeaponSlot::Primary, 3200, 20, 60, 600.f, 135, 46, 37, 33, 1.05f),
		Wpn(TEXT("DB"), TEXT("Double Barrel"), ENexusWeaponCategory::Shotgun, ENexusWeaponSlot::Primary, 900, 2, 16, 120.f, 28, 28, 28, 28, 0.4f, 12),
		Wpn(TEXT("XM"), TEXT("XM"), ENexusWeaponCategory::Shotgun, ENexusWeaponSlot::Primary, 1300, 8, 32, 180.f, 23, 23, 23, 23, 0.45f, 12),
		Wpn(TEXT("L96A1"), TEXT("L96A1"), ENexusWeaponCategory::Sniper, ENexusWeaponSlot::Primary, 3000, 5, 15, 50.f, 250, 150, 95, 85, 1.2f),
		Wpn(TEXT("SVU"), TEXT("SVU-AS"), ENexusWeaponCategory::Sniper, ENexusWeaponSlot::Primary, 2600, 10, 20, 150.f, 180, 85, 65, 55, 1.1f),
		Wpn(TEXT("M60"), TEXT("M60"), ENexusWeaponCategory::LMG, ENexusWeaponSlot::Primary, 3000, 100, 200, 650.f, 105, 42, 34, 30, 1.15f),
		Wpn(TEXT("AWP"), TEXT("AWP"), ENexusWeaponCategory::Sniper, ENexusWeaponSlot::Primary, 4750, 5, 10, 41.f, 250, 150, 95, 85, 1.25f),
		Wpn(TEXT("Knife"), TEXT("Knife"), ENexusWeaponCategory::Melee, ENexusWeaponSlot::Melee, 0, 1, 0, 120.f, 55, 40, 30, 25, 0.f)
	};

	Gadgets =
	{
		Gad(ENexusGadgetId::Drone, TEXT("Drone"), 2, 18.f, 2.f, 35.f, 0.f, TEXT("Destroy / jammer / EMP")),
		Gad(ENexusGadgetId::OpticalBeacon, TEXT("Optical Beacon"), 2, 25.f, 12.f, 12.f, 0.5f, TEXT("Destroy / jammer")),
		Gad(ENexusGadgetId::BreachCharge, TEXT("Breach Charge"), 2, 1.5f, 2.5f, 5.f, 2.f, TEXT("Reinforcement / rotate")),
		Gad(ENexusGadgetId::PenetratingCharge, TEXT("Penetrating Charge"), 1, 3.f, 3.5f, 6.f, 3.f, TEXT("Reinforcement")),
		Gad(ENexusGadgetId::PersonalJammer, TEXT("Personal Jammer"), 2, 10.f, 4.f, 4.f, 0.f, TEXT("Wait duration")),
		Gad(ENexusGadgetId::SilentCharge, TEXT("Silent Charge"), 2, 1.5f, 1.f, 3.f, 1.5f, TEXT("Watch angles")),
		Gad(ENexusGadgetId::Smoke, TEXT("Smoke"), 2, 18.f, 6.f, 20.f, 0.4f, TEXT("Reposition / thermal / wait")),
		Gad(ENexusGadgetId::Incendiary, TEXT("Incendiary"), 1, 8.f, 4.f, 18.f, 0.4f, TEXT("Avoid area")),
		Gad(ENexusGadgetId::BallisticCover, TEXT("Ballistic Cover"), 1, 20.f, 1.2f, 4.f, 1.f, TEXT("Flank / destroy")),
		Gad(ENexusGadgetId::APAmmo, TEXT("AP Ammo"), 2, 12.f, 0.f, 0.f, 0.f, TEXT("Wait duration")),
		Gad(ENexusGadgetId::Camera, TEXT("Camera"), 2, 40.f, 1.f, 40.f, 1.f, TEXT("Destroy / EMP / jammer")),
		Gad(ENexusGadgetId::MotionSensor, TEXT("Motion Sensor"), 2, 30.f, 8.f, 8.f, 0.8f, TEXT("Slow / crouch / jammer")),
		Gad(ENexusGadgetId::ElectricCharge, TEXT("Electric Charge"), 2, 8.f, 3.f, 6.f, 0.5f, TEXT("Destroy / wait")),
		Gad(ENexusGadgetId::RapidReinforcement, TEXT("Rapid Reinforcement"), 2, 4.f, 2.f, 4.f, 4.f, TEXT("Rotate / breach elsewhere")),
		Gad(ENexusGadgetId::Jammer, TEXT("Jammer"), 2, 20.f, 10.f, 10.f, 0.6f, TEXT("Destroy / EMP")),
		Gad(ENexusGadgetId::EMPMine, TEXT("EMP Mine"), 2, 6.f, 5.f, 8.f, 0.8f, TEXT("Shoot / trigger from range")),
		Gad(ENexusGadgetId::BallisticBarrier, TEXT("Ballistic Barrier"), 1, 25.f, 1.5f, 4.f, 1.2f, TEXT("Flank / destroy")),
		Gad(ENexusGadgetId::Reinforcement, TEXT("Reinforcement"), 2, 4.f, 2.f, 4.f, 4.f, TEXT("Rotate")),
		Gad(ENexusGadgetId::OpticalDetector, TEXT("Optical Detector"), 2, 8.f, 15.f, 15.f, 0.f, TEXT("Slow / crouch / jammer")),
		Gad(ENexusGadgetId::ThermalScope, TEXT("Thermal Scope"), 1, 10.f, 0.f, 40.f, 0.f, TEXT("Smoke wait / reposition"))
	};

	const float Light = 5.7f, Std = 5.5f, Hvy = 5.2f;
	Operators =
	{
		Op(ENexusOperatorId::Aze, TEXT("AZE"), ENexusTeam::Attackers, ENexusWeightClass::Light, {TEXT("FAMAS"), TEXT("UMP")}, {TEXT("USP"), TEXT("Glock")}, ENexusGadgetId::Drone, ENexusGadgetId::OpticalBeacon, ENexusUltimateId::BlackEye, 7, Light),
		Op(ENexusOperatorId::Brutus, TEXT("BRUTUS"), ENexusTeam::Attackers, ENexusWeightClass::Standard, {TEXT("AK47"), TEXT("MP5K")}, {TEXT("Glock"), TEXT("Deagle")}, ENexusGadgetId::BreachCharge, ENexusGadgetId::PenetratingCharge, ENexusUltimateId::BreachHammer, 7, Std),
		Op(ENexusOperatorId::Nyx, TEXT("NYX"), ENexusTeam::Attackers, ENexusWeightClass::Light, {TEXT("MP5K"), TEXT("P90")}, {TEXT("USP"), TEXT("Glock")}, ENexusGadgetId::PersonalJammer, ENexusGadgetId::SilentCharge, ENexusUltimateId::GhostWalk, 8, Light),
		Op(ENexusOperatorId::Vanta, TEXT("VANTA"), ENexusTeam::Attackers, ENexusWeightClass::Standard, {TEXT("M4A4"), TEXT("UMP")}, {TEXT("USP"), TEXT("Deagle")}, ENexusGadgetId::Smoke, ENexusGadgetId::Incendiary, ENexusUltimateId::Screen, 7, Std),
		Op(ENexusOperatorId::Titan, TEXT("TITAN"), ENexusTeam::Attackers, ENexusWeightClass::Heavy, {TEXT("M60"), TEXT("SCARH")}, {TEXT("Deagle"), TEXT("Glock")}, ENexusGadgetId::BallisticCover, ENexusGadgetId::APAmmo, ENexusUltimateId::APLoad, 8, Hvy),
		Op(ENexusOperatorId::Warden, TEXT("WARDEN"), ENexusTeam::Defenders, ENexusWeightClass::Standard, {TEXT("M4A4"), TEXT("UMP")}, {TEXT("USP"), TEXT("Deagle")}, ENexusGadgetId::Camera, ENexusGadgetId::MotionSensor, ENexusUltimateId::ControlRoom, 7, Std),
		Op(ENexusOperatorId::Kraken, TEXT("KRAKEN"), ENexusTeam::Defenders, ENexusWeightClass::Standard, {TEXT("AK47"), TEXT("MP5K")}, {TEXT("Glock"), TEXT("USP")}, ENexusGadgetId::ElectricCharge, ENexusGadgetId::RapidReinforcement, ENexusUltimateId::Lockdown, 8, Std),
		Op(ENexusOperatorId::Echo, TEXT("ECHO"), ENexusTeam::Defenders, ENexusWeightClass::Light, {TEXT("MP5K"), TEXT("FAMAS")}, {TEXT("USP"), TEXT("Glock")}, ENexusGadgetId::Jammer, ENexusGadgetId::EMPMine, ENexusUltimateId::Blackout, 7, Light),
		Op(ENexusOperatorId::Bulwark, TEXT("BULWARK"), ENexusTeam::Defenders, ENexusWeightClass::Heavy, {TEXT("SCARH"), TEXT("XM")}, {TEXT("Deagle"), TEXT("USP")}, ENexusGadgetId::BallisticBarrier, ENexusGadgetId::Reinforcement, ENexusUltimateId::Fortify, 8, Hvy),
		Op(ENexusOperatorId::Hawk, TEXT("HAWK"), ENexusTeam::Defenders, ENexusWeightClass::Light, {TEXT("SVU"), TEXT("M4A4")}, {TEXT("USP"), TEXT("Deagle")}, ENexusGadgetId::OpticalDetector, ENexusGadgetId::ThermalScope, ENexusUltimateId::Hunter, 8, Light)
	};
}

const FNexusPenetrationProfile& UNexusGameplaySettings::GetProfile(ENexusMaterialType Type) const
{
	for (const FNexusPenetrationProfile& Profile : PenetrationTable)
	{
		if (Profile.Material == Type)
		{
			return Profile;
		}
	}
	static FNexusPenetrationProfile Fallback;
	Fallback.Material = Type;
	Fallback.DamageReduction = 0.5f;
	return Fallback;
}

const FNexusWeaponDefinition* UNexusGameplaySettings::FindWeapon(FName ID) const
{
	for (const FNexusWeaponDefinition& W : Weapons)
	{
		if (W.ID == ID)
		{
			return &W;
		}
	}
	return nullptr;
}

const FNexusOperatorDefinition* UNexusGameplaySettings::FindOperator(ENexusOperatorId ID) const
{
	for (const FNexusOperatorDefinition& O : Operators)
	{
		if (O.ID == ID)
		{
			return &O;
		}
	}
	return nullptr;
}

const FNexusGadgetDefinition* UNexusGameplaySettings::FindGadget(ENexusGadgetId ID) const
{
	for (const FNexusGadgetDefinition& G : Gadgets)
	{
		if (G.ID == ID)
		{
			return &G;
		}
	}
	return nullptr;
}

FNexusArmorDefinition UNexusGameplaySettings::GetArmorDef(ENexusArmorType Type) const
{
	if (Type == ENexusArmorType::Light) { return LightArmor; }
	if (Type == ENexusArmorType::Heavy) { return HeavyArmor; }
	return FNexusArmorDefinition();
}
