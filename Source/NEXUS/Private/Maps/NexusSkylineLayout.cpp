#include "Maps/NexusMapCatalog.h"
#include "Maps/NexusLayoutHelpers.h"

/**
 * SKYLINE — ville moderne, ~130x130 m (GDD poster map 2).
 * Sites A/B. Éléments clés: tour centrale, bureaux, parking, toits, passerelles.
 * Callouts principaux: A=Tour, B=Parking, Mid=Lobby, CT=Toits, T=Passerelle.
 * Attaquants: spawn ouest. Défenseurs: spawn est.
 *
 * La tour centrale EST le site A ("Tour"): plant au 2e étage (z=4.5) et au lobby.
 * Le rez-de-chaussée de la tour est le mid ("Lobby").
 * Deux passerelles à z=4.5 relient la tour aux bureaux ouest (attaquants) et est (défenseurs).
 */
FNexusVelasqoLayout NexusBuildSkylineLayout()
{
	using namespace NexusLayoutHelpers;
	FNexusVelasqoLayout L;

	// --- TERRAIN + PÉRIMÈTRE (130x130, top Z=0) ---
	AddBox(L, TEXT("Terrain"), FVector(0.f, 0.f, -0.25f), FVector(130.f, 130.f, 0.5f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("Perim_N"), FVector(0.f, 65.1f, 5.f), FVector(131.f, 0.4f, 10.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_S"), FVector(0.f, -65.1f, 5.f), FVector(131.f, 0.4f, 10.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_E"), FVector(65.1f, 0.f, 5.f), FVector(0.4f, 131.f, 10.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_W"), FVector(-65.1f, 0.f, 5.f), FVector(0.4f, 131.f, 10.f), ENexusMaterialType::Structural);

	// --- CHICANES SPAWN (murs segmentés, 2 passages chacun — pas de recouvrement) ---
	// Ouest x=-48: passages y[-24,-21] et y[13,16].
	AddBox(L, TEXT("ChiW_S"), FVector(-48.f, -34.5f, 2.5f), FVector(0.5f, 21.f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiW_M"), FVector(-48.f, -4.f, 2.5f), FVector(0.5f, 34.f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiW_N"), FVector(-48.f, 40.5f, 2.5f), FVector(0.5f, 49.f, 5.f), ENexusMaterialType::Concrete);
	// Est x=48: passages y[-16,-13] et y[18,21].
	AddBox(L, TEXT("ChiE_S"), FVector(48.f, -40.5f, 2.5f), FVector(0.5f, 49.f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiE_M"), FVector(48.f, 2.5f, 2.5f), FVector(0.5f, 31.f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiE_N"), FVector(48.f, 43.f, 2.5f), FVector(0.5f, 44.f, 5.f), ENexusMaterialType::Concrete);
	// Sud y=-42: passages x[-26,-23] et x[10,13].
	AddBox(L, TEXT("ChiS_W"), FVector(-45.5f, -42.f, 2.5f), FVector(39.f, 0.5f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiS_M"), FVector(-6.5f, -42.f, 2.5f), FVector(33.f, 0.5f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiS_E"), FVector(39.f, -42.f, 2.5f), FVector(52.f, 0.5f, 5.f), ENexusMaterialType::Concrete);
	// Nord y=42: passages x[-12,-9] et x[26,29].
	AddBox(L, TEXT("ChiN_W"), FVector(-38.5f, 42.f, 2.5f), FVector(53.f, 0.5f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiN_M"), FVector(8.5f, 42.f, 2.5f), FVector(35.f, 0.5f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiN_E"), FVector(47.f, 42.f, 2.5f), FVector(36.f, 0.5f, 5.f), ENexusMaterialType::Concrete);

	// --- TOUR CENTRALE (site A "Tour", mid "Lobby") — footprint 18x18 à (0,2), 2 niveaux + toit ---
	const float TwX = 0.f, TwY = 2.f;
	const float TwHx = 9.f, TwHy = 9.f; // demi-tailles
	const float F2 = 4.5f;   // hauteur plancher 2e étage
	const float RoofZ = 9.2f; // dalle toit

	// Lobby: dalle + murs RDC h4.5 avec 4 portes.
	AddBox(L, TEXT("TW_LobbyFloor"), FVector(TwX, TwY, 0.08f), FVector(18.f, 18.f, 0.16f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddWallYDoor(L, TEXT("TW_WallW"), TwX - TwHx + 0.15f, TwY, 0.f, 18.f, F2, 0.3f, TwY, 2.4f, 2.6f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("TW_WallE"), TwX + TwHx - 0.15f, TwY, 0.f, 18.f, F2, 0.3f, TwY, 2.4f, 2.6f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("TW_WallS"), TwX, TwY - TwHy + 0.15f, 0.f, 18.f, F2, 0.3f, TwX, 2.4f, 2.6f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("TW_WallN"), TwX, TwY + TwHy - 0.15f, 0.f, 18.f, F2, 0.3f, TwX, 2.4f, 2.6f, ENexusMaterialType::Concrete);
	// Colonnes lobby.
	AddBox(L, TEXT("TW_Col1"), FVector(TwX - 4.f, TwY - 4.f, 2.2f), FVector(0.6f, 0.6f, 4.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TW_Col2"), FVector(TwX + 4.f, TwY - 4.f, 2.2f), FVector(0.6f, 0.6f, 4.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TW_Col3"), FVector(TwX - 4.f, TwY + 4.f, 2.2f), FVector(0.6f, 0.6f, 4.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TW_Col4"), FVector(TwX + 4.f, TwY + 4.f, 2.2f), FVector(0.6f, 0.6f, 4.4f), ENexusMaterialType::Structural);
	// Accueil lobby (cover).
	AddBox(L, TEXT("TW_Desk"), FVector(TwX + 3.f, TwY + 5.f, 0.55f), FVector(3.2f, 1.1f, 1.1f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);

	// Dalle 2e étage (trou d'escalier au sud-ouest).
	AddBox(L, TEXT("TW_F2SlabE"), FVector(TwX + 2.f, TwY, F2 - 0.1f), FVector(14.f, 18.f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("TW_F2SlabW"), FVector(TwX - 7.f, TwY + 3.25f, F2 - 0.1f), FVector(4.f, 11.5f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	// Escalier lobby -> 2e (sud-ouest, monte vers +Y).
	AddStairs(L, TEXT("TW_Stair1"), FVector(TwX - 6.5f, TwY - 7.2f, 0.f), FVector(0.f, 1.f, 0.f), 1.7f, F2, 16, ENexusMaterialType::Structural);

	// Bandeaux 2e étage: parapet h1.1 + vitrage destructible h1.9 + linteau — par face,
	// avec trouées passerelles sur W et E (y ~ [0.7, 3.3]).
	auto TowerBandY = [&](const TCHAR* Tag, float FaceX, bool bBridgeGap)
	{
		const float P0 = F2, P1 = F2 + 1.1f, G1 = F2 + 3.0f, H1 = RoofZ;
		if (bBridgeGap)
		{
			// Segments y: [-7, 0.7] et [3.3, 11] (local à la face, longueur totale 18 centrée TwY=2).
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Par_S"), Tag)), FVector(FaceX, -3.15f + 0.f, (P0 + P1) * 0.5f), FVector(0.3f, 7.7f, P1 - P0), ENexusMaterialType::Concrete);
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Par_N"), Tag)), FVector(FaceX, 7.15f, (P0 + P1) * 0.5f), FVector(0.3f, 7.7f, P1 - P0), ENexusMaterialType::Concrete);
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Glass_S"), Tag)), FVector(FaceX, -3.15f, (P1 + G1) * 0.5f), FVector(0.15f, 7.7f, G1 - P1), ENexusMaterialType::Glass, ENexusBoxUsage::Destructible, true, 300.f, 0.05f);
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Glass_N"), Tag)), FVector(FaceX, 7.15f, (P1 + G1) * 0.5f), FVector(0.15f, 7.7f, G1 - P1), ENexusMaterialType::Glass, ENexusBoxUsage::Destructible, true, 300.f, 0.05f);
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Head"), Tag)), FVector(FaceX, 2.f, (G1 + H1) * 0.5f), FVector(0.3f, 18.f, H1 - G1), ENexusMaterialType::Concrete);
		}
		else
		{
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Par"), Tag)), FVector(FaceX, 2.f, (P0 + P1) * 0.5f), FVector(0.3f, 18.f, P1 - P0), ENexusMaterialType::Concrete);
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Glass"), Tag)), FVector(FaceX, 2.f, (P1 + G1) * 0.5f), FVector(0.15f, 18.f, G1 - P1), ENexusMaterialType::Glass, ENexusBoxUsage::Destructible, true, 300.f, 0.05f);
			AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Head"), Tag)), FVector(FaceX, 2.f, (G1 + H1) * 0.5f), FVector(0.3f, 18.f, H1 - G1), ENexusMaterialType::Concrete);
		}
	};
	auto TowerBandX = [&](const TCHAR* Tag, float FaceY)
	{
		const float P0 = F2, P1 = F2 + 1.1f, G1 = F2 + 3.0f, H1 = RoofZ;
		AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Par"), Tag)), FVector(0.f, FaceY, (P0 + P1) * 0.5f), FVector(18.f, 0.3f, P1 - P0), ENexusMaterialType::Concrete);
		AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Glass"), Tag)), FVector(0.f, FaceY, (P1 + G1) * 0.5f), FVector(18.f, 0.15f, G1 - P1), ENexusMaterialType::Glass, ENexusBoxUsage::Destructible, true, 300.f, 0.05f);
		AddBox(L, FName(*FString::Printf(TEXT("TW_%s_Head"), Tag)), FVector(0.f, FaceY, (G1 + H1) * 0.5f), FVector(18.f, 0.3f, H1 - G1), ENexusMaterialType::Concrete);
	};
	TowerBandY(TEXT("F2W"), TwX - TwHx + 0.15f, true);
	TowerBandY(TEXT("F2E"), TwX + TwHx - 0.15f, true);
	TowerBandX(TEXT("F2S"), TwY - TwHy + 0.15f);
	TowerBandX(TEXT("F2N"), TwY + TwHy - 0.15f);

	// Toit "Toits" (CT): dalle + parapet, trou d'escalier à l'est.
	AddBox(L, TEXT("TW_RoofW"), FVector(TwX - 2.f, TwY, RoofZ), FVector(14.f, 18.f, 0.3f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("TW_RoofE"), FVector(TwX + 7.f, TwY + 4.25f, RoofZ), FVector(4.f, 9.5f, 0.3f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("TW_RoofParN"), FVector(TwX, TwY + TwHy - 0.15f, RoofZ + 0.7f), FVector(18.f, 0.3f, 1.1f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("TW_RoofParS"), FVector(TwX, TwY - TwHy + 0.15f, RoofZ + 0.7f), FVector(18.f, 0.3f, 1.1f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("TW_RoofParE"), FVector(TwX + TwHx - 0.15f, TwY, RoofZ + 0.7f), FVector(0.3f, 18.f, 1.1f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("TW_RoofParW"), FVector(TwX - TwHx + 0.15f, TwY, RoofZ + 0.7f), FVector(0.3f, 18.f, 1.1f), ENexusMaterialType::Concrete);
	// Escalier 2e -> toit (est, monte vers +Y).
	AddStairs(L, TEXT("TW_Stair2"), FVector(TwX + 6.8f, TwY - 6.2f, F2), FVector(0.f, 1.f, 0.f), 1.6f, RoofZ - F2 + 0.15f, 17, ENexusMaterialType::Structural);

	// --- PASSERELLES (T) à z=4.5 ---
	// Est: tour -> bureaux est (x 9 -> 28).
	AddBox(L, TEXT("PS_E_Floor"), FVector(18.5f, 2.f, F2 - 0.08f), FVector(19.f, 2.6f, 0.16f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("PS_E_RailN"), FVector(18.5f, 3.2f, F2 + 0.55f), FVector(19.f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("PS_E_RailS"), FVector(18.5f, 0.8f, F2 + 0.55f), FVector(19.f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	// Ouest: bureaux ouest -> tour (x -18 -> -9).
	AddBox(L, TEXT("PS_W_Floor"), FVector(-13.5f, 2.f, F2 - 0.08f), FVector(9.f, 2.6f, 0.16f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("PS_W_RailN"), FVector(-13.5f, 3.2f, F2 + 0.55f), FVector(9.f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("PS_W_RailS"), FVector(-13.5f, 0.8f, F2 + 0.55f), FVector(9.f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);

	// --- BUREAUX OUEST (attaquants) à (-24,2), 12x10, 2 niveaux ---
	AddBox(L, TEXT("OFW_Floor0"), FVector(-24.f, 2.f, 0.08f), FVector(12.f, 10.f, 0.16f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddWallYDoor(L, TEXT("OFW_WallW"), -29.85f, 2.f, 0.f, 10.f, 9.f, 0.3f, 2.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("OFW_WallE"), -18.15f, 2.f, 0.f, 10.f, 9.f, 0.3f, -1.f, 1.6f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("OFW_WallS"), -24.f, -2.85f, 0.f, 12.f, 9.f, 0.3f, -26.f, 1.6f, 2.4f, ENexusMaterialType::Concrete);
	AddBox(L, TEXT("OFW_WallN"), FVector(-24.f, 6.85f, 4.5f), FVector(12.f, 0.3f, 9.f), ENexusMaterialType::Concrete);
	// Dalle 2e (trou d'escalier au sud).
	AddBox(L, TEXT("OFW_F2"), FVector(-24.f, 3.4f, F2 - 0.1f), FVector(11.4f, 6.6f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddStairs(L, TEXT("OFW_Stair"), FVector(-27.5f, -1.9f, 0.f), FVector(0.f, 1.f, 0.f), 1.5f, F2, 16, ENexusMaterialType::Structural);
	// Ouverture passerelle côté est au 2e: mur est percé au-dessus (gap y [0.7,3.3] z [4.5,7.2]).
	AddBox(L, TEXT("OFW_F2ES"), FVector(-18.15f, -1.65f, 6.75f), FVector(0.3f, 6.7f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("OFW_F2EN"), FVector(-18.15f, 5.15f, 6.75f), FVector(0.3f, 3.4f, 4.5f), ENexusMaterialType::Concrete);
	// Fenêtre 2e face est (vitrage destructible au-dessus du gap).
	AddBox(L, TEXT("OFW_F2Glass"), FVector(-18.15f, 2.f, 8.1f), FVector(0.12f, 2.6f, 2.2f), ENexusMaterialType::Glass, ENexusBoxUsage::Destructible, true, 300.f, 0.05f);
	// Toit plein (pas d'accès).
	AddBox(L, TEXT("OFW_Roof"), FVector(-24.f, 2.f, 9.05f), FVector(12.f, 10.f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);

	// --- BUREAUX EST (défenseurs) à (36,4), 16x14, 2 niveaux ---
	AddBox(L, TEXT("OFE_Floor0"), FVector(36.f, 4.f, 0.08f), FVector(16.f, 14.f, 0.16f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddWallYDoor(L, TEXT("OFE_WallW"), 28.15f, 4.f, 0.f, 14.f, 9.f, 0.3f, 0.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("OFE_WallE"), 43.85f, 4.f, 0.f, 14.f, 9.f, 0.3f, 8.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("OFE_WallS"), 36.f, -2.85f, 0.f, 16.f, 9.f, 0.3f, 36.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("OFE_WallN"), 36.f, 10.85f, 0.f, 16.f, 9.f, 0.3f, 40.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	// Dalle 2e (trou d'escalier au nord-est).
	AddBox(L, TEXT("OFE_F2W"), FVector(31.f, 4.f, F2 - 0.1f), FVector(5.4f, 13.4f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("OFE_F2E"), FVector(38.5f, 1.f, F2 - 0.1f), FVector(9.6f, 7.4f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddStairs(L, TEXT("OFE_Stair"), FVector(40.f, 5.2f, 0.f), FVector(0.f, 1.f, 0.f), 1.6f, F2, 16, ENexusMaterialType::Structural);
	// Ouverture passerelle côté ouest au 2e.
	AddBox(L, TEXT("OFE_F2WS"), FVector(28.15f, -0.65f, 6.75f), FVector(0.3f, 4.7f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("OFE_F2WN"), FVector(28.15f, 7.15f, 6.75f), FVector(0.3f, 7.7f, 4.5f), ENexusMaterialType::Concrete);
	// Vitrage 2e face ouest (regarde le mid — destructible).
	AddBox(L, TEXT("OFE_F2Glass"), FVector(28.15f, 2.f, 8.1f), FVector(0.12f, 2.6f, 2.2f), ENexusMaterialType::Glass, ENexusBoxUsage::Destructible, true, 300.f, 0.05f);
	AddBox(L, TEXT("OFE_Roof"), FVector(36.f, 4.f, 9.05f), FVector(16.f, 14.f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);

	// --- PLAZA NORD-OUEST (approche A) ---
	AddBox(L, TEXT("PLZ_Floor"), FVector(-24.f, 22.f, 0.04f), FVector(20.f, 16.f, 0.08f), ENexusMaterialType::LightStone, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("PLZ_Planter1"), FVector(-30.f, 26.f, 0.55f), FVector(2.6f, 1.2f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("PLZ_Planter2"), FVector(-20.f, 18.f, 0.55f), FVector(2.6f, 1.2f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("PLZ_Kiosk"), FVector(-26.f, 20.f, 1.4f), FVector(3.2f, 2.4f, 2.8f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	// Immeuble backdrop nord (plein).
	AddBox(L, TEXT("PLZ_Block"), FVector(-24.f, 34.f, 4.f), FVector(20.f, 8.f, 8.f), ENexusMaterialType::Concrete);

	// --- PARKING (site B) à (26,-18), couvert, 24x18 ---
	const float PkX = 26.f, PkY = -18.f;
	AddBox(L, TEXT("PK_Floor"), FVector(PkX, PkY, 0.06f), FVector(24.f, 18.f, 0.12f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	// Mur nord avec porte (Double), mur est avec porte (Reinforced), sud demi-mur (cover), ouest ouvert.
	AddWallXDoor(L, TEXT("PK_WallN"), PkX, PkY + 8.85f, 0.f, 24.f, 3.5f, 0.3f, PkX, 2.2f, 2.6f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("PK_WallE"), PkX + 11.85f, PkY, 0.f, 18.f, 3.5f, 0.3f, PkY, 2.0f, 2.6f, ENexusMaterialType::Concrete);
	AddBox(L, TEXT("PK_WallS"), FVector(PkX, PkY - 8.85f, 0.6f), FVector(24.f, 0.3f, 1.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	// Toit sur piliers.
	AddBox(L, TEXT("PK_Roof"), FVector(PkX, PkY, 3.5f), FVector(24.f, 18.f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("PK_Pil1"), FVector(PkX - 8.f, PkY - 5.f, 1.7f), FVector(0.5f, 0.5f, 3.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("PK_Pil2"), FVector(PkX, PkY - 5.f, 1.7f), FVector(0.5f, 0.5f, 3.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("PK_Pil3"), FVector(PkX + 8.f, PkY - 5.f, 1.7f), FVector(0.5f, 0.5f, 3.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("PK_Pil4"), FVector(PkX - 8.f, PkY + 5.f, 1.7f), FVector(0.5f, 0.5f, 3.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("PK_Pil5"), FVector(PkX + 8.f, PkY + 5.f, 1.7f), FVector(0.5f, 0.5f, 3.4f), ENexusMaterialType::Structural);
	// Voitures garées (cover).
	AddBox(L, TEXT("PK_Car1"), FVector(PkX - 5.f, PkY - 3.f, 0.7f), FVector(4.2f, 1.8f, 1.4f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("PK_Car2"), FVector(PkX + 3.f, PkY + 3.f, 0.7f), FVector(4.2f, 1.8f, 1.4f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("PK_Car3"), FVector(PkX + 6.f, PkY - 4.f, 0.7f), FVector(4.2f, 1.8f, 1.4f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	// Cloison technique destructible (wallbang).
	AddBox(L, TEXT("PK_Tech"), FVector(PkX - 9.f, PkY + 3.f, 1.5f), FVector(0.12f, 5.f, 3.f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.12f);

	// --- BLOQUEURS DE LIGNES (immeubles pleins) ---
	AddBox(L, TEXT("BLK_W1"), FVector(-44.f, 6.f, 3.5f), FVector(8.f, 16.f, 7.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("BLK_E1"), FVector(46.f, -16.f, 3.5f), FVector(6.f, 14.f, 7.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("BLK_S1"), FVector(6.f, -30.f, 3.f), FVector(14.f, 6.f, 6.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("BLK_N1"), FVector(16.f, 30.f, 3.5f), FVector(12.f, 8.f, 7.f), ENexusMaterialType::Concrete);

	// --- COUVERTURE MID ---
	AddBox(L, TEXT("MID_Kiosk"), FVector(-2.f, -14.f, 1.4f), FVector(4.f, 3.f, 2.8f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("MID_Jersey1"), FVector(-11.f, -8.f, 0.55f), FVector(3.6f, 0.7f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("MID_Planter"), FVector(8.f, -10.f, 0.55f), FVector(2.6f, 1.2f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("MID_Jersey2"), FVector(-6.f, 14.f, 0.55f), FVector(3.6f, 0.7f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);

	// --- SITES ---
	FNexusSiteDef SiteA;
	SiteA.SiteId = ENexusSiteId::A;
	SiteA.CenterMeters = FVector(TwX, TwY, 0.f);
	SiteA.SizeMeters = FVector(18.f, 18.f, 12.f);
	SiteA.PlantZones = {
		{ ENexusSiteId::A, TEXT("A_Plant_F2"), FVector(TwX, TwY + 4.f, F2), 1.5f },
		{ ENexusSiteId::A, TEXT("A_Plant_F2W"), FVector(TwX - 5.f, TwY - 1.f, F2), 1.5f },
		{ ENexusSiteId::A, TEXT("A_Plant_Lobby"), FVector(TwX + 5.f, TwY + 5.f, 0.f), 1.5f }
	};
	SiteA.EntranceCentersMeters = {
		FVector(TwX - 9.f, TwY, 1.f), FVector(TwX + 9.f, TwY, 1.f),
		FVector(TwX, TwY - 9.f, 1.f), FVector(TwX, TwY + 9.f, 1.f),
		FVector(-13.5f, 2.f, F2 + 1.f), FVector(18.5f, 2.f, F2 + 1.f)
	};
	SiteA.DefensivePositionsMeters = { FVector(TwX + 6.f, TwY - 4.f, F2), FVector(TwX + 3.f, TwY + 5.f, 0.f) };
	L.Sites.Add(SiteA);

	FNexusSiteDef SiteB;
	SiteB.SiteId = ENexusSiteId::B;
	SiteB.CenterMeters = FVector(PkX, PkY, 0.f);
	SiteB.SizeMeters = FVector(24.f, 18.f, 6.f);
	SiteB.PlantZones = {
		{ ENexusSiteId::B, TEXT("B_Plant_Centre"), FVector(PkX, PkY, 0.f), 1.5f },
		{ ENexusSiteId::B, TEXT("B_Plant_NE"), FVector(PkX + 6.f, PkY + 5.f, 0.f), 1.5f },
		{ ENexusSiteId::B, TEXT("B_Plant_SW"), FVector(PkX - 6.f, PkY - 5.f, 0.f), 1.5f }
	};
	SiteB.EntranceCentersMeters = { FVector(PkX - 12.5f, PkY, 1.f), FVector(PkX, PkY + 9.f, 1.f), FVector(PkX + 12.5f, PkY, 1.f) };
	SiteB.DefensivePositionsMeters = { FVector(PkX + 8.f, PkY - 6.f, 0.f), FVector(PkX - 5.f, PkY + 4.f, 0.f) };
	L.Sites.Add(SiteB);

	// --- COVERS ---
	L.Covers = {
		{ FVector(-24.f, 22.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.3f },
		{ FVector(-14.f, 10.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(-2.f, -10.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.3f },
		{ FVector(14.f, -12.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(20.f, -24.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.4f },
		{ FVector(34.f, -20.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(30.f, 14.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.3f },
		{ FVector(-34.f, 14.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(4.f, 22.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.1f },
		{ FVector(-30.f, -20.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusCoverType::High, 1.2f }
	};

	// --- PORTES ---
	L.Doors = {
		{ TEXT("Door_TW_W"), FVector(TwX - 8.85f, TwY, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Double },
		{ TEXT("Door_TW_E"), FVector(TwX + 8.85f, TwY, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Double },
		{ TEXT("Door_TW_S"), FVector(TwX, TwY - 8.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_TW_N"), FVector(TwX, TwY + 8.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_OFW_W"), FVector(-29.85f, 2.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_OFW_E"), FVector(-18.15f, -1.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_OFE_W"), FVector(28.15f, 4.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_OFE_N"), FVector(40.f, 10.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_PK_N"), FVector(PkX, PkY + 8.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Double },
		{ TEXT("Door_PK_E"), FVector(PkX + 11.85f, PkY, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Reinforced }
	};

	// --- SPAWNS (attaquants ouest, défenseurs est — fidèle au poster) ---
	AddSpawn(L, 1, ENexusTeam::Attackers, FVector(-56.f, -20.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 2, ENexusTeam::Attackers, FVector(-56.f, -14.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 3, ENexusTeam::Attackers, FVector(-56.f, -8.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 4, ENexusTeam::Attackers, FVector(-56.f, -2.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 5, ENexusTeam::Attackers, FVector(-56.f, 4.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 6, ENexusTeam::Attackers, FVector(-56.f, 10.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 7, ENexusTeam::Attackers, FVector(-56.f, 16.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 8, ENexusTeam::Attackers, FVector(-60.f, -14.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 9, ENexusTeam::Attackers, FVector(-60.f, -2.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 10, ENexusTeam::Attackers, FVector(-60.f, 10.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 11, ENexusTeam::Attackers, FVector(-56.f, -26.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 12, ENexusTeam::Attackers, FVector(-56.f, 22.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 13, ENexusTeam::Attackers, FVector(-60.f, -26.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 14, ENexusTeam::Attackers, FVector(-60.f, 22.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 15, ENexusTeam::Attackers, FVector(-60.f, 4.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));

	AddSpawn(L, 1, ENexusTeam::Defenders, FVector(56.f, -8.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 2, ENexusTeam::Defenders, FVector(56.f, -2.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 3, ENexusTeam::Defenders, FVector(56.f, 4.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 4, ENexusTeam::Defenders, FVector(56.f, 10.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 5, ENexusTeam::Defenders, FVector(56.f, 16.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 6, ENexusTeam::Defenders, FVector(60.f, -8.f, 0.f), ENexusRouteId::RotateLongBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 7, ENexusTeam::Defenders, FVector(60.f, 0.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 8, ENexusTeam::Defenders, FVector(60.f, 8.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 9, ENexusTeam::Defenders, FVector(60.f, 16.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 10, ENexusTeam::Defenders, FVector(56.f, 22.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, 180.f, 0.f));

	// --- CALLOUTS (>=15 requis par le validateur) ---
	AddCallout(L, TEXT("SpawnOuest"), TEXT("Spawn Ouest"), FVector(-56.f, 0.f, 3.f), FVector(9.f, 30.f, 4.f));
	AddCallout(L, TEXT("SpawnEst"), TEXT("Spawn Est"), FVector(58.f, 8.f, 3.f), FVector(8.f, 20.f, 4.f));
	AddCallout(L, TEXT("Tour"), TEXT("Tour"), FVector(TwX, TwY, 7.f), FVector(9.f, 9.f, 6.f));
	AddCallout(L, TEXT("Lobby"), TEXT("Lobby"), FVector(TwX, TwY, 2.f), FVector(9.f, 9.f, 2.4f));
	AddCallout(L, TEXT("Toits"), TEXT("Toits"), FVector(TwX, TwY, 10.5f), FVector(9.f, 9.f, 2.f));
	AddCallout(L, TEXT("PasserelleEst"), TEXT("Passerelle Est"), FVector(18.5f, 2.f, 5.5f), FVector(9.5f, 1.5f, 2.f));
	AddCallout(L, TEXT("PasserelleOuest"), TEXT("Passerelle Ouest"), FVector(-13.5f, 2.f, 5.5f), FVector(4.5f, 1.5f, 2.f));
	AddCallout(L, TEXT("Bureaux"), TEXT("Bureaux"), FVector(36.f, 4.f, 4.f), FVector(8.f, 7.f, 5.f));
	AddCallout(L, TEXT("BureauxOuest"), TEXT("Bureaux Ouest"), FVector(-24.f, 2.f, 4.f), FVector(6.f, 5.f, 5.f));
	AddCallout(L, TEXT("Plaza"), TEXT("Plaza"), FVector(-24.f, 22.f, 2.f), FVector(10.f, 8.f, 4.f));
	AddCallout(L, TEXT("Parking"), TEXT("Parking"), FVector(PkX, PkY, 2.f), FVector(12.f, 9.f, 3.5f));
	AddCallout(L, TEXT("RueSud"), TEXT("Rue Sud"), FVector(-4.f, -36.f, 2.f), FVector(34.f, 6.f, 4.f));
	AddCallout(L, TEXT("RueNord"), TEXT("Rue Nord"), FVector(-4.f, 26.f, 2.f), FVector(24.f, 6.f, 4.f));
	AddCallout(L, TEXT("SKYLINE_A_TOWER"), TEXT("A TOWER"), FVector(TwX, TwY, 7.f), FVector(9.f, 9.f, 6.f));
	AddCallout(L, TEXT("SKYLINE_B_PARKING"), TEXT("B PARKING"), FVector(PkX, PkY, 2.f), FVector(12.f, 9.f, 3.5f));
	AddCallout(L, TEXT("SKYLINE_MID_LOBBY"), TEXT("MID LOBBY"), FVector(TwX, TwY, 2.f), FVector(9.f, 9.f, 2.4f));
	AddCallout(L, TEXT("SKYLINE_CT_ROOF"), TEXT("CT ROOF"), FVector(TwX, TwY, 10.5f), FVector(9.f, 9.f, 2.f));
	AddCallout(L, TEXT("SKYLINE_T_BRIDGE"), TEXT("T BRIDGE"), FVector(18.5f, 2.f, 5.5f), FVector(9.5f, 1.5f, 2.f));

	// --- ROUTES ---
	AddRoute(L, ENexusRouteId::RouteA, TEXT("ROUTE_A"), {
		FVector(-56.f, 10.f, 0.f), FVector(-48.f, 14.5f, 0.f), FVector(-36.f, 10.f, 0.f), FVector(-32.f, 4.f, 0.f),
		FVector(-24.f, 2.f, 0.f), FVector(-24.f, 2.f, 4.5f), FVector(-13.5f, 2.f, 4.5f), FVector(0.f, 4.f, 4.5f)
	});
	AddRoute(L, ENexusRouteId::RouteB, TEXT("ROUTE_B"), {
		FVector(-56.f, -16.f, 0.f), FVector(-48.f, -22.5f, 0.f), FVector(-36.f, -30.f, 0.f), FVector(-16.f, -36.f, 0.f),
		FVector(6.f, -38.f, 0.f), FVector(16.f, -30.f, 0.f), FVector(18.f, -22.f, 0.f), FVector(26.f, -18.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RouteC, TEXT("ROUTE_C"), {
		FVector(-56.f, -6.f, 0.f), FVector(-48.f, -19.5f, 0.f), FVector(-30.f, -8.f, 0.f), FVector(-14.f, -2.f, 0.f),
		FVector(-9.f, 2.f, 0.f), FVector(0.f, 2.f, 0.f), FVector(3.f, 6.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateShortAB, TEXT("ROTATE_SHORT_AB"), {
		FVector(0.f, 2.f, 0.f), FVector(0.f, -7.f, 0.f), FVector(6.f, -12.f, 0.f), FVector(13.f, -18.f, 0.f),
		FVector(26.f, -18.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateShortBA, TEXT("ROTATE_SHORT_BA"), {
		FVector(26.f, -18.f, 0.f), FVector(13.f, -18.f, 0.f), FVector(6.f, -12.f, 0.f), FVector(0.f, -7.f, 0.f),
		FVector(0.f, 2.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateLongAB, TEXT("ROTATE_LONG_AB"), {
		FVector(0.f, 4.f, 4.5f), FVector(18.5f, 2.f, 4.5f), FVector(33.f, 2.f, 4.5f), FVector(36.f, 4.f, 0.f),
		FVector(36.f, -3.f, 0.f), FVector(30.f, -8.f, 0.f), FVector(26.f, -9.f, 0.f), FVector(26.f, -18.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateLongBA, TEXT("ROTATE_LONG_BA"), {
		FVector(26.f, -18.f, 0.f), FVector(26.f, -9.f, 0.f), FVector(30.f, -8.f, 0.f), FVector(36.f, -3.f, 0.f),
		FVector(36.f, 4.f, 0.f), FVector(33.f, 2.f, 4.5f), FVector(18.5f, 2.f, 4.5f), FVector(0.f, 4.f, 4.5f)
	});

	return L;
}
