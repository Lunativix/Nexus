#include "NexusBakeVelasqoNavCommandlet.h"
#include "NexusEditorNavBounds.h"
#include "Engine/World.h"
#include "FileHelpers.h"
#include "UObject/SavePackage.h"
#include "Misc/Paths.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "EngineUtils.h"
#include "Editor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BrushComponent.h"

UNexusBakeVelasqoNavCommandlet::UNexusBakeVelasqoNavCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UNexusBakeVelasqoNavCommandlet::Main(const FString& Params)
{
	const FString PackageName = TEXT("/Game/NEXUS/Maps/Velasqo/LV_Velasqo_Greybox");
	const FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::ProjectContentDir() / TEXT("NEXUS/Maps/Velasqo/LV_Velasqo_Greybox.umap"));

	if (!FPaths::FileExists(Filename))
	{
		UE_LOG(LogTemp, Error, TEXT("VELASQO umap missing — bake aborted (will not regenerate layout): %s"), *Filename);
		return 1;
	}

	UWorld* World = nullptr;
	if (GEditor)
	{
		FEditorFileUtils::LoadMap(Filename, false, true);
		World = GEditor->GetEditorWorldContext().World();
	}
	if (!World)
	{
		UPackage* Package = LoadPackage(nullptr, *PackageName, LOAD_None);
		World = Package ? UWorld::FindWorldInPackage(Package) : nullptr;
	}
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No UWorld in LV_Velasqo_Greybox"));
		return 3;
	}

	bool bHasNavBounds = false;
	for (TActorIterator<ANavMeshBoundsVolume> It(World); It; ++It)
	{
		bHasNavBounds = true;
		NexusEnsureNavMeshBoundsBrush(*It);
	}
	if (!bHasNavBounds)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.OverrideLevel = World->GetCurrentLevel();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(FVector(0.f, 0.f, 400.f), FRotator::ZeroRotator, SpawnParams))
		{
			NavBounds->SetActorScale3D(FVector(60.f, 60.f, 8.f));
			NexusEnsureNavMeshBoundsBrush(NavBounds);
			UE_LOG(LogTemp, Display, TEXT("Spawned NavMeshBoundsVolume covering ±60m playable area"));
		}
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		TArray<UPrimitiveComponent*> PrimComps;
		It->GetComponents<UPrimitiveComponent>(PrimComps);
		for (UPrimitiveComponent* Prim : PrimComps)
		{
			if (Prim)
			{
				Prim->SetCanEverAffectNavigation(true);
			}
		}
	}

	FNavigationSystem::AddNavigationSystemToWorld(*World, FNavigationSystemRunMode::EditorMode);
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys)
	{
		UE_LOG(LogTemp, Error, TEXT("No NavigationSystem"));
		return 4;
	}

	NavSys->ReleaseInitialBuildingLock();
	NavSys->RemoveNavigationBuildLock(ENavigationBuildLock::AsyncLoadLock);
	NavSys->RemoveNavigationBuildLock(ENavigationBuildLock::NoUpdateInEditor);
	NavSys->GetDefaultNavDataInstance(FNavigationSystem::ECreateIfEmpty::Create);
	const FBox Dirty(FVector(-6500.f, -6500.f, -200.f), FVector(6500.f, 6500.f, 2000.f));
	NavSys->AddDirtyArea(Dirty, ENavigationDirtyFlag::All);
	NavSys->Build();

	const double Deadline = FPlatformTime::Seconds() + 60.0;
	while (NavSys->GetNumRemainingBuildTasks() > 0 && FPlatformTime::Seconds() < Deadline)
	{
		NavSys->Tick(0.05f);
	}

	int32 RecastCount = 0;
	int32 Tiles = 0;
	for (TActorIterator<ARecastNavMesh> It(World); It; ++It)
	{
		++RecastCount;
		Tiles += It->GetNumActiveTiles();
	}
	UE_LOG(LogTemp, Display, TEXT("VELASQO nav bake: RecastNavMesh actors=%d tiles=%d remainingTasks=%d"),
		RecastCount, Tiles, NavSys->GetNumRemainingBuildTasks());

	UPackage* Package = World->GetOutermost();
	Package->MarkPackageDirty();
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.SaveFlags = SAVE_NoError;
	const bool bSaved = UPackage::SavePackage(Package, World, *Filename, SaveArgs);
	UE_LOG(LogTemp, Display, TEXT("VELASQO nav save %s -> %s"), bSaved ? TEXT("OK") : TEXT("FAILED"), *Filename);
	return (bSaved && RecastCount > 0) ? 0 : 5;
}
