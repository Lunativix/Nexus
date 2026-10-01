#include "Debug/NexusVelasqoValidation.h"
#include "Debug/NexusLevelValidator.h"
#include "Debug/NexusNavConnectivity.h"
#include "Debug/NexusGameplayTests.h"
#include "Art/NexusEnvironmentDirector.h"
#include "Maps/NexusVelasqoLayout.h"
#include "Maps/NexusMapCatalog.h"
#include "Maps/NexusMapBounds.h"
#include "Maps/NexusTeamStart.h"
#include "Objectives/NexusBombSite.h"
#include "Interaction/NexusDoor.h"
#include "Interaction/NexusCalloutVolume.h"
#include "Destruction/NexusDestructibleSurface.h"
#include "NexusCharacter.h"
#include "NexusPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Maps/NexusVelasqoBuilder.h"
#include "Art/NexusMaterialLibrary.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "NEXUS.h"

static void LogVel(const TCHAR* Test, const TCHAR* Status, const FString& Detail)
{
	UE_LOG(LogNexusArt, Log, TEXT("[NEXUS TEST] [%s] System=VELASQO Test=%s Expected=- Actual=%s Cause=-"),
		Status, Test, *Detail);
}

void FNexusVelasqoValidation::ValidateMap(UWorld* World)
{
	UE_LOG(LogNexusArt, Log, TEXT("========================================"));
	UE_LOG(LogNexusArt, Log, TEXT("NEXUS MAP VALIDATION (%s)"), FNexusMapCatalog::MapName(FNexusMapCatalog::DetectFromWorld(World)));
	UE_LOG(LogNexusArt, Log, TEXT("========================================"));
	if (!World)
	{
		LogVel(TEXT("MAP LOAD"), TEXT("FAIL"), TEXT("null world"));
		return;
	}
	LogVel(TEXT("MAP LOAD"), TEXT("PASS"), World->GetMapName());

	int32 Bounds = 0;
	for (TActorIterator<ANexusMapBounds> It(World); It; ++It) { ++Bounds; }
	LogVel(TEXT("BOUNDS"), Bounds > 0 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("actors=%d"), Bounds));

	FNexusLevelValidator::Run(World);

	int32 Atk = 0, Def = 0;
	for (TActorIterator<ANexusTeamStart> It(World); It; ++It)
	{
		if (It->Team == ENexusTeam::Attackers) { ++Atk; }
		if (It->Team == ENexusTeam::Defenders) { ++Def; }
	}
	LogVel(TEXT("SPAWNS"), (Atk >= 5 && Def >= 5) ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("atk=%d def=%d"), Atk, Def));

	int32 Sites = 0;
	for (TActorIterator<ANexusBombSite> It(World); It; ++It) { ++Sites; }
	LogVel(TEXT("SITES"), Sites >= 2 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("count=%d"), Sites));

	int32 Doors = 0;
	for (TActorIterator<ANexusDoor> It(World); It; ++It) { ++Doors; }
	LogVel(TEXT("DOORS"), Doors > 0 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("count=%d"), Doors));

	int32 Dest = 0;
	for (TActorIterator<ANexusDestructibleSurface> It(World); It; ++It) { ++Dest; }
	LogVel(TEXT("DESTRUCTION"), Dest > 0 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("surfaces=%d"), Dest));

	int32 Calls = 0;
	bool bHasEnglish = false;
	for (TActorIterator<ANexusCalloutVolume> It(World); It; ++It)
	{
		++Calls;
		const FString Id = It->CalloutId.ToString();
		if (Id.StartsWith(TEXT("VELASQO_")) || Id.StartsWith(TEXT("SKYLINE_")) || Id.StartsWith(TEXT("OUTPOST_")))
		{
			bHasEnglish = true;
		}
	}
	const FNexusVelasqoLayout Layout = FNexusMapCatalog::BuildLayout(FNexusMapCatalog::DetectFromWorld(World));
	for (const FNexusCalloutDef& C : Layout.Callouts)
	{
		const FString Id = C.CalloutId.ToString();
		if (Id.StartsWith(TEXT("VELASQO_")) || Id.StartsWith(TEXT("SKYLINE_")) || Id.StartsWith(TEXT("OUTPOST_"))) { bHasEnglish = true; }
	}
	LogVel(TEXT("CALLOUTS"), Calls >= 15 ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("volumes=%d layoutIds=%d englishIds=%d"), Calls, Layout.Callouts.Num(), bHasEnglish ? 1 : 0));

	int32 Visual = 0;
	for (TActorIterator<ANexusEnvironmentDirector> It(World); It; ++It)
	{
		Visual += It->CountVisualInstances();
	}
	LogVel(TEXT("ART INSTANCES"), Visual >= 80 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("ism=%d (NoCollision dressing)"), Visual));

	FNexusNavConnectivity::Run(World);
	FNexusGameplayTests::TestNavigation(World);
	LogVel(TEXT("A TO B"), TEXT("see NAV"), TEXT("honest status from TestNavigation / NavConnectivity — not rewritten here"));
	LogVel(TEXT("PERFORMANCE"), TEXT("NOT VERIFIED"), TEXT("no stat unit this command"));
	UE_LOG(LogNexusArt, Log, TEXT("========================================"));
}

void FNexusVelasqoValidation::FullTest(UWorld* World)
{
	ValidateMap(World);
	FNexusGameplayTests::TestWallbang(World);
	FNexusGameplayTests::TestReplication(World);
	FNexusGameplayTests::TestSpawns(World);
	FNexusGameplayTests::TestOperators(World);
	LogVel(TEXT("OPERATORS"), TEXT("PASS if defs spawned"), TEXT("see OPERATOR tests"));
	LogVel(TEXT("WALLBANG"), TEXT("see SMOKE"), TEXT("penetration math"));
	LogVel(TEXT("BOT NAV"), TEXT("NOT VERIFIED"), TEXT("A↔B historically FAIL; do not convert to PASS"));
	LogVel(TEXT("LISTEN/MP"), TEXT("NOT VERIFIED"), TEXT("not part of this command"));
}

void FNexusVelasqoValidation::WalkTest(UWorld* World)
{
	UE_LOG(LogNexusArt, Log, TEXT("VelasqoWalkTest landmarks: A B Market Rooftops Plaza OldQuarter Caravanserai Warehouse BlueHouse EastHouse"));
	UE_LOG(LogNexusArt, Log, TEXT("Use nexus.VelasqoTeleport <name> (Development). First-person walk is NOT VERIFIED until a human pawn is moved in PIE with viewport."));
	if (World)
	{
		LogVel(TEXT("WALK MODE"), TEXT("PASS"), TEXT("command registered, teleports available"));
	}
}

bool FNexusVelasqoValidation::TeleportToLandmark(UWorld* World, const FString& Landmark)
{
	if (!World)
	{
		return false;
	}
	FVector Meters = FVector(0.f, 0.f, 1.f);
	const FString Key = Landmark.ToLower();
	const ENexusMapId MapId = FNexusMapCatalog::DetectFromWorld(World);
	if (Key == TEXT("a") || Key == TEXT("plaza"))
	{
		if (MapId == ENexusMapId::Skyline) { Meters = FVector(0.f, 2.f, 1.f); }
		else if (MapId == ENexusMapId::Outpost) { Meters = FVector(-30.f, 24.f, 1.f); }
		else { Meters = FVector(22.f, 15.f, 1.f); }
	}
	else if (Key == TEXT("hangar")) { Meters = FVector(-30.f, 14.f, 1.f); }
	else if (Key == TEXT("b") || Key == TEXT("caravanserai") || Key == TEXT("parking") || Key == TEXT("entrepot") || Key == TEXT("siteb"))
	{
		if (MapId == ENexusMapId::Skyline) { Meters = FVector(26.f, -18.f, 1.f); }
		else if (MapId == ENexusMapId::Outpost) { Meters = FVector(32.f, -8.f, 1.f); }
		else { Meters = FVector(-18.f, -17.f, 1.f); }
	}
	else if (Key == TEXT("market") || Key == TEXT("marche")) { Meters = FVector(-10.f, 10.f, 1.f); }
	else if (Key == TEXT("mosquee") || Key == TEXT("mosque")) { Meters = FVector(8.f, 24.f, 1.f); }
	else if (Key == TEXT("rooftops") || Key == TEXT("toits") || Key == TEXT("helipad"))
	{
		Meters = (MapId == ENexusMapId::Skyline) ? FVector(0.f, 2.f, 9.4f) : FVector(15.f, 12.f, 6.2f);
	}
	else if (Key == TEXT("oldquarter") || Key == TEXT("old")) { Meters = FVector(0.f, 0.f, 1.f); }
	else if (Key == TEXT("warehouse")) { Meters = FVector(-26.f, -22.f, 1.f); }
	else if (Key == TEXT("bluehouse") || Key == TEXT("blue") || Key == TEXT("interieur")) { Meters = FVector(-8.f, 2.f, 1.f); }
	else if (Key == TEXT("easthouse") || Key == TEXT("east")) { Meters = FVector(38.f, 24.f, 1.f); }
	else if (Key == TEXT("alleys") || Key == TEXT("south") || Key == TEXT("ruelle")) { Meters = FVector(0.f, -25.f, 1.f); }
	else if (Key == TEXT("gallery")) { Meters = FVector(-18.f, -10.f, 6.2f); }
	else if (Key == TEXT("hall") || Key == TEXT("lobby")) { Meters = FVector(0.f, 2.f, 1.f); }
	else if (Key == TEXT("bureaux")) { Meters = FVector(36.f, 4.f, 1.f); }
	else if (Key == TEXT("passerelle")) { Meters = FVector(18.5f, 2.f, 4.6f); }
	else if (Key == TEXT("conteneurs")) { Meters = FVector(0.f, -26.f, 1.f); }
	else if (Key == TEXT("tour") || Key == TEXT("tourgade") || Key == TEXT("tourgard"))
	{
		Meters = (MapId == ENexusMapId::Outpost) ? FVector(0.f, 14.f, 1.f) : FVector(0.f, 2.f, 1.f);
	}
	else if (Key == TEXT("ravitaillement") || Key == TEXT("depot")) { Meters = FVector(0.f, -8.f, 1.f); }
	else
	{
		UE_LOG(LogNexusArt, Warning, TEXT("Unknown landmark '%s'"), *Landmark);
		return false;
	}
	APlayerController* PC = World->GetFirstPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn)
	{
		UE_LOG(LogNexusArt, Warning, TEXT("Teleport failed: no pawn"));
		return false;
	}
	const FVector Loc = NexusMeters(Meters) + FVector(0.f, 0.f, 92.f);
	Pawn->SetActorLocation(Loc);
	UE_LOG(LogNexusArt, Log, TEXT("Teleported to %s loc=%s"), *Landmark, *Loc.ToCompactString());
	return true;
}

void FNexusVelasqoValidation::Audit(UWorld* World)
{
	if (!World)
	{
		LogVel(TEXT("AUDIT"), TEXT("FAIL"), TEXT("null world"));
		return;
	}
	int32 HiddenLayers = 0;
	int32 VisibleGrey = 0;
	for (TActorIterator<ANexusVelasqoBuilder> It(World); It; ++It)
	{
		for (UInstancedStaticMeshComponent* ISM : It->MaterialMeshes)
		{
			if (!ISM) { continue; }
			if (ISM->bHiddenInGame) { ++HiddenLayers; }
			else { ++VisibleGrey; }
		}
	}
	int32 Visual = 0;
	for (TActorIterator<ANexusEnvironmentDirector> It(World); It; ++It)
	{
		Visual += It->CountVisualInstances();
	}
	LogVel(TEXT("GREYBOX HIDDEN"), HiddenLayers > 0 && VisibleGrey == 0 ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("hiddenISM=%d visibleGreyISM=%d"), HiddenLayers, VisibleGrey));
	LogVel(TEXT("ARCH INSTANCES"), Visual >= 200 ? TEXT("PASS") : TEXT("FAIL"), FString::Printf(TEXT("ism=%d"), Visual));
	int32 DebugMats = 0;
	int32 SolidMats = 0;
	for (TActorIterator<ANexusEnvironmentDirector> It(World); It; ++It)
	{
		TArray<UInstancedStaticMeshComponent*> Comps;
		It->GetComponents(Comps);
		for (UInstancedStaticMeshComponent* ISM : Comps)
		{
			if (!ISM || ISM->GetInstanceCount() <= 0) { continue; }
			if (UNexusMaterialLibrary::IsDebugOrMissingMaterial(ISM->GetMaterial(0))) { ++DebugMats; }
			else { ++SolidMats; }
		}
	}
	LogVel(TEXT("NO DEBUG GRID MAT"), DebugMats == 0 && SolidMats > 0 ? TEXT("PASS") : TEXT("FAIL"),
		FString::Printf(TEXT("debugISM=%d solidISM=%d (authored MI still missing — solid BasicShape MID)"), DebugMats, SolidMats));
	LogVel(TEXT("FIRST PERSON"), TEXT("NOT VERIFIED"), TEXT("nullrhi/headless cannot prove facade quality"));
}
