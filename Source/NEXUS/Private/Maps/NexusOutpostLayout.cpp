#include "Maps/NexusMapCatalog.h"
#include "Maps/NexusLayoutHelpers.h"

/**
 * OUTPOST — base de ravitaillement militaire désertique, ~140x140 m (GDD poster map 3).
 * Sites A/B. Éléments clés: hangars, conteneurs, zone de ravitaillement, tours de garde.
 * Callouts principaux: A=Hangar, B=Entrepôt, Mid=Zone centrale, CT=Tour, T=Conteneurs.
 * Attaquants: spawn ouest. Défenseurs: spawn est.
 */
FNexusVelasqoLayout NexusBuildOutpostLayout()
{
	using namespace NexusLayoutHelpers;
	FNexusVelasqoLayout L;

	// --- TERRAIN + PÉRIMÈTRE (140x140, top Z=0) ---
	AddBox(L, TEXT("Terrain"), FVector(0.f, 0.f, -0.25f), FVector(140.f, 140.f, 0.5f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("Perim_N"), FVector(0.f, 70.1f, 4.f), FVector(141.f, 0.4f, 8.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_S"), FVector(0.f, -70.1f, 4.f), FVector(141.f, 0.4f, 8.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_E"), FVector(70.1f, 0.f, 4.f), FVector(0.4f, 141.f, 8.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_W"), FVector(-70.1f, 0.f, 4.f), FVector(0.4f, 141.f, 8.f), ENexusMaterialType::Structural);

	// --- CHICANES SPAWN (segmentées, 2 passages chacune) ---
	// Ouest x=-50: passages y[-26,-23] et y[16,19].
	AddBox(L, TEXT("ChiW_S"), FVector(-50.f, -48.f, 2.25f), FVector(0.5f, 44.f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiW_M"), FVector(-50.f, -3.5f, 2.25f), FVector(0.5f, 39.f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiW_N"), FVector(-50.f, 44.5f, 2.25f), FVector(0.5f, 51.f, 4.5f), ENexusMaterialType::Concrete);
	// Est x=50: passages y[-14,-11] et y[12,15].
	AddBox(L, TEXT("ChiE_S"), FVector(50.f, -42.f, 2.25f), FVector(0.5f, 56.f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiE_M"), FVector(50.f, 0.5f, 2.25f), FVector(0.5f, 23.f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiE_N"), FVector(50.f, 42.5f, 2.25f), FVector(0.5f, 55.f, 4.5f), ENexusMaterialType::Concrete);
	// Sud y=-44: passages x[-30,-27] et x[8,11].
	AddBox(L, TEXT("ChiS_W"), FVector(-50.f, -44.f, 2.25f), FVector(40.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiS_M"), FVector(-9.5f, -44.f, 2.25f), FVector(35.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiS_E"), FVector(40.5f, -44.f, 2.25f), FVector(59.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);
	// Nord y=44: passages x[-16,-13] et x[24,27].
	AddBox(L, TEXT("ChiN_W"), FVector(-43.f, 44.f, 2.25f), FVector(54.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiN_M"), FVector(5.5f, 44.f, 2.25f), FVector(37.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ChiN_E"), FVector(48.5f, 44.f, 2.25f), FVector(43.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);

	// --- SITE A: HANGAR NORD-OUEST à (-30,24), 26x20, h7, mezzanine ---
	const float HgX = -30.f, HgY = 24.f;
	AddBox(L, TEXT("HG_Floor"), FVector(HgX, HgY, 0.08f), FVector(26.f, 20.f, 0.16f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	// Face sud: grande ouverture véhicule (8 m, linteau au-dessus de 4.5).
	AddWallXDoor(L, TEXT("HG_WallS"), HgX, HgY - 9.85f, 0.f, 26.f, 7.f, 0.3f, HgX, 8.f, 4.5f, ENexusMaterialType::Concrete);
	// Face nord pleine.
	AddBox(L, TEXT("HG_WallN"), FVector(HgX, HgY + 9.85f, 3.5f), FVector(26.f, 0.3f, 7.f), ENexusMaterialType::Concrete);
	// Faces est/ouest avec portes de service.
	AddWallYDoor(L, TEXT("HG_WallE"), HgX + 12.85f, HgY, 0.f, 20.f, 7.f, 0.3f, HgY - 4.f, 1.6f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("HG_WallW"), HgX - 12.85f, HgY, 0.f, 20.f, 7.f, 0.3f, HgY + 4.f, 1.6f, 2.4f, ENexusMaterialType::Concrete);
	// Toit plein.
	AddBox(L, TEXT("HG_Roof"), FVector(HgX, HgY, 7.1f), FVector(26.f, 20.f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	// Mezzanine le long du mur nord (z=3.2) + escalier + rambarde.
	AddBox(L, TEXT("HG_Mezz"), FVector(HgX, HgY + 7.4f, 3.2f), FVector(22.f, 4.4f, 0.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("HG_MezzRail"), FVector(HgX, HgY + 5.3f, 3.85f), FVector(22.f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddStairs(L, TEXT("HG_MezzStair"), FVector(HgX - 11.3f, HgY + 1.4f, 0.f), FVector(0.f, 1.f, 0.f), 1.5f, 3.2f, 12, ENexusMaterialType::Structural);
	// Caisses (cover).
	AddBox(L, TEXT("HG_Crate1"), FVector(HgX - 5.f, HgY - 3.f, 0.65f), FVector(2.2f, 1.6f, 1.3f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("HG_Crate2"), FVector(HgX + 4.f, HgY + 1.f, 0.65f), FVector(2.2f, 1.6f, 1.3f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("HG_Crate3"), FVector(HgX + 8.f, HgY - 5.f, 0.55f), FVector(1.8f, 1.4f, 1.1f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	// Cloison bureau destructible dans le coin est.
	AddBox(L, TEXT("HG_Office"), FVector(HgX + 9.f, HgY + 4.f, 1.5f), FVector(0.12f, 6.f, 3.f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.12f);

	FNexusSiteDef SiteA;
	SiteA.SiteId = ENexusSiteId::A;
	SiteA.CenterMeters = FVector(HgX, HgY, 0.f);
	SiteA.SizeMeters = FVector(26.f, 20.f, 8.f);
	SiteA.PlantZones = {
		{ ENexusSiteId::A, TEXT("A_Plant_Centre"), FVector(HgX, HgY - 2.f, 0.f), 1.5f },
		{ ENexusSiteId::A, TEXT("A_Plant_Est"), FVector(HgX + 6.f, HgY + 4.f, 0.f), 1.5f },
		{ ENexusSiteId::A, TEXT("A_Plant_Mezz"), FVector(HgX, HgY + 7.4f, 3.25f), 1.5f }
	};
	SiteA.EntranceCentersMeters = { FVector(HgX, HgY - 10.5f, 1.f), FVector(HgX + 13.3f, HgY - 4.f, 1.f), FVector(HgX - 13.3f, HgY + 4.f, 1.f) };
	SiteA.DefensivePositionsMeters = { FVector(HgX - 6.f, HgY + 7.4f, 3.2f), FVector(HgX + 4.f, HgY + 1.f, 0.f) };
	L.Sites.Add(SiteA);

	// --- SITE B: ENTREPÔT EST à (32,-8), 22x16, h6, quai de chargement ---
	const float WhX = 32.f, WhY = -8.f;
	AddBox(L, TEXT("WH_Floor"), FVector(WhX, WhY, 0.08f), FVector(22.f, 16.f, 0.16f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	// Ouest: rideau métallique destructible (bois gameplay) 4 m.
	AddWallYDoor(L, TEXT("WH_WallW"), WhX - 10.85f, WhY, 0.f, 16.f, 6.f, 0.25f, WhY, 4.f, 3.f, ENexusMaterialType::Wood, true, 600.f);
	// Est plein, nord porte renforcée, sud porte standard.
	AddBox(L, TEXT("WH_WallE"), FVector(WhX + 10.85f, WhY, 3.f), FVector(0.25f, 16.f, 6.f), ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("WH_WallN"), WhX, WhY + 7.85f, 0.f, 22.f, 6.f, 0.25f, WhX - 6.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("WH_WallS"), WhX, WhY - 7.85f, 0.f, 22.f, 6.f, 0.25f, WhX + 6.f, 1.8f, 2.4f, ENexusMaterialType::Concrete);
	AddBox(L, TEXT("WH_Roof"), FVector(WhX, WhY, 6.1f), FVector(22.f, 16.f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	// Racks (cover haut) en deux rangées.
	AddBox(L, TEXT("WH_Rack1"), FVector(WhX - 3.f, WhY + 3.5f, 1.1f), FVector(7.f, 1.2f, 2.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("WH_Rack2"), FVector(WhX + 3.f, WhY - 3.5f, 1.1f), FVector(7.f, 1.2f, 2.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("WH_Pallet"), FVector(WhX - 6.f, WhY - 4.5f, 0.45f), FVector(1.6f, 1.2f, 0.9f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	// Quai de chargement extérieur sud (plateforme h1.2 + marches).
	AddBox(L, TEXT("WH_Dock"), FVector(WhX + 2.f, WhY - 10.f, 0.6f), FVector(10.f, 4.f, 1.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddStairs(L, TEXT("WH_DockStair"), FVector(WhX - 3.6f, WhY - 10.f, 0.f), FVector(1.f, 0.f, 0.f), 2.2f, 1.2f, 5, ENexusMaterialType::Concrete);

	FNexusSiteDef SiteB;
	SiteB.SiteId = ENexusSiteId::B;
	SiteB.CenterMeters = FVector(WhX, WhY, 0.f);
	SiteB.SizeMeters = FVector(22.f, 16.f, 7.f);
	SiteB.PlantZones = {
		{ ENexusSiteId::B, TEXT("B_Plant_Centre"), FVector(WhX, WhY, 0.f), 1.5f },
		{ ENexusSiteId::B, TEXT("B_Plant_NO"), FVector(WhX - 6.f, WhY + 4.f, 0.f), 1.5f },
		{ ENexusSiteId::B, TEXT("B_Plant_SE"), FVector(WhX + 6.f, WhY - 4.f, 0.f), 1.5f }
	};
	SiteB.EntranceCentersMeters = { FVector(WhX - 11.5f, WhY, 1.f), FVector(WhX - 6.f, WhY + 8.5f, 1.f), FVector(WhX + 6.f, WhY - 8.5f, 1.f) };
	SiteB.DefensivePositionsMeters = { FVector(WhX + 6.f, WhY + 5.f, 0.f), FVector(WhX - 3.f, WhY - 5.f, 0.f) };
	L.Sites.Add(SiteB);

	// --- ZONE CENTRALE (mid): abri + sacs de sable + dépôt carburant ---
	// Abri ouvert (toit sur poteaux + mur nord).
	AddBox(L, TEXT("ZC_ShedRoof"), FVector(0.f, 4.f, 3.5f), FVector(8.f, 6.f, 0.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("ZC_ShedWallN"), FVector(0.f, 6.85f, 1.75f), FVector(8.f, 0.3f, 3.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("ZC_ShedP1"), FVector(-3.7f, 1.3f, 1.7f), FVector(0.35f, 0.35f, 3.4f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("ZC_ShedP2"), FVector(3.7f, 1.3f, 1.7f), FVector(0.35f, 0.35f, 3.4f), ENexusMaterialType::Structural);
	// Sacs de sable (covers bas).
	AddBox(L, TEXT("ZC_Sand1"), FVector(-8.f, -4.f, 0.55f), FVector(6.f, 0.7f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("ZC_Sand2"), FVector(7.f, -6.f, 0.55f), FVector(0.7f, 5.f, 1.1f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	// Dépôt carburant (gros blocs).
	AddBox(L, TEXT("ZC_Fuel1"), FVector(-2.f, -13.f, 1.5f), FVector(3.f, 3.f, 3.f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("ZC_Fuel2"), FVector(2.5f, -13.f, 1.25f), FVector(2.4f, 2.4f, 2.5f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	// Caisse d'appro.
	AddBox(L, TEXT("ZC_Crate"), FVector(-4.f, 8.f, 0.65f), FVector(2.f, 1.5f, 1.3f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);

	// --- TOUR DE GARDE (CT) à (0,20): plateforme h5.6, escalier double volée ---
	AddBox(L, TEXT("TG_Leg1"), FVector(-1.6f, 18.4f, 2.75f), FVector(0.4f, 0.4f, 5.5f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TG_Leg2"), FVector(1.6f, 18.4f, 2.75f), FVector(0.4f, 0.4f, 5.5f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TG_Leg3"), FVector(-1.6f, 21.6f, 2.75f), FVector(0.4f, 0.4f, 5.5f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TG_Leg4"), FVector(1.6f, 21.6f, 2.75f), FVector(0.4f, 0.4f, 5.5f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("TG_Deck"), FVector(0.f, 20.f, 5.6f), FVector(4.4f, 4.4f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("TG_RailN"), FVector(0.f, 22.1f, 6.3f), FVector(4.4f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("TG_RailS"), FVector(0.f, 17.9f, 6.3f), FVector(4.4f, 0.12f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("TG_RailW"), FVector(-2.1f, 20.f, 6.3f), FVector(0.12f, 4.4f, 1.1f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("TG_Canopy"), FVector(0.f, 20.f, 8.4f), FVector(4.8f, 4.8f, 0.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	// Escalier: volée 1 vers +Y jusqu'à palier, volée 2 vers -X jusqu'au deck.
	AddStairs(L, TEXT("TG_Stair1"), FVector(3.4f, 14.2f, 0.f), FVector(0.f, 1.f, 0.f), 1.3f, 2.8f, 10, ENexusMaterialType::Structural);
	AddBox(L, TEXT("TG_Landing"), FVector(3.4f, 18.f, 2.8f), FVector(1.5f, 1.6f, 0.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	AddStairs(L, TEXT("TG_Stair2"), FVector(2.6f, 19.4f, 2.8f), FVector(-1.f, 0.f, 0.f), 1.4f, 2.8f, 10, ENexusMaterialType::Structural);

	// --- CONTENEURS SUD (T): deux rangées décalées, une pile double ---
	// Rangée 1 y=-28.
	AddBox(L, TEXT("CN_R1a"), FVector(-24.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R1b"), FVector(-12.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R1c"), FVector(0.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R1d"), FVector(12.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R1e"), FVector(24.f, -28.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	// Rangée 2 y=-34 (décalée).
	AddBox(L, TEXT("CN_R2a"), FVector(-18.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R2b"), FVector(-6.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R2c"), FVector(6.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_R2d"), FVector(18.f, -34.f, 1.3f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	// Pile double (bloqueur de ligne).
	AddBox(L, TEXT("CN_Stack"), FVector(6.f, -34.f, 3.9f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("CN_Stack2"), FVector(-12.f, -28.f, 3.9f), FVector(6.f, 2.4f, 2.6f), ENexusMaterialType::Structural);
	// Conteneur isolé en travers (jog).
	AddBox(L, TEXT("CN_Jog"), FVector(-28.f, -31.f, 1.3f), FVector(2.4f, 6.f, 2.6f), ENexusMaterialType::Structural);

	// --- BLOQUEURS / BÂTIMENTS PLEINS ---
	AddBox(L, TEXT("BLK_Barracks"), FVector(-46.f, 4.f, 3.f), FVector(8.f, 12.f, 6.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("BLK_East"), FVector(46.f, 8.f, 3.f), FVector(6.f, 12.f, 6.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("BLK_North"), FVector(0.f, 36.f, 1.5f), FVector(16.f, 1.f, 3.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("BLK_HgJersey"), FVector(-30.f, 8.f, 0.6f), FVector(4.f, 0.8f, 1.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Cover);
	// Camions (covers moyens) sur les routes.
	AddBox(L, TEXT("TRK_1"), FVector(-14.f, 14.f, 1.1f), FVector(5.5f, 2.2f, 2.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("TRK_2"), FVector(18.f, 10.f, 1.1f), FVector(5.5f, 2.2f, 2.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("TRK_3"), FVector(14.f, -18.f, 1.1f), FVector(5.5f, 2.2f, 2.2f), ENexusMaterialType::Structural, ENexusBoxUsage::Cover);

	// --- COVERS ---
	L.Covers = {
		{ FVector(-30.f, 12.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.3f },
		{ FVector(-20.f, 2.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(-6.f, -2.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.3f },
		{ FVector(10.f, -4.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(20.f, -14.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.3f },
		{ FVector(-30.f, -22.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(30.f, 4.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(-8.f, 24.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.3f },
		{ FVector(10.f, 30.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.1f },
		{ FVector(38.f, -20.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusCoverType::High, 1.2f }
	};

	// --- PORTES ---
	L.Doors = {
		{ TEXT("Door_HG_E"), FVector(HgX + 12.85f, HgY - 4.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_HG_W"), FVector(HgX - 12.85f, HgY + 4.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_WH_Shutter"), FVector(WhX - 10.85f, WhY, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Destructible },
		{ TEXT("Door_WH_N"), FVector(WhX - 6.f, WhY + 7.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Reinforced },
		{ TEXT("Door_WH_S"), FVector(WhX + 6.f, WhY - 7.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard }
	};

	// --- SPAWNS (attaquants ouest, défenseurs est) ---
	AddSpawn(L, 1, ENexusTeam::Attackers, FVector(-58.f, -24.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 2, ENexusTeam::Attackers, FVector(-58.f, -18.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 3, ENexusTeam::Attackers, FVector(-58.f, -10.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 4, ENexusTeam::Attackers, FVector(-58.f, -2.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 5, ENexusTeam::Attackers, FVector(-58.f, 6.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 6, ENexusTeam::Attackers, FVector(-58.f, 14.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 7, ENexusTeam::Attackers, FVector(-58.f, 22.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 8, ENexusTeam::Attackers, FVector(-62.f, -18.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 9, ENexusTeam::Attackers, FVector(-62.f, -6.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 10, ENexusTeam::Attackers, FVector(-62.f, 6.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 11, ENexusTeam::Attackers, FVector(-62.f, 18.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 12, ENexusTeam::Attackers, FVector(-58.f, -30.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 13, ENexusTeam::Attackers, FVector(-62.f, -30.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 14, ENexusTeam::Attackers, FVector(-58.f, 28.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));
	AddSpawn(L, 15, ENexusTeam::Attackers, FVector(-62.f, 28.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 0.f, 0.f));

	AddSpawn(L, 1, ENexusTeam::Defenders, FVector(58.f, -8.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 2, ENexusTeam::Defenders, FVector(58.f, -2.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 3, ENexusTeam::Defenders, FVector(58.f, 4.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 4, ENexusTeam::Defenders, FVector(58.f, 10.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 5, ENexusTeam::Defenders, FVector(58.f, 16.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 6, ENexusTeam::Defenders, FVector(62.f, -6.f, 0.f), ENexusRouteId::RotateLongBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 7, ENexusTeam::Defenders, FVector(62.f, 2.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 8, ENexusTeam::Defenders, FVector(62.f, 10.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 9, ENexusTeam::Defenders, FVector(62.f, 18.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, 180.f, 0.f));
	AddSpawn(L, 10, ENexusTeam::Defenders, FVector(58.f, 22.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, 180.f, 0.f));

	// --- CALLOUTS (>=15) ---
	AddCallout(L, TEXT("SpawnOuest"), TEXT("Spawn Ouest"), FVector(-58.f, 0.f, 3.f), FVector(9.f, 34.f, 4.f));
	AddCallout(L, TEXT("SpawnEst"), TEXT("Spawn Est"), FVector(60.f, 8.f, 3.f), FVector(8.f, 18.f, 4.f));
	AddCallout(L, TEXT("Hangar"), TEXT("Hangar"), FVector(HgX, HgY, 3.5f), FVector(13.f, 10.f, 4.5f));
	AddCallout(L, TEXT("Mezzanine"), TEXT("Mezzanine"), FVector(HgX, HgY + 7.4f, 4.2f), FVector(11.f, 2.2f, 2.f));
	AddCallout(L, TEXT("Entrepot"), TEXT("Entrepôt"), FVector(WhX, WhY, 3.f), FVector(11.f, 8.f, 3.5f));
	AddCallout(L, TEXT("Quai"), TEXT("Quai"), FVector(WhX + 2.f, WhY - 10.f, 1.5f), FVector(5.f, 2.f, 2.f));
	AddCallout(L, TEXT("ZoneCentrale"), TEXT("Zone Centrale"), FVector(0.f, -2.f, 2.f), FVector(12.f, 12.f, 4.f));
	AddCallout(L, TEXT("Depot"), TEXT("Dépôt"), FVector(0.f, -13.f, 2.f), FVector(5.f, 3.f, 3.5f));
	AddCallout(L, TEXT("TourGarde"), TEXT("Tour de Garde"), FVector(0.f, 20.f, 6.f), FVector(3.f, 3.f, 3.f));
	AddCallout(L, TEXT("Conteneurs"), TEXT("Conteneurs"), FVector(0.f, -31.f, 2.f), FVector(28.f, 5.f, 3.5f));
	AddCallout(L, TEXT("RouteNord"), TEXT("Route Nord"), FVector(-4.f, 30.f, 2.f), FVector(20.f, 5.f, 4.f));
	AddCallout(L, TEXT("RouteSud"), TEXT("Route Sud"), FVector(0.f, -39.f, 2.f), FVector(30.f, 4.f, 4.f));
	AddCallout(L, TEXT("Caserne"), TEXT("Caserne"), FVector(-46.f, 4.f, 3.f), FVector(4.5f, 6.5f, 3.5f));
	AddCallout(L, TEXT("OUTPOST_A_HANGAR"), TEXT("A HANGAR"), FVector(HgX, HgY, 3.5f), FVector(13.f, 10.f, 4.5f));
	AddCallout(L, TEXT("OUTPOST_B_WAREHOUSE"), TEXT("B WAREHOUSE"), FVector(WhX, WhY, 3.f), FVector(11.f, 8.f, 3.5f));
	AddCallout(L, TEXT("OUTPOST_MID_YARD"), TEXT("MID YARD"), FVector(0.f, -2.f, 2.f), FVector(12.f, 12.f, 4.f));
	AddCallout(L, TEXT("OUTPOST_CT_TOWER"), TEXT("CT TOWER"), FVector(0.f, 20.f, 6.f), FVector(3.f, 3.f, 3.f));
	AddCallout(L, TEXT("OUTPOST_T_CONTAINERS"), TEXT("T CONTAINERS"), FVector(0.f, -31.f, 2.f), FVector(28.f, 5.f, 3.5f));

	// --- ROUTES ---
	AddRoute(L, ENexusRouteId::RouteA, TEXT("ROUTE_A"), {
		FVector(-58.f, 14.f, 0.f), FVector(-50.f, 17.5f, 0.f), FVector(-45.f, 20.f, 0.f), FVector(-44.f, 26.f, 0.f),
		FVector(-43.f, 28.f, 0.f), FVector(-36.f, 26.f, 0.f), FVector(-30.f, 22.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RouteB, TEXT("ROUTE_B"), {
		FVector(-58.f, -18.f, 0.f), FVector(-50.f, -24.5f, 0.f), FVector(-38.f, -28.f, 0.f), FVector(-28.f, -31.f, 0.f),
		FVector(-12.f, -31.f, 0.f), FVector(6.f, -31.f, 0.f), FVector(16.f, -22.f, 0.f), FVector(20.f, -8.f, 0.f),
		FVector(32.f, -8.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RouteC, TEXT("ROUTE_C"), {
		FVector(-58.f, -6.f, 0.f), FVector(-50.f, -24.5f, 0.f), FVector(-34.f, -12.f, 0.f), FVector(-16.f, -4.f, 0.f),
		FVector(0.f, -2.f, 0.f), FVector(14.f, 0.f, 0.f), FVector(26.f, -1.f, 0.f), FVector(26.f, -4.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateShortAB, TEXT("ROTATE_SHORT_AB"), {
		FVector(-30.f, 22.f, 0.f), FVector(-30.f, 12.f, 0.f), FVector(-16.f, 4.f, 0.f), FVector(0.f, 0.f, 0.f),
		FVector(14.f, -2.f, 0.f), FVector(20.f, -8.f, 0.f), FVector(32.f, -8.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateShortBA, TEXT("ROTATE_SHORT_BA"), {
		FVector(32.f, -8.f, 0.f), FVector(20.f, -8.f, 0.f), FVector(14.f, -2.f, 0.f), FVector(0.f, 0.f, 0.f),
		FVector(-16.f, 4.f, 0.f), FVector(-30.f, 12.f, 0.f), FVector(-30.f, 22.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateLongAB, TEXT("ROTATE_LONG_AB"), {
		FVector(-30.f, 22.f, 0.f), FVector(-43.f, 16.f, 0.f), FVector(-40.f, -10.f, 0.f), FVector(-32.f, -24.f, 0.f),
		FVector(-12.f, -31.f, 0.f), FVector(12.f, -31.f, 0.f), FVector(24.f, -20.f, 0.f), FVector(34.f, -18.f, 0.f),
		FVector(38.f, -12.f, 0.f), FVector(32.f, -8.f, 0.f)
	});
	AddRoute(L, ENexusRouteId::RotateLongBA, TEXT("ROTATE_LONG_BA"), {
		FVector(32.f, -8.f, 0.f), FVector(38.f, -12.f, 0.f), FVector(34.f, -18.f, 0.f), FVector(24.f, -20.f, 0.f),
		FVector(12.f, -31.f, 0.f), FVector(-12.f, -31.f, 0.f), FVector(-32.f, -24.f, 0.f), FVector(-40.f, -10.f, 0.f),
		FVector(-43.f, 16.f, 0.f), FVector(-30.f, 22.f, 0.f)
	});

	return L;
}
