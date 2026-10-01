#include "NexusHUD.h"
#include "NexusCharacter.h"
#include "NexusPlayerController.h"
#include "NexusPlayerState.h"
#include "NexusGameState.h"
#include "NexusGameplaySettings.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "Maps/NexusMapCatalog.h"

int32 GNexusHUDDrawCount = 0;

void ANexusHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas)
	{
		return;
	}
	DrawRect(FLinearColor(0.05f, 0.07f, 0.05f, 0.55f), 16.f, 10.f, 420.f, 58.f);
	const ENexusMapId MapId = FNexusMapCatalog::DetectFromWorld(GetWorld());
	DrawText(FString::Printf(TEXT("NEXUS  //  %s"), FNexusMapCatalog::MapName(MapId)), FLinearColor(0.72f, 0.78f, 0.58f), 24.f, 16.f, nullptr, 1.15f);

	const float CX = Canvas->SizeX * 0.5f;
	ANexusCharacter* Character = Cast<ANexusCharacter>(GetOwningPawn());
	ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetOwningPlayerController());
	ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	ANexusPlayerState* PS = PlayerOwner ? PlayerOwner->GetPlayerState<ANexusPlayerState>() : nullptr;
	const UNexusGameplaySettings* S = GetDefault<UNexusGameplaySettings>();

	DrawRect(FLinearColor::White, CX - 1.f, Canvas->SizeY * 0.5f - 6.f, 2.f, 12.f);
	DrawRect(FLinearColor::White, CX - 6.f, Canvas->SizeY * 0.5f - 1.f, 12.f, 2.f);
	if (PC && PC->HitMarkerTime > 0.f)
	{
		const FLinearColor Mk = PC->bHitMarkerHead ? FLinearColor(1.f, 0.25f, 0.2f) : FLinearColor(0.85f, 0.85f, 0.75f);
		DrawRect(Mk, CX - 10.f, Canvas->SizeY * 0.5f - 1.f, 6.f, 2.f);
		DrawRect(Mk, CX + 4.f, Canvas->SizeY * 0.5f - 1.f, 6.f, 2.f);
		DrawRect(Mk, CX - 1.f, Canvas->SizeY * 0.5f - 10.f, 2.f, 6.f);
		DrawRect(Mk, CX - 1.f, Canvas->SizeY * 0.5f + 4.f, 2.f, 6.f);
	}

	if (GS)
	{
		DrawText(FString::Printf(TEXT("VANGUARD %d   ROUND %d   SENTINEL %d"), GS->VanguardRounds, GS->RoundNumber, GS->SentinelRounds),
			FLinearColor::White, CX - 180.f, 16.f, nullptr, 1.35f);
		DrawText(FString::Printf(TEXT("%s  %.0f"), *UEnum::GetValueAsString(GS->Phase), GS->PhaseTimeRemaining),
			FLinearColor(0.9f, 0.9f, 0.4f), CX - 80.f, 40.f, nullptr, 1.15f);
		if (GS->bBombPlanted)
		{
			DrawText(FString::Printf(TEXT("BOMB %.0f  SITE %d"), GS->BombTimeRemaining, (int32)GS->PlantedSite), FLinearColor::Red, CX - 70.f, 62.f, nullptr, 1.2f);
		}
		float FeedY = 86.f;
		for (int32 i = GS->KillFeed.Num() - 1; i >= 0; --i)
		{
			const FNexusKillFeedEntry& E = GS->KillFeed[i];
			DrawText(FString::Printf(TEXT("%s [%s] %s%s%s"), *E.Killer, *E.Weapon.ToString(), *E.Victim,
				E.bHeadshot ? TEXT(" HS") : TEXT(""), E.bWallbang ? TEXT(" WB") : TEXT("")),
				FLinearColor(1.f, 0.7f, 0.3f), Canvas->SizeX - 420.f, FeedY, nullptr, 1.05f);
			FeedY += 18.f;
		}
	}

	const float Bottom = Canvas->SizeY - 120.f;
	DrawRect(FLinearColor(0.04f, 0.06f, 0.04f, 0.62f), 16.f, Bottom - 8.f, 280.f, 100.f);
	if (Character && Character->Health > 0.f && PS)
	{
		DrawText(FString::Printf(TEXT("HP %.0f  ARM %.0f"), Character->Health, PS->ArmorValue), FLinearColor(0.3f, 0.95f, 0.35f), 24.f, Bottom, nullptr, 1.4f);
		const FNexusWeaponDefinition* Wep = S->FindWeapon(PS->EquippedWeapon);
		const FString WepName = Wep ? Wep->DisplayName.ToString() : PS->EquippedWeapon.ToString();
		const FString Slot = Wep ? UEnum::GetValueAsString(Wep->Category) : TEXT("");
		DrawText(FString::Printf(TEXT("%s"), *WepName), FLinearColor::White, 24.f, Bottom + 22.f, nullptr, 1.35f);
		DrawText(FString::Printf(TEXT("%d  |  %d"), PS->MagAmmo, PS->ReserveAmmo),
			PS->MagAmmo <= 0 ? FLinearColor(1.f, 0.35f, 0.2f) : FLinearColor(0.95f, 0.95f, 0.85f),
			24.f, Bottom + 44.f, nullptr, 1.2f);
		if (Wep && Wep->MagazineSize > 0)
		{
			const float MagFrac = FMath::Clamp((float)PS->MagAmmo / (float)Wep->MagazineSize, 0.f, 1.f);
			DrawRect(FLinearColor(0.15f, 0.15f, 0.15f, 0.8f), 24.f, Bottom + 66.f, 160.f, 6.f);
			DrawRect(FLinearColor(0.85f, 0.75f, 0.2f), 24.f, Bottom + 66.f, 160.f * MagFrac, 6.f);
		}
		DrawText(FString::Printf(TEXT("$%d  %s%s"), PS->Credits, *Slot,
			Character->bADS ? TEXT("  ADS") : TEXT("")), FLinearColor(0.4f, 0.9f, 0.4f), 24.f, Bottom + 76.f, nullptr, 1.05f);

		const float GunX = Canvas->SizeX - 220.f;
		const float GunY = Canvas->SizeY - 70.f;
		FLinearColor GunCol(0.2f, 0.22f, 0.24f, 0.85f);
		if (Wep)
		{
			switch (Wep->Category)
			{
			case ENexusWeaponCategory::Sniper:
				DrawRect(GunCol, GunX, GunY + 10.f, 150.f, 8.f);
				DrawRect(GunCol, GunX + 20.f, GunY, 18.f, 28.f);
				break;
			case ENexusWeaponCategory::Shotgun:
				DrawRect(GunCol, GunX, GunY + 8.f, 120.f, 14.f);
				break;
			case ENexusWeaponCategory::Pistol:
				DrawRect(GunCol, GunX + 40.f, GunY + 10.f, 70.f, 10.f);
				DrawRect(GunCol, GunX + 48.f, GunY + 18.f, 10.f, 18.f);
				break;
			case ENexusWeaponCategory::Melee:
				DrawRect(GunCol, GunX + 70.f, GunY, 10.f, 40.f);
				break;
			default:
				DrawRect(GunCol, GunX, GunY + 10.f, 130.f, 10.f);
				DrawRect(GunCol, GunX + 24.f, GunY, 16.f, 22.f);
				break;
			}
		}
		const FNexusOperatorDefinition* Op = S->FindOperator(PS->OperatorId);
		const int32 Need = Op ? Op->UltimateCost : 7;
		DrawText(FString::Printf(TEXT("ULT %d/%d  G %d/%d  %s"), PS->UltimatePoints, Need, PS->Gadget1Charges, PS->Gadget2Charges,
			Op ? *Op->Name.ToString() : TEXT("")), FLinearColor(0.7f, 0.8f, 1.f), Canvas->SizeX - 360.f, Bottom, nullptr, 1.15f);
		if (Character->bIsPlanting)
		{
			DrawText(FString::Printf(TEXT("PLANTING %.0f%%"), Character->ObjectiveProgress * 100.f), FLinearColor::Red, CX - 80.f, Bottom, nullptr, 1.5f);
		}
		if (Character->bIsDefusing)
		{
			DrawText(FString::Printf(TEXT("DEFUSING %.0f%%"), Character->ObjectiveProgress * 100.f), FLinearColor::Blue, CX - 80.f, Bottom, nullptr, 1.5f);
		}
	}
	else if (PC && PC->bSpectating)
	{
		DrawText(TEXT("SPECTATING"), FLinearColor(0.8f, 0.8f, 0.8f), CX - 80.f, Bottom, nullptr, 1.2f);
		if (ANexusCharacter* ViewChar = Cast<ANexusCharacter>(PC->GetViewTarget()))
		{
			if (ANexusPlayerState* VPS = ViewChar->GetNexusPS())
			{
				DrawText(FString::Printf(TEXT("%s  HP %.0f  ARM %.0f  %s"), *VPS->GetPlayerName(), ViewChar->Health, VPS->ArmorValue, *VPS->EquippedWeapon.ToString()),
					FLinearColor(0.85f, 0.85f, 0.7f), CX - 140.f, Bottom + 22.f, nullptr, 1.1f);
			}
		}
	}

	if (GS && GS->Phase == ENexusRoundPhase::PostRound)
	{
		DrawText(FString::Printf(TEXT("ROUND %s  %s"), *UEnum::GetValueAsString(GS->LastRoundWinner), *UEnum::GetValueAsString(GS->LastRoundEndReason)),
			FLinearColor(1.f, 0.85f, 0.3f), CX - 160.f, Canvas->SizeY * 0.35f, nullptr, 1.6f);
	}
	if (GS && GS->Phase == ENexusRoundPhase::MatchEnd)
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), CX - 280.f, Canvas->SizeY * 0.28f, 560.f, 180.f);
		DrawText(TEXT("MATCH COMPLETE"), FLinearColor(0.9f, 0.85f, 0.4f), CX - 120.f, Canvas->SizeY * 0.3f, nullptr, 1.8f);
		DrawText(FString::Printf(TEXT("VANGUARD %d    SENTINEL %d"), GS->VanguardRounds, GS->SentinelRounds),
			FLinearColor::White, CX - 160.f, Canvas->SizeY * 0.36f, nullptr, 1.5f);
		DrawText(TEXT("nexus.Rematch   |   return to menu"), FLinearColor::Gray, CX - 140.f, Canvas->SizeY * 0.42f, nullptr, 1.1f);
	}
	if (GS && GS->Phase == ENexusRoundPhase::Overtime)
	{
		DrawText(TEXT("OVERTIME"), FLinearColor(1.f, 0.5f, 0.2f), CX - 70.f, 70.f, nullptr, 1.8f);
	}

	if (PC && PC->CalloutTimeRemaining > 0.f)
	{
		DrawText(PC->LastCallout.ToString(), FLinearColor(1.f, 0.85f, 0.2f), CX - 80.f, 80.f, nullptr, 1.8f);
	}

	if (PC && PC->bShowScoreboard && GS)
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.65f), CX - 360.f, 100.f, 720.f, 360.f);
		float Y = 110.f;
		DrawText(TEXT("PLAYER          K  D  A    $  PING  OP"), FLinearColor::Gray, CX - 340.f, Y, nullptr, 1.1f);
		Y += 22.f;
		auto DrawTeam = [&](ENexusFaction Fac, const TCHAR* Label)
		{
			DrawText(Label, FLinearColor::Yellow, CX - 340.f, Y, nullptr, 1.15f);
			Y += 20.f;
			for (TActorIterator<ANexusPlayerState> It(GetWorld()); It; ++It)
			{
				if (It->Faction != Fac) { continue; }
				const int32 Ping = It->GetCompressedPing() * 4;
				DrawText(FString::Printf(TEXT("%-14s %2d %2d %2d %5d %3d  %s"), *It->GetPlayerName(), It->Kills, It->Deaths, It->Assists, It->Credits, Ping, *UEnum::GetValueAsString(It->OperatorId)),
					FLinearColor::White, CX - 340.f, Y, nullptr, 1.05f);
				Y += 18.f;
			}
		};
		DrawTeam(ENexusFaction::Vanguard, TEXT("VANGUARD"));
		DrawTeam(ENexusFaction::Sentinel, TEXT("SENTINEL"));
	}

	if (PC && PC->bShowBuyMenu && GS && GS->IsBuyPhase())
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), 20.f, 100.f, 420.f, 340.f);
		DrawText(TEXT("BUY  console: Buy AK47 | Buy LightArmor"), FLinearColor::Yellow, 28.f, 108.f, nullptr, 1.1f);
		float Y = 130.f;
		int32 Shown = 0;
		for (const FNexusWeaponDefinition& W : S->Weapons)
		{
			if (Shown++ > 12) { break; }
			DrawText(FString::Printf(TEXT("%s  $%d"), *W.DisplayName.ToString(), W.Price), FLinearColor::White, 32.f, Y, nullptr, 1.f);
			Y += 16.f;
		}
	}

	DrawText(FString::Printf(TEXT("NEXUS V1.0  %s  greybox hidden  architecture pass"), FNexusMapCatalog::MapName(MapId)), FLinearColor(0.55f, 0.6f, 0.45f), 24.f, Canvas->SizeY - 22.f, nullptr, 0.95f);
}
