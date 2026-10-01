#pragma once

#include "CoreMinimal.h"
#include "Maps/NexusVelasqoLayout.h"

/**
 * Shared greybox authoring helpers for all NEXUS maps (Velasqo / Skyline / Outpost).
 * Extracted verbatim from NexusVelasqoLayout.cpp — behavior must stay identical.
 */
namespace NexusLayoutHelpers
{
	inline void AddBox(FNexusVelasqoLayout& L, FName Id, FVector Center, FVector Size, ENexusMaterialType Mat,
		ENexusBoxUsage Usage = ENexusBoxUsage::Structure, bool bDest = false, float HP = 0.f, float Thick = 0.2f)
	{
		FNexusBoxDef B;
		B.Id = Id;
		B.CenterMeters = Center;
		B.SizeMeters = Size;
		B.Material = Mat;
		B.Usage = Usage;
		B.bCanBeDestroyed = bDest;
		B.Health = HP;
		B.ThicknessMeters = Thick;
		L.Boxes.Add(B);
	}

	/** Wall running along X, thin in Y, with a door gap. */
	inline void AddWallXDoor(FNexusVelasqoLayout& L, const FString& Id, float Cx, float Cy, float Cz, float Length, float Height, float Thick,
		float DoorCenterX, float DoorW, float DoorH, ENexusMaterialType Mat, bool bDest = false, float HP = 0.f)
	{
		const float Left = Cx - Length * 0.5f;
		const float Right = Cx + Length * 0.5f;
		const float DoorL = DoorCenterX - DoorW * 0.5f;
		const float DoorR = DoorCenterX + DoorW * 0.5f;
		if (DoorL > Left + 0.1f)
		{
			const float W = DoorL - Left;
			AddBox(L, FName(*(Id + TEXT("_L"))), FVector(Left + W * 0.5f, Cy, Cz + Height * 0.5f), FVector(W, Thick, Height), Mat, ENexusBoxUsage::Structure, bDest, HP, Thick);
		}
		if (Right > DoorR + 0.1f)
		{
			const float W = Right - DoorR;
			AddBox(L, FName(*(Id + TEXT("_R"))), FVector(DoorR + W * 0.5f, Cy, Cz + Height * 0.5f), FVector(W, Thick, Height), Mat, ENexusBoxUsage::Structure, bDest, HP, Thick);
		}
		if (Height > DoorH + 0.05f)
		{
			const float H = Height - DoorH;
			AddBox(L, FName(*(Id + TEXT("_H"))), FVector(DoorCenterX, Cy, Cz + DoorH + H * 0.5f), FVector(DoorW, Thick, H), Mat, ENexusBoxUsage::Structure, bDest, HP, Thick);
		}
	}

	/** Wall running along Y, thin in X, with a door gap. */
	inline void AddWallYDoor(FNexusVelasqoLayout& L, const FString& Id, float Cx, float Cy, float Cz, float Length, float Height, float Thick,
		float DoorCenterY, float DoorW, float DoorH, ENexusMaterialType Mat, bool bDest = false, float HP = 0.f)
	{
		const float Bottom = Cy - Length * 0.5f;
		const float Top = Cy + Length * 0.5f;
		const float DoorL = DoorCenterY - DoorW * 0.5f;
		const float DoorR = DoorCenterY + DoorW * 0.5f;
		if (DoorL > Bottom + 0.1f)
		{
			const float W = DoorL - Bottom;
			AddBox(L, FName(*(Id + TEXT("_B"))), FVector(Cx, Bottom + W * 0.5f, Cz + Height * 0.5f), FVector(Thick, W, Height), Mat, ENexusBoxUsage::Structure, bDest, HP, Thick);
		}
		if (Top > DoorR + 0.1f)
		{
			const float W = Top - DoorR;
			AddBox(L, FName(*(Id + TEXT("_T"))), FVector(Cx, DoorR + W * 0.5f, Cz + Height * 0.5f), FVector(Thick, W, Height), Mat, ENexusBoxUsage::Structure, bDest, HP, Thick);
		}
		if (Height > DoorH + 0.05f)
		{
			const float H = Height - DoorH;
			AddBox(L, FName(*(Id + TEXT("_H"))), FVector(Cx, DoorCenterY, Cz + DoorH + H * 0.5f), FVector(Thick, DoorW, H), Mat, ENexusBoxUsage::Structure, bDest, HP, Thick);
		}
	}

	inline void AddStairs(FNexusVelasqoLayout& L, const FString& Id, FVector StartBottom, FVector RunDir, float Width, float Height, int32 Steps, ENexusMaterialType Mat)
	{
		const float Rise = Height / Steps;
		const float Run = 0.28f;
		const FVector Dir = RunDir.GetSafeNormal();
		for (int32 i = 0; i < Steps; ++i)
		{
			const FVector C = StartBottom + Dir * (Run * (i + 0.5f)) + FVector(0.f, 0.f, Rise * (i + 0.5f));
			AddBox(L, FName(*(Id + FString::Printf(TEXT("_%d"), i))), C, FVector(FMath::Abs(Dir.X) > 0.5f ? Run : Width, FMath::Abs(Dir.Y) > 0.5f ? Run : Width, Rise), Mat, ENexusBoxUsage::Stairs);
		}
	}

	inline void AddSpawn(FNexusVelasqoLayout& L, int32 Id, ENexusTeam Team, FVector Loc, ENexusRouteId Route, FRotator Rot)
	{
		FNexusSpawnDef S;
		S.SpawnID = Id;
		S.Team = Team;
		S.LocationMeters = Loc;
		S.Rotation = Rot;
		S.PreferredRoute = Route;
		S.SafeZoneRadiusMeters = 4.f;
		L.Spawns.Add(S);
	}

	inline void AddCallout(FNexusVelasqoLayout& L, const TCHAR* Id, const TCHAR* Name, FVector C, FVector Ext)
	{
		FNexusCalloutDef D;
		D.CalloutId = Id;
		D.DisplayName = FText::FromString(Name);
		D.CenterMeters = C;
		D.ExtentMeters = Ext;
		L.Callouts.Add(D);
	}

	inline void AddRoute(FNexusVelasqoLayout& L, ENexusRouteId Id, const TCHAR* Name, const TArray<FVector>& Pts)
	{
		FNexusRouteDef R;
		R.RouteId = Id;
		R.DisplayName = Name;
		R.PointsMeters = Pts;
		L.Routes.Add(R);
	}
}
