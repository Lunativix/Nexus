#include "NexusBuildMapsCommandlet.h"
#include "Maps/NexusVelasqoBuilder.h"
#include "Maps/NexusMapCatalog.h"
#include "NexusGameMode.h"
#include "NexusEditorNavBounds.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Factories/WorldFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
#include "GameFramework/WorldSettings.h"
#include "Editor.h"

UNexusBuildMapsCommandlet::UNexusBuildMapsCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UNexusBuildMapsCommandlet::Main(const FString& Params)
{
	int32 Failed = 0;
	const ENexusMapId Maps[] = { ENexusMapId::Skyline, ENexusMapId::Outpost };
	for (ENexusMapId Id : Maps)
	{
		const FString PackageName = FNexusMapCatalog::PackagePath(Id);
		FString AssetName;
		PackageName.Split(TEXT("/"), nullptr, &AssetName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		const FString Rel = PackageName.RightChop(6); // strip "/Game/"
		const FString Filename = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / Rel + TEXT(".umap"));
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
		if (IFileManager::Get().FileExists(*Filename))
		{
			IFileManager::Get().Delete(*Filename, false, true);
		}

		UPackage* Package = CreatePackage(*PackageName);
		UWorldFactory* Factory = NewObject<UWorldFactory>();
		UWorld* World = Cast<UWorld>(Factory->FactoryCreateNew(
			UWorld::StaticClass(),
			Package,
			FName(*AssetName),
			RF_Public | RF_Standalone,
			nullptr,
			GWarn));
		if (!World)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create %s"), *PackageName);
			++Failed;
			continue;
		}
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->DefaultGameMode = ANexusGameMode::StaticClass();
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.OverrideLevel = World->GetCurrentLevel();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ANexusVelasqoBuilder* Builder = World->SpawnActor<ANexusVelasqoBuilder>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Builder)
		{
			Builder->bBuildOnBeginPlay = false;
			Builder->BuildMap();
		}
		for (TActorIterator<ANavMeshBoundsVolume> It(World); It; ++It)
		{
			NexusEnsureNavMeshBoundsBrush(*It);
		}

		FAssetRegistryModule::AssetCreated(World);
		Package->MarkPackageDirty();
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.bForceByteSwapping = false;
		SaveArgs.bWarnOfLongFilename = true;
		SaveArgs.SaveFlags = SAVE_NoError;
		const bool bSaved = UPackage::SavePackage(Package, World, *Filename, SaveArgs);
		UE_LOG(LogTemp, Display, TEXT("%s map save %s -> %s"), FNexusMapCatalog::MapName(Id), bSaved ? TEXT("OK") : TEXT("FAILED"), *Filename);
		if (!bSaved)
		{
			++Failed;
		}
	}
	return Failed;
}
