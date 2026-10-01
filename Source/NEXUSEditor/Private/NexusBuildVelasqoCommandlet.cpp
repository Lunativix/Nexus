#include "NexusBuildVelasqoCommandlet.h"
#include "Maps/NexusVelasqoBuilder.h"
#include "NexusGameMode.h"
#include "NexusEditorNavBounds.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Factories/WorldFactory.h"
#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "UObject/SavePackage.h"
#include "GameFramework/WorldSettings.h"
#include "Editor.h"

UNexusBuildVelasqoCommandlet::UNexusBuildVelasqoCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UNexusBuildVelasqoCommandlet::Main(const FString& Params)
{
	const FString PackageName = TEXT("/Game/NEXUS/Maps/Velasqo/LV_Velasqo_Greybox");
	const FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::ProjectContentDir() / TEXT("NEXUS/Maps/Velasqo/LV_Velasqo_Greybox.umap"));
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
		FName(TEXT("LV_Velasqo_Greybox")),
		RF_Public | RF_Standalone,
		nullptr,
		GWarn));

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create LV_Velasqo_Greybox"));
		return 1;
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

	UE_LOG(LogTemp, Display, TEXT("VELASQO map save %s -> %s"), bSaved ? TEXT("OK") : TEXT("FAILED"), *Filename);
	return bSaved ? 0 : 2;
}
