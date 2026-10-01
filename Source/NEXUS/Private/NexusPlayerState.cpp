#include "NexusPlayerState.h"
#include "NexusGameplaySettings.h"
#include "Net/UnrealNetwork.h"

ANexusPlayerState::ANexusPlayerState()
{
	bReplicates = true;
}

void ANexusPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANexusPlayerState, Team);
	DOREPLIFETIME(ANexusPlayerState, Faction);
	DOREPLIFETIME(ANexusPlayerState, bHasBomb);
	DOREPLIFETIME(ANexusPlayerState, bIsBotPlayer);
	DOREPLIFETIME(ANexusPlayerState, Credits);
	DOREPLIFETIME(ANexusPlayerState, OperatorId);
	DOREPLIFETIME(ANexusPlayerState, ArmorType);
	DOREPLIFETIME(ANexusPlayerState, ArmorValue);
	DOREPLIFETIME(ANexusPlayerState, PrimaryWeapon);
	DOREPLIFETIME(ANexusPlayerState, SecondaryWeapon);
	DOREPLIFETIME(ANexusPlayerState, EquippedWeapon);
	DOREPLIFETIME(ANexusPlayerState, MagAmmo);
	DOREPLIFETIME(ANexusPlayerState, ReserveAmmo);
	DOREPLIFETIME(ANexusPlayerState, Kills);
	DOREPLIFETIME(ANexusPlayerState, Deaths);
	DOREPLIFETIME(ANexusPlayerState, Assists);
	DOREPLIFETIME(ANexusPlayerState, UltimatePoints);
	DOREPLIFETIME(ANexusPlayerState, Gadget1Charges);
	DOREPLIFETIME(ANexusPlayerState, Gadget2Charges);
	DOREPLIFETIME(ANexusPlayerState, bDiedThisRound);
	DOREPLIFETIME(ANexusPlayerState, bBoughtThisRound);
	DOREPLIFETIME(ANexusPlayerState, DamageDealt);
	DOREPLIFETIME(ANexusPlayerState, Headshots);
	DOREPLIFETIME(ANexusPlayerState, Plants);
	DOREPLIFETIME(ANexusPlayerState, Defuses);
}

void ANexusPlayerState::ServerAddCredits(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}
	const int32 Cap = GetDefault<UNexusGameplaySettings>()->Economy.MaximumCredits;
	Credits = FMath::Clamp(Credits + Amount, 0, Cap);
}

bool ANexusPlayerState::ServerTrySpend(int32 Amount)
{
	if (!HasAuthority() || Amount < 0 || Credits < Amount)
	{
		return false;
	}
	Credits -= Amount;
	return true;
}

void ANexusPlayerState::ApplyOperatorDefaults()
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	const FNexusOperatorDefinition* Op = S->FindOperator(OperatorId);
	if (!Op)
	{
		OperatorId = (Team == ENexusTeam::Defenders) ? ENexusOperatorId::Warden : ENexusOperatorId::Aze;
		Op = S->FindOperator(OperatorId);
	}
	if (!Op)
	{
		return;
	}
	SecondaryWeapon = Op->SecondaryWeapons.Num() > 0 ? Op->SecondaryWeapons[0] : FName(TEXT("USP"));
	if (const FNexusGadgetDefinition* G1 = S->FindGadget(Op->Gadget1)) { Gadget1Charges = G1->Charges; }
	if (const FNexusGadgetDefinition* G2 = S->FindGadget(Op->Gadget2)) { Gadget2Charges = G2->Charges; }
}

void ANexusPlayerState::GrantLoadoutForNewRound(bool bKeepPurchases)
{
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();
	ApplyOperatorDefaults();
	if (!bKeepPurchases)
	{
		PrimaryWeapon = NAME_None;
		ArmorType = ENexusArmorType::None;
		ArmorValue = 0.f;
		EquippedWeapon = SecondaryWeapon;
	}
	else if (!PrimaryWeapon.IsNone())
	{
		EquippedWeapon = PrimaryWeapon;
	}
	else
	{
		EquippedWeapon = SecondaryWeapon;
	}
	if (const FNexusWeaponDefinition* W = S->FindWeapon(EquippedWeapon))
	{
		MagAmmo = W->MagazineSize;
		ReserveAmmo = W->ReserveAmmo;
	}
	bDiedThisRound = false;
	bBoughtThisRound = false;
	const FNexusOperatorDefinition* Op = S->FindOperator(OperatorId);
	if (Op)
	{
		if (const FNexusGadgetDefinition* G1 = S->FindGadget(Op->Gadget1)) { Gadget1Charges = G1->Charges; }
		if (const FNexusGadgetDefinition* G2 = S->FindGadget(Op->Gadget2)) { Gadget2Charges = G2->Charges; }
	}
}
