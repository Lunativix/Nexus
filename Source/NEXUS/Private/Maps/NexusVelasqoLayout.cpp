#include "Maps/NexusVelasqoLayout.h"
#include "Maps/NexusLayoutHelpers.h"

namespace
{
	using namespace NexusLayoutHelpers;

	void AddTwoStoryHouse(FNexusVelasqoLayout& L, const FString& Prefix, FVector Center, FVector Footprint, float FloorH, ENexusMaterialType WallMat,
		float DoorYLocal, bool bRoofAccess)
	{
		const float T = 0.3f;
		const float Sx = Footprint.X;
		const float Sy = Footprint.Y;
		const float TotalH = FloorH * 2.f;
		AddBox(L, FName(*(Prefix + TEXT("_Floor0"))), FVector(Center.X, Center.Y, 0.1f), FVector(Sx, Sy, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
		AddBox(L, FName(*(Prefix + TEXT("_Floor1"))), FVector(Center.X, Center.Y, FloorH), FVector(Sx - 0.4f, Sy - 0.4f, 0.2f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
		if (bRoofAccess)
		{
			AddBox(L, FName(*(Prefix + TEXT("_Roof"))), FVector(Center.X + 1.5f, Center.Y, TotalH), FVector(Sx - 3.2f, Sy - 0.4f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
			AddBox(L, FName(*(Prefix + TEXT("_ParapetN"))), FVector(Center.X, Center.Y + Sy * 0.5f - 0.15f, TotalH + 0.6f), FVector(Sx, 0.3f, 1.2f), ENexusMaterialType::LightStone);
			AddBox(L, FName(*(Prefix + TEXT("_ParapetS"))), FVector(Center.X, Center.Y - Sy * 0.5f + 0.15f, TotalH + 0.6f), FVector(Sx, 0.3f, 1.2f), ENexusMaterialType::LightStone);
			AddBox(L, FName(*(Prefix + TEXT("_ParapetE"))), FVector(Center.X + Sx * 0.5f - 0.15f, Center.Y, TotalH + 0.6f), FVector(0.3f, Sy, 1.2f), ENexusMaterialType::LightStone);
			AddBox(L, FName(*(Prefix + TEXT("_ParapetW"))), FVector(Center.X - Sx * 0.5f + 0.15f, Center.Y, TotalH + 0.6f), FVector(0.3f, Sy, 1.2f), ENexusMaterialType::LightStone);
		}
		else
		{
			AddBox(L, FName(*(Prefix + TEXT("_Roof"))), FVector(Center.X, Center.Y, TotalH), FVector(Sx, Sy, 0.3f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
		}

		AddWallXDoor(L, Prefix + TEXT("_South"), Center.X, Center.Y - Sy * 0.5f + T * 0.5f, 0.f, Sx, TotalH, T, Center.X, 1.0f, 2.1f, WallMat);
		AddWallXDoor(L, Prefix + TEXT("_North"), Center.X, Center.Y + Sy * 0.5f - T * 0.5f, 0.f, Sx, TotalH, T, Center.X + 1.5f, 1.0f, 2.1f, WallMat);
		AddWallYDoor(L, Prefix + TEXT("_West"), Center.X - Sx * 0.5f + T * 0.5f, Center.Y, 0.f, Sy, TotalH, T, Center.Y + DoorYLocal, 1.0f, 2.1f, WallMat);
		AddWallYDoor(L, Prefix + TEXT("_East"), Center.X + Sx * 0.5f - T * 0.5f, Center.Y, 0.f, Sy, TotalH, T, Center.Y, 1.2f, 1.4f, ENexusMaterialType::Plaster, true, 500.f);

		AddStairs(L, Prefix + TEXT("_Stair"), FVector(Center.X - Sx * 0.35f, Center.Y - Sy * 0.25f, 0.f), FVector(0.f, 1.f, 0.f), 1.2f, FloorH, 12, ENexusMaterialType::Wood);
		if (bRoofAccess)
		{
			AddStairs(L, Prefix + TEXT("_RoofStair"), FVector(Center.X - Sx * 0.4f, Center.Y + Sy * 0.15f, FloorH), FVector(0.f, 1.f, 0.f), 1.1f, FloorH, 10, ENexusMaterialType::Wood);
		}

		AddBox(L, FName(*(Prefix + TEXT("_Part"))), FVector(Center.X + 1.2f, Center.Y, FloorH * 0.5f), FVector(0.12f, Sy * 0.45f, FloorH * 0.9f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.12f);
	}
}

FNexusVelasqoLayout FNexusVelasqoLayout::Build()
{
	FNexusVelasqoLayout L;

	// Terrain slab. Top at Z=0. Playable bounds 120x120m.
	AddBox(L, TEXT("Terrain"), FVector(0.f, 0.f, -0.25f), FVector(120.f, 120.f, 0.5f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);

	// Perimeter (structural, not destructible).
	AddBox(L, TEXT("Perim_N"), FVector(0.f, 60.1f, 4.f), FVector(121.f, 0.4f, 8.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_S"), FVector(0.f, -60.1f, 4.f), FVector(121.f, 0.4f, 8.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_E"), FVector(60.1f, 0.f, 4.f), FVector(0.4f, 121.f, 8.f), ENexusMaterialType::Structural);
	AddBox(L, TEXT("Perim_W"), FVector(-60.1f, 0.f, 4.f), FVector(0.4f, 121.f, 8.f), ENexusMaterialType::Structural);

	// Spawn chicanes — block spawn → site LOS.
	AddWallXDoor(L, TEXT("Chicane_S"), 0.f, -46.f, 0.f, 100.f, 4.5f, 0.5f, -20.f, 3.2f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("Chicane_S2"), 0.f, -46.f, 0.f, 100.f, 4.5f, 0.5f, 0.f, 3.2f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("Chicane_S3"), 0.f, -46.f, 0.f, 100.f, 4.5f, 0.5f, 20.f, 3.2f, 2.4f, ENexusMaterialType::Concrete);
	AddBox(L, TEXT("Chicane_SFillL"), FVector(-35.f, -46.f, 2.25f), FVector(30.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("Chicane_SFillR"), FVector(35.f, -46.f, 2.25f), FVector(30.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);

	AddWallYDoor(L, TEXT("Chicane_W"), -48.f, -5.f, 0.f, 70.f, 4.5f, 0.5f, -25.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("Chicane_W2"), -48.f, -5.f, 0.f, 70.f, 4.5f, 0.5f, -15.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("Chicane_W3"), -48.f, 5.f, 0.f, 40.f, 4.5f, 0.5f, 5.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddBox(L, TEXT("Chicane_WFill"), FVector(-48.f, -5.f, 2.25f), FVector(0.5f, 18.f, 4.5f), ENexusMaterialType::Concrete);

	AddWallYDoor(L, TEXT("Chicane_E"), 48.f, 5.f, 0.f, 70.f, 4.5f, 0.5f, -15.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("Chicane_E2"), 48.f, 5.f, 0.f, 70.f, 4.5f, 0.5f, 5.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallYDoor(L, TEXT("Chicane_E3"), 48.f, 15.f, 0.f, 40.f, 4.5f, 0.5f, 15.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);

	AddWallXDoor(L, TEXT("Chicane_N"), 0.f, 36.f, 0.f, 110.f, 4.5f, 0.5f, -36.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("Chicane_N2"), 0.f, 36.f, 0.f, 110.f, 4.5f, 0.5f, -22.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("Chicane_N3"), 0.f, 36.f, 0.f, 110.f, 4.5f, 0.5f, 22.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddWallXDoor(L, TEXT("Chicane_N4"), 0.f, 36.f, 0.f, 110.f, 4.5f, 0.5f, 36.f, 3.0f, 2.4f, ENexusMaterialType::Concrete);
	AddBox(L, TEXT("Chicane_NFill"), FVector(0.f, 36.f, 2.25f), FVector(20.f, 0.5f, 4.5f), ENexusMaterialType::Concrete);

	// --- SITE A PLACE (22,15) 20x18 ---
	AddBox(L, TEXT("A_Floor"), FVector(22.f, 15.f, 0.05f), FVector(20.f, 18.f, 0.1f), ENexusMaterialType::LightStone, ENexusBoxUsage::Floor);
	// Fontaine A1 (22,16) 8x7 h 1.1 + muret 1.0
	AddBox(L, TEXT("A_Fountain"), FVector(22.f, 16.f, 0.55f), FVector(4.5f, 3.8f, 1.1f), ENexusMaterialType::ThickStone, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("A_FountainWallN"), FVector(22.f, 19.4f, 0.5f), FVector(8.f, 0.25f, 1.0f), ENexusMaterialType::LightStone, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("A_FountainWallS"), FVector(22.f, 12.6f, 0.5f), FVector(8.f, 0.25f, 1.0f), ENexusMaterialType::LightStone, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("A_FountainWallE"), FVector(26.f, 16.f, 0.5f), FVector(0.25f, 7.f, 1.0f), ENexusMaterialType::LightStone, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("A_FountainWallW"), FVector(18.f, 16.f, 0.5f), FVector(0.25f, 7.f, 1.0f), ENexusMaterialType::LightStone, ENexusBoxUsage::Cover);
	// Boutique A2 (31,14) 7x8 h 3.2 — rear thick stone (east), laterals plaster destructible
	AddBox(L, TEXT("A_ShopFloor"), FVector(31.f, 14.f, 0.08f), FVector(7.f, 8.f, 0.16f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("A_ShopRear"), FVector(34.35f, 14.f, 1.6f), FVector(0.3f, 8.f, 3.2f), ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("A_ShopRoof"), FVector(31.f, 14.f, 3.25f), FVector(7.f, 8.f, 0.2f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("A_ShopNorth"), FVector(31.f, 17.85f, 1.6f), FVector(6.4f, 0.15f, 3.2f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.15f);
	AddBox(L, TEXT("A_ShopSouth"), FVector(31.f, 10.15f, 1.6f), FVector(6.4f, 0.15f, 3.2f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.15f);
	AddWallYDoor(L, TEXT("A_ShopWest"), 27.6f, 14.f, 0.f, 8.f, 3.2f, 0.15f, 14.f, 1.8f, 2.4f, ENexusMaterialType::Plaster, true, 500.f);
	// Arcade A3 (17,22) 8x4
	AddBox(L, TEXT("A_ArcadeRoof"), FVector(17.f, 22.f, 3.1f), FVector(8.f, 4.f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("A_ArcadeCol1"), FVector(13.4f, 20.3f, 1.5f), FVector(0.4f, 0.4f, 3.f), ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("A_ArcadeCol2"), FVector(20.6f, 20.3f, 1.5f), FVector(0.4f, 0.4f, 3.f), ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("A_ArcadeCol3"), FVector(13.4f, 23.7f, 1.5f), FVector(0.4f, 0.4f, 3.f), ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("A_ArcadeCol4"), FVector(20.6f, 23.7f, 1.5f), FVector(0.4f, 0.4f, 3.f), ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("A_ArcadeBack"), FVector(17.f, 23.9f, 1.6f), FVector(8.f, 0.25f, 3.2f), ENexusMaterialType::LightStone);

	FNexusSiteDef SiteA;
	SiteA.SiteId = ENexusSiteId::A;
	SiteA.CenterMeters = FVector(22.f, 15.f, 0.f);
	SiteA.SizeMeters = FVector(20.f, 18.f, 6.f);
	SiteA.PlantZones = {
		{ ENexusSiteId::A, TEXT("A_Plant_Fountain"), FVector(22.f, 13.2f, 0.f), 1.5f },
		{ ENexusSiteId::A, TEXT("A_Plant_Shop"), FVector(31.f, 14.f, 0.f), 1.5f },
		{ ENexusSiteId::A, TEXT("A_Plant_Arcade"), FVector(17.f, 21.5f, 0.f), 1.5f }
	};
	SiteA.EntranceCentersMeters = { FVector(12.2f, 15.f, 1.f), FVector(22.f, 6.2f, 1.f), FVector(31.5f, 18.f, 1.f), FVector(17.f, 24.5f, 1.f) };
	SiteA.DefensivePositionsMeters = { FVector(22.f, 19.f, 0.f), FVector(31.f, 14.f, 0.f) };
	L.Sites.Add(SiteA);

	// --- SITE B CARAVANSERAI (-18,-17) 23x20 h9 ---
	AddBox(L, TEXT("B_Floor"), FVector(-18.f, -17.f, 0.05f), FVector(23.f, 20.f, 0.1f), ENexusMaterialType::ThickStone, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("B_WallE"), FVector(-6.65f, -17.f, 4.5f), FVector(0.4f, 20.f, 9.f), ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("B_WallW"), FVector(-29.35f, -17.f, 4.5f), FVector(0.4f, 20.f, 9.f), ENexusMaterialType::ThickStone);
	AddWallXDoor(L, TEXT("B_WallS"), -18.f, -26.8f, 0.f, 23.f, 9.f, 0.4f, -18.f, 2.4f, 2.4f, ENexusMaterialType::ThickStone);
	AddWallXDoor(L, TEXT("B_WallN"), -18.f, -7.2f, 0.f, 23.f, 9.f, 0.4f, -12.f, 2.2f, 2.4f, ENexusMaterialType::ThickStone);
	AddWallXDoor(L, TEXT("B_WallN2"), -18.f, -7.2f, 0.f, 23.f, 9.f, 0.4f, -24.f, 2.2f, 2.4f, ENexusMaterialType::ThickStone);
	AddBox(L, TEXT("B_Roof"), FVector(-18.f, -20.f, 9.1f), FVector(22.2f, 14.f, 0.25f), ENexusMaterialType::Structural, ENexusBoxUsage::Floor);
	// Cour B1 (-18,-15) 12x10
	AddBox(L, TEXT("B_CourCover"), FVector(-18.f, -15.f, 0.45f), FVector(2.2f, 1.6f, 0.9f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	// Entrepôt B2 (-26,-22) 8x7
	AddBox(L, TEXT("B_WhFloor"), FVector(-26.f, -22.f, 0.1f), FVector(8.f, 7.f, 0.2f), ENexusMaterialType::Concrete, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("B_WhWallW"), FVector(-29.8f, -22.f, 1.75f), FVector(0.25f, 7.f, 3.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("B_WhWallS"), FVector(-26.f, -25.35f, 1.75f), FVector(8.f, 0.25f, 3.5f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("B_WhWallE"), FVector(-22.2f, -22.f, 1.75f), FVector(0.12f, 6.5f, 3.5f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 1200.f, 0.2f);
	AddWallXDoor(L, TEXT("B_WhNorth"), -26.f, -18.6f, 0.f, 8.f, 3.5f, 0.2f, -26.f, 1.8f, 2.4f, ENexusMaterialType::Wood, true, 500.f);
	AddBox(L, TEXT("B_WhRoof"), FVector(-26.f, -22.f, 3.6f), FVector(8.f, 7.f, 0.2f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
	// Galerie B3 (-18,-10,6) 18x3
	AddBox(L, TEXT("B_GalFloor"), FVector(-18.f, -10.f, 6.f), FVector(18.f, 3.f, 0.25f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("B_GalRailN"), FVector(-18.f, -8.6f, 6.6f), FVector(18.f, 0.12f, 1.0f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("B_GalRailS"), FVector(-18.f, -11.4f, 6.6f), FVector(18.f, 0.12f, 1.0f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("B_GalRoof"), FVector(-18.f, -10.f, 8.6f), FVector(18.f, 3.2f, 0.2f), ENexusMaterialType::ThickStone, ENexusBoxUsage::Floor);
	AddStairs(L, TEXT("B_GalStairs"), FVector(-27.5f, -10.f, 0.f), FVector(1.f, 0.f, 0.f), 1.4f, 6.f, 22, ENexusMaterialType::Wood);

	FNexusSiteDef SiteB;
	SiteB.SiteId = ENexusSiteId::B;
	SiteB.CenterMeters = FVector(-18.f, -17.f, 0.f);
	SiteB.SizeMeters = FVector(23.f, 20.f, 9.f);
	SiteB.PlantZones = {
		{ ENexusSiteId::B, TEXT("B_Plant_Cour"), FVector(-18.f, -15.f, 0.f), 1.5f },
		{ ENexusSiteId::B, TEXT("B_Plant_Wh"), FVector(-26.f, -22.f, 0.f), 1.5f },
		{ ENexusSiteId::B, TEXT("B_Plant_UnderGal"), FVector(-18.f, -11.5f, 0.f), 1.5f }
	};
	SiteB.EntranceCentersMeters = { FVector(-12.f, -7.2f, 1.f), FVector(-24.f, -7.2f, 1.f), FVector(-18.f, -26.8f, 1.f) };
	SiteB.DefensivePositionsMeters = { FVector(-26.f, -22.f, 0.f), FVector(-18.f, -10.f, 6.f) };
	L.Sites.Add(SiteB);

	// --- MARCHÉ (-10,10) 22x12 h 3.5 ---
	AddBox(L, TEXT("Mkt_Roof"), FVector(-10.f, 10.f, 3.5f), FVector(22.f, 12.f, 0.15f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
	AddBox(L, TEXT("Mkt_Post1"), FVector(-19.f, 5.2f, 1.75f), FVector(0.3f, 0.3f, 3.5f), ENexusMaterialType::Wood);
	AddBox(L, TEXT("Mkt_Post2"), FVector(-1.f, 5.2f, 1.75f), FVector(0.3f, 0.3f, 3.5f), ENexusMaterialType::Wood);
	AddBox(L, TEXT("Mkt_Post3"), FVector(-19.f, 14.8f, 1.75f), FVector(0.3f, 0.3f, 3.5f), ENexusMaterialType::Wood);
	AddBox(L, TEXT("Mkt_Post4"), FVector(-1.f, 14.8f, 1.75f), FVector(0.3f, 0.3f, 3.5f), ENexusMaterialType::Wood);
	AddBox(L, TEXT("Mkt_Post5"), FVector(-10.f, 10.f, 1.75f), FVector(0.3f, 0.3f, 3.5f), ENexusMaterialType::Wood);
	AddBox(L, TEXT("Mkt_Stand1"), FVector(-16.f, 8.f, 0.45f), FVector(2.4f, 1.1f, 0.9f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("Mkt_Stand2"), FVector(-10.f, 12.5f, 0.45f), FVector(2.4f, 1.1f, 0.9f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("Mkt_Stand3"), FVector(-5.f, 8.f, 0.45f), FVector(2.4f, 1.1f, 0.9f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);
	AddBox(L, TEXT("Mkt_Part"), FVector(-7.f, 10.f, 1.2f), FVector(0.1f, 3.5f, 2.4f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.1f);
	AddBox(L, TEXT("Mkt_Part2"), FVector(-14.f, 11.5f, 1.0f), FVector(3.0f, 0.1f, 2.0f), ENexusMaterialType::Wood, ENexusBoxUsage::Destructible, true, 500.f, 0.1f);

	// --- VIEUX QUARTIER (0,0) 30x20 ---
	AddBox(L, TEXT("VQ_House1"), FVector(8.f, -4.f, 1.6f), FVector(6.f, 5.f, 3.2f), ENexusMaterialType::LightStone);
	AddWallYDoor(L, TEXT("VQ_House1W"), 5.1f, -4.f, 0.f, 5.f, 3.2f, 0.25f, -4.f, 1.0f, 2.1f, ENexusMaterialType::LightStone);
	AddBox(L, TEXT("VQ_House1Inner"), FVector(8.f, -4.f, 1.5f), FVector(0.1f, 2.4f, 2.8f), ENexusMaterialType::Plaster, ENexusBoxUsage::Destructible, true, 500.f, 0.1f);
	AddBox(L, TEXT("VQ_Balcony"), FVector(8.f, -1.1f, 3.4f), FVector(3.5f, 1.4f, 0.15f), ENexusMaterialType::Wood, ENexusBoxUsage::Floor);
	AddStairs(L, TEXT("VQ_Stairs"), FVector(5.6f, -1.4f, 0.f), FVector(0.f, 1.f, 0.f), 1.1f, 3.2f, 12, ENexusMaterialType::Wood);
	AddBox(L, TEXT("VQ_House2"), FVector(4.f, 6.f, 1.5f), FVector(5.f, 5.f, 3.0f), ENexusMaterialType::LightStone);
	AddWallXDoor(L, TEXT("VQ_House2S"), 4.f, 3.6f, 0.f, 5.f, 3.0f, 0.25f, 4.f, 1.0f, 2.1f, ENexusMaterialType::LightStone);
	AddBox(L, TEXT("VQ_Narrow1"), FVector(1.2f, 0.f, 2.f), FVector(0.4f, 8.f, 4.f), ENexusMaterialType::LightStone);
	AddBox(L, TEXT("VQ_Narrow2"), FVector(-1.5f, -6.f, 2.f), FVector(8.f, 0.4f, 4.f), ENexusMaterialType::LightStone);

	// --- BLUE HOUSE (-8,2) 12x9 two levels + roof ---
	AddTwoStoryHouse(L, TEXT("BH"), FVector(-8.f, 2.f, 0.f), FVector(12.f, 9.f, 0.f), 3.0f, ENexusMaterialType::LightStone, 0.f, true);

	// --- EAST HOUSE (38,24) 14x12 two levels ---
	AddTwoStoryHouse(L, TEXT("EH"), FVector(38.f, 24.f, 0.f), FVector(14.f, 12.f, 0.f), 3.0f, ENexusMaterialType::LightStone, 1.f, true);
	AddBox(L, TEXT("EH_WindowCover"), FVector(31.2f, 24.f, 4.4f), FVector(0.2f, 2.5f, 1.2f), ENexusMaterialType::Wood, ENexusBoxUsage::Cover);

	// --- RUELLES Y -35 to -15, width ~2.2, three character routes ---
	const float AlleyW = 2.2f;
	auto AlleyNS = [&](const FString& Id, float X)
	{
		AddBox(L, FName(*(Id + TEXT("_W"))), FVector(X - AlleyW * 0.5f - 0.2f, -25.f, 2.f), FVector(0.4f, 20.f, 4.f), ENexusMaterialType::LightStone);
		AddBox(L, FName(*(Id + TEXT("_E"))), FVector(X + AlleyW * 0.5f + 0.2f, -25.f, 2.f), FVector(0.4f, 20.f, 4.f), ENexusMaterialType::LightStone);
	};
	auto AlleyEW = [&](const FString& Id, float Y, float X0, float X1)
	{
		const float Len = X1 - X0;
		const float Cx = (X0 + X1) * 0.5f;
		AddBox(L, FName(*(Id + TEXT("_N"))), FVector(Cx, Y + AlleyW * 0.5f + 0.2f, 2.f), FVector(Len, 0.4f, 4.f), ENexusMaterialType::LightStone);
		AddBox(L, FName(*(Id + TEXT("_S"))), FVector(Cx, Y - AlleyW * 0.5f - 0.2f, 2.f), FVector(Len, 0.4f, 4.f), ENexusMaterialType::LightStone);
	};
	AlleyEW(TEXT("Alley_Main"), -25.f, -40.f, 40.f);
	AlleyEW(TEXT("Alley_South"), -32.f, -40.f, 40.f);
	AlleyEW(TEXT("Alley_North"), -18.f, -40.f, 12.f);
	AlleyNS(TEXT("Alley_NS1"), -30.f);
	AlleyNS(TEXT("Alley_NS2"), -10.f);
	AlleyNS(TEXT("Alley_NS3"), 10.f);
	AlleyNS(TEXT("Alley_NS4"), 28.f);
	// Break LOS down long alleys with jogs
	AddBox(L, TEXT("AlleyJog1"), FVector(-20.f, -25.f, 1.5f), FVector(1.6f, 0.4f, 3.f), ENexusMaterialType::LightStone);
	AddBox(L, TEXT("AlleyJog2"), FVector(5.f, -32.f, 1.5f), FVector(1.6f, 0.4f, 3.f), ENexusMaterialType::LightStone);
	AddBox(L, TEXT("AlleyJog3"), FVector(-5.f, -18.f, 1.5f), FVector(0.4f, 1.6f, 3.f), ENexusMaterialType::LightStone);

	// Extra LOS blockers between market and long sightlines, east house vs B
	AddBox(L, TEXT("Block_MidN"), FVector(10.f, 28.f, 2.5f), FVector(8.f, 0.5f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("Block_MidE"), FVector(28.f, 4.f, 2.5f), FVector(0.5f, 10.f, 5.f), ENexusMaterialType::Concrete);
	AddBox(L, TEXT("Block_WestB"), FVector(-34.f, -8.f, 2.5f), FVector(6.f, 0.5f, 5.f), ENexusMaterialType::Concrete);

	// Covers (HIGH rare)
	L.Covers = {
		{ FVector(22.f, 10.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.4f },
		{ FVector(15.f, 15.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(26.f, 20.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.3f },
		{ FVector(-18.f, -20.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.5f },
		{ FVector(-14.f, -12.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.2f },
		{ FVector(-10.f, 7.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.1f },
		{ FVector(0.f, 8.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.2f },
		{ FVector(38.f, 18.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Medium, 1.4f },
		{ FVector(-8.f, -2.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::Low, 1.0f },
		{ FVector(-22.f, -8.f, 0.f), FRotator::ZeroRotator, ENexusCoverType::High, 1.2f }
	};

	// Doors
	L.Doors = {
		{ TEXT("Door_BH_S"), FVector(-8.f, -2.35f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_BH_N"), FVector(-6.5f, 6.35f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_EH_S"), FVector(38.f, 18.15f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_EH_N"), FVector(39.5f, 29.85f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_Shop"), FVector(27.6f, 14.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Double },
		{ TEXT("Door_Wh"), FVector(-26.f, -18.6f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Destructible },
		{ TEXT("Door_B_N"), FVector(-12.f, -7.2f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Double },
		{ TEXT("Door_B_S"), FVector(-18.f, -26.8f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Reinforced },
		{ TEXT("Door_VQ1"), FVector(5.1f, -4.f, 0.f), FRotator(0.f, 90.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_VQ2"), FVector(4.f, 3.6f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard },
		{ TEXT("Door_Arcade"), FVector(17.f, 20.1f, 0.f), FRotator(0.f, 0.f, 0.f), ENexusDoorType::Standard }
	};

	auto Atk = [&](int32 Id, FVector Loc, ENexusRouteId Route, FRotator Rot)
	{
		FNexusSpawnDef S;
		S.SpawnID = Id;
		S.Team = ENexusTeam::Attackers;
		S.LocationMeters = Loc;
		S.Rotation = Rot;
		S.PreferredRoute = Route;
		S.SafeZoneRadiusMeters = 4.f;
		L.Spawns.Add(S);
	};
	auto Def = [&](int32 Id, FVector Loc, ENexusRouteId Route, FRotator Rot)
	{
		FNexusSpawnDef S;
		S.SpawnID = Id;
		S.Team = ENexusTeam::Defenders;
		S.LocationMeters = Loc;
		S.Rotation = Rot;
		S.PreferredRoute = Route;
		S.SafeZoneRadiusMeters = 4.f;
		L.Spawns.Add(S);
	};

	Atk(1, FVector(-20.f, -52.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 90.f, 0.f));
	Atk(2, FVector(-10.f, -52.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 90.f, 0.f));
	Atk(3, FVector(0.f, -52.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 90.f, 0.f));
	Atk(4, FVector(10.f, -52.f, 0.f), ENexusRouteId::RouteA, FRotator(0.f, 90.f, 0.f));
	Atk(5, FVector(20.f, -52.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 90.f, 0.f));
	Atk(6, FVector(-53.f, -25.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	Atk(7, FVector(-53.f, -15.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	Atk(8, FVector(-53.f, -5.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	Atk(9, FVector(-53.f, 5.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	Atk(10, FVector(-53.f, 15.f, 0.f), ENexusRouteId::RouteB, FRotator(0.f, 0.f, 0.f));
	Atk(11, FVector(53.f, -15.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 180.f, 0.f));
	Atk(12, FVector(53.f, -5.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 180.f, 0.f));
	Atk(13, FVector(53.f, 5.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 180.f, 0.f));
	Atk(14, FVector(53.f, 15.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 180.f, 0.f));
	Atk(15, FVector(53.f, 25.f, 0.f), ENexusRouteId::RouteC, FRotator(0.f, 180.f, 0.f));

	Def(1, FVector(-43.f, 42.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, -90.f, 0.f));
	Def(2, FVector(-36.f, 42.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, -90.f, 0.f));
	Def(3, FVector(-29.f, 42.f, 0.f), ENexusRouteId::RotateLongBA, FRotator(0.f, -90.f, 0.f));
	Def(4, FVector(-22.f, 42.f, 0.f), ENexusRouteId::RotateShortBA, FRotator(0.f, -90.f, 0.f));
	Def(5, FVector(-15.f, 42.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, -90.f, 0.f));
	Def(6, FVector(15.f, 42.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, -90.f, 0.f));
	Def(7, FVector(22.f, 42.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, -90.f, 0.f));
	Def(8, FVector(29.f, 42.f, 0.f), ENexusRouteId::RotateLongAB, FRotator(0.f, -90.f, 0.f));
	Def(9, FVector(36.f, 42.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, -90.f, 0.f));
	Def(10, FVector(43.f, 42.f, 0.f), ENexusRouteId::RotateShortAB, FRotator(0.f, -90.f, 0.f));

	auto Call = [&](const TCHAR* Id, const TCHAR* Name, FVector C, FVector Ext)
	{
		FNexusCalloutDef D;
		D.CalloutId = Id;
		D.DisplayName = FText::FromString(Name);
		D.CenterMeters = C;
		D.ExtentMeters = Ext;
		L.Callouts.Add(D);
	};
	Call(TEXT("SpawnNord"), TEXT("Spawn Nord"), FVector(0.f, 48.f, 3.f), FVector(50.f, 10.f, 4.f));
	Call(TEXT("SpawnSud"), TEXT("Spawn Sud"), FVector(0.f, -54.f, 3.f), FVector(50.f, 8.f, 4.f));
	Call(TEXT("Marche"), TEXT("Marché"), FVector(-10.f, 10.f, 2.f), FVector(11.f, 6.f, 4.f));
	Call(TEXT("Rue"), TEXT("Rue"), FVector(10.f, 15.f, 2.f), FVector(8.f, 4.f, 4.f));
	Call(TEXT("Place"), TEXT("Place"), FVector(22.f, 15.f, 2.f), FVector(10.f, 9.f, 4.f));
	Call(TEXT("Fontaine"), TEXT("Fontaine"), FVector(22.f, 16.f, 2.f), FVector(4.f, 3.5f, 3.f));
	Call(TEXT("Boutique"), TEXT("Boutique"), FVector(31.f, 14.f, 2.f), FVector(3.5f, 4.f, 3.f));
	Call(TEXT("MaisonBleue"), TEXT("Maison Bleue"), FVector(-8.f, 2.f, 4.f), FVector(6.f, 4.5f, 5.f));
	Call(TEXT("MaisonEst"), TEXT("Maison Est"), FVector(38.f, 24.f, 4.f), FVector(7.f, 6.f, 5.f));
	Call(TEXT("VieuxQuartier"), TEXT("Vieux Quartier"), FVector(0.f, 0.f, 3.f), FVector(15.f, 10.f, 4.f));
	Call(TEXT("Caravanserail"), TEXT("Caravansérail"), FVector(-18.f, -17.f, 4.f), FVector(11.5f, 10.f, 6.f));
	Call(TEXT("Galerie"), TEXT("Galerie"), FVector(-18.f, -10.f, 7.f), FVector(9.f, 1.5f, 2.f));
	Call(TEXT("Entrepot"), TEXT("Entrepôt"), FVector(-26.f, -22.f, 2.f), FVector(4.f, 3.5f, 3.f));
	Call(TEXT("Ruelles"), TEXT("Ruelles"), FVector(0.f, -25.f, 2.f), FVector(45.f, 10.f, 4.f));
	Call(TEXT("Toits"), TEXT("Toits"), FVector(15.f, 12.f, 8.f), FVector(40.f, 30.f, 4.f));
	Call(TEXT("Mosquee"), TEXT("Mosquée"), FVector(8.f, 32.f, 6.f), FVector(8.f, 8.f, 8.f));
	Call(TEXT("VELASQO_MOSQUE"), TEXT("MOSQUE"), FVector(8.f, 32.f, 6.f), FVector(8.f, 8.f, 8.f));
	Call(TEXT("VELASQO_MARKET"), TEXT("MARKET"), FVector(-10.f, 10.f, 2.f), FVector(11.f, 6.f, 4.f));
	Call(TEXT("VELASQO_OLD_QUARTER"), TEXT("OLD QUARTER"), FVector(0.f, 0.f, 3.f), FVector(15.f, 10.f, 4.f));
	Call(TEXT("VELASQO_BLUE_HOUSE"), TEXT("BLUE HOUSE"), FVector(-8.f, 2.f, 4.f), FVector(6.f, 4.5f, 5.f));
	Call(TEXT("VELASQO_EAST_HOUSE"), TEXT("EAST HOUSE"), FVector(38.f, 24.f, 4.f), FVector(7.f, 6.f, 5.f));
	Call(TEXT("VELASQO_PLAZA"), TEXT("PLAZA"), FVector(22.f, 15.f, 2.f), FVector(10.f, 9.f, 4.f));
	Call(TEXT("VELASQO_CARAVANSERAI"), TEXT("CARAVANSERAI"), FVector(-18.f, -17.f, 4.f), FVector(11.5f, 10.f, 6.f));
	Call(TEXT("VELASQO_WAREHOUSE"), TEXT("WAREHOUSE"), FVector(-26.f, -22.f, 2.f), FVector(4.f, 3.5f, 3.f));
	Call(TEXT("VELASQO_GALLERY"), TEXT("GALLERY"), FVector(-18.f, -10.f, 7.f), FVector(9.f, 1.5f, 2.f));
	Call(TEXT("VELASQO_SOUTH_ALLEYS"), TEXT("SOUTH ALLEYS"), FVector(0.f, -25.f, 2.f), FVector(45.f, 10.f, 4.f));
	Call(TEXT("VELASQO_ROOFTOPS"), TEXT("ROOFTOPS"), FVector(15.f, 12.f, 8.f), FVector(40.f, 30.f, 4.f));

	auto Route = [&](ENexusRouteId Id, const TCHAR* Name, const TArray<FVector>& Pts)
	{
		FNexusRouteDef R;
		R.RouteId = Id;
		R.DisplayName = Name;
		R.PointsMeters = Pts;
		L.Routes.Add(R);
	};

	Route(ENexusRouteId::RouteA, TEXT("ROUTE_A"), {
		FVector(0.f, -52.f, 0.f), FVector(0.f, -44.f, 0.f), FVector(0.f, -25.f, 0.f), FVector(0.f, 0.f, 0.f),
		FVector(-4.f, 10.f, 0.f), FVector(12.f, 14.f, 0.f), FVector(22.f, 15.f, 0.f)
	});
	Route(ENexusRouteId::RouteB, TEXT("ROUTE_B"), {
		FVector(-53.f, -15.f, 0.f), FVector(-46.f, -15.f, 0.f), FVector(-40.f, -25.f, 0.f), FVector(-30.f, -25.f, 0.f),
		FVector(-30.f, -18.f, 0.f), FVector(-24.f, -10.f, 0.f), FVector(-18.f, -12.f, 0.f)
	});
	Route(ENexusRouteId::RouteC, TEXT("ROUTE_C"), {
		FVector(53.f, 15.f, 0.f), FVector(46.f, 15.f, 0.f), FVector(42.f, 22.f, 0.f), FVector(38.f, 24.f, 3.f),
		FVector(32.f, 22.f, 0.f), FVector(28.f, 16.f, 0.f), FVector(22.f, 15.f, 0.f)
	});
	Route(ENexusRouteId::RotateShortAB, TEXT("ROTATE_SHORT_AB"), {
		FVector(22.f, 15.f, 0.f), FVector(12.f, 14.f, 0.f), FVector(-8.f, 10.f, 0.f), FVector(-8.f, 2.f, 0.f),
		FVector(-10.f, -8.f, 0.f), FVector(-22.f, -8.f, 0.f), FVector(-28.f, -12.f, 0.f), FVector(-18.f, -15.f, 0.f)
	});
	Route(ENexusRouteId::RotateShortBA, TEXT("ROTATE_SHORT_BA"), {
		FVector(-18.f, -15.f, 0.f), FVector(-28.f, -12.f, 0.f), FVector(-22.f, -8.f, 0.f), FVector(-10.f, -8.f, 0.f),
		FVector(-8.f, 2.f, 0.f), FVector(-8.f, 10.f, 0.f), FVector(12.f, 14.f, 0.f), FVector(22.f, 15.f, 0.f)
	});
	Route(ENexusRouteId::RotateLongAB, TEXT("ROTATE_LONG_AB"), {
		FVector(22.f, 15.f, 0.f), FVector(32.f, 18.f, 0.f), FVector(42.f, 12.f, 0.f), FVector(42.f, -8.f, 0.f),
		FVector(38.f, -25.f, 0.f), FVector(20.f, -32.f, 0.f), FVector(0.f, -32.f, 0.f), FVector(-16.f, -28.f, 0.f),
		FVector(-18.f, -17.f, 0.f)
	});
	Route(ENexusRouteId::RotateLongBA, TEXT("ROTATE_LONG_BA"), {
		FVector(-18.f, -17.f, 0.f), FVector(-16.f, -28.f, 0.f), FVector(0.f, -32.f, 0.f), FVector(20.f, -32.f, 0.f),
		FVector(38.f, -25.f, 0.f), FVector(42.f, -8.f, 0.f), FVector(42.f, 12.f, 0.f), FVector(32.f, 18.f, 0.f),
		FVector(22.f, 15.f, 0.f)
	});

	return L;
}
