#include "NexusCreateArtAssetsCommandlet.h"
#include "Art/NexusArtTypes.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionSine.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionConstant.h"
#include "MaterialEditingLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "NEXUS.h"

UNexusCreateArtAssetsCommandlet::UNexusCreateArtAssetsCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

static bool SaveAsset(UObject* Asset)
{
	if (!Asset)
	{
		return false;
	}
	UPackage* Package = Asset->GetOutermost();
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(Asset);
	FString Filename;
	if (!FPackageName::TryConvertLongPackageNameToFilename(Package->GetName(), Filename, FPackageName::GetAssetPackageExtension()))
	{
		return false;
	}
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.Error = GError;
	return UPackage::SavePackage(Package, Asset, *Filename, SaveArgs);
}

static UMaterial* CreateMaster(const TCHAR* Name, float DefaultMetallic, float DefaultRoughness)
{
	const FString PackageName = FString::Printf(TEXT("/Game/NEXUS/Art/Materials/%s"), Name);
	UPackage* Package = CreatePackage(*PackageName);
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
	UMaterial* Mat = Cast<UMaterial>(Factory->FactoryCreateNew(
		UMaterial::StaticClass(), Package, FName(Name), RF_Public | RF_Standalone, nullptr, GWarn));
	if (!Mat)
	{
		return nullptr;
	}

	UMaterialExpressionVectorParameter* BaseColor = Cast<UMaterialExpressionVectorParameter>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionVectorParameter::StaticClass(), -480, 0));
	BaseColor->ParameterName = TEXT("BaseColor");
	BaseColor->DefaultValue = FLinearColor(0.5f, 0.42f, 0.32f);

	UMaterialExpressionScalarParameter* Metallic = Cast<UMaterialExpressionScalarParameter>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionScalarParameter::StaticClass(), -480, 180));
	Metallic->ParameterName = TEXT("Metallic");
	Metallic->DefaultValue = DefaultMetallic;

	UMaterialExpressionScalarParameter* Roughness = Cast<UMaterialExpressionScalarParameter>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionScalarParameter::StaticClass(), -480, 280));
	Roughness->ParameterName = TEXT("Roughness");
	Roughness->DefaultValue = DefaultRoughness;

	UMaterialExpressionScalarParameter* Dirt = Cast<UMaterialExpressionScalarParameter>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionScalarParameter::StaticClass(), -480, 380));
	Dirt->ParameterName = TEXT("Dirt");
	Dirt->DefaultValue = 0.15f;

	UMaterialExpressionTextureCoordinate* UV = Cast<UMaterialExpressionTextureCoordinate>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionTextureCoordinate::StaticClass(), -800, 80));
	UMaterialExpressionSine* Sine = Cast<UMaterialExpressionSine>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionSine::StaticClass(), -640, 80));
	UMaterialEditingLibrary::ConnectMaterialExpressions(UV, TEXT(""), Sine, TEXT(""));

	UMaterialExpressionMultiply* DirtMul = Cast<UMaterialExpressionMultiply>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionMultiply::StaticClass(), -320, 80));
	UMaterialEditingLibrary::ConnectMaterialExpressions(Sine, TEXT(""), DirtMul, TEXT("A"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(Dirt, TEXT(""), DirtMul, TEXT("B"));

	UMaterialExpressionConstant* Scale = Cast<UMaterialExpressionConstant>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionConstant::StaticClass(), -320, 180));
	Scale->R = 0.08f;

	UMaterialExpressionMultiply* DirtAmt = Cast<UMaterialExpressionMultiply>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionMultiply::StaticClass(), -160, 80));
	UMaterialEditingLibrary::ConnectMaterialExpressions(DirtMul, TEXT(""), DirtAmt, TEXT("A"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(Scale, TEXT(""), DirtAmt, TEXT("B"));

	UMaterialExpressionAdd* ColorOut = Cast<UMaterialExpressionAdd>(
		UMaterialEditingLibrary::CreateMaterialExpression(Mat, UMaterialExpressionAdd::StaticClass(), -40, 0));
	UMaterialEditingLibrary::ConnectMaterialExpressions(BaseColor, TEXT(""), ColorOut, TEXT("A"));
	UMaterialEditingLibrary::ConnectMaterialExpressions(DirtAmt, TEXT(""), ColorOut, TEXT("B"));

	UMaterialEditingLibrary::ConnectMaterialProperty(ColorOut, TEXT(""), MP_BaseColor);
	UMaterialEditingLibrary::ConnectMaterialProperty(Metallic, TEXT(""), MP_Metallic);
	UMaterialEditingLibrary::ConnectMaterialProperty(Roughness, TEXT(""), MP_Roughness);

	UMaterialEditingLibrary::RecompileMaterial(Mat);
	SaveAsset(Mat);
	return Mat;
}

static void CreateInstance(UMaterial* Parent, const TCHAR* Name, ENexusArtSurface Surface, float Metallic, float Roughness, float Dirt)
{
	if (!Parent)
	{
		return;
	}
	const FString PackageName = FString::Printf(TEXT("/Game/NEXUS/Art/Materials/%s"), Name);
	UPackage* Package = CreatePackage(*PackageName);
	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(Factory->FactoryCreateNew(
		UMaterialInstanceConstant::StaticClass(), Package, FName(Name), RF_Public | RF_Standalone, nullptr, GWarn));
	if (!MIC)
	{
		return;
	}
	MIC->SetParentEditorOnly(Parent);
	UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue(MIC, TEXT("BaseColor"), NexusArtSurfaceColor(Surface));
	UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue(MIC, TEXT("Metallic"), Metallic);
	UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue(MIC, TEXT("Roughness"), Roughness);
	UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue(MIC, TEXT("Dirt"), Dirt);
	UMaterialEditingLibrary::UpdateMaterialInstance(MIC);
	SaveAsset(MIC);
}

int32 UNexusCreateArtAssetsCommandlet::Main(const FString& Params)
{
	UMaterial* Wall = CreateMaster(TEXT("NEXUS_Master_Wall"), 0.02f, 0.74f);
	UMaterial* Floor = CreateMaster(TEXT("NEXUS_Master_Floor"), 0.0f, 0.82f);
	UMaterial* Metal = CreateMaster(TEXT("NEXUS_Master_Metal"), 0.72f, 0.4f);
	UMaterial* Wood = CreateMaster(TEXT("NEXUS_Master_Wood"), 0.0f, 0.78f);
	UMaterial* Glass = CreateMaster(TEXT("NEXUS_Master_Glass"), 0.05f, 0.12f);
	UMaterial* Fabric = CreateMaster(TEXT("NEXUS_Master_Fabric"), 0.0f, 0.86f);
	UMaterial* Concrete = CreateMaster(TEXT("NEXUS_Master_Concrete"), 0.02f, 0.8f);

	CreateInstance(Wall, TEXT("NEXUS_MI_OldStone"), ENexusArtSurface::OldStone, 0.02f, 0.8f, 0.22f);
	CreateInstance(Wall, TEXT("NEXUS_MI_Plaster"), ENexusArtSurface::Plaster, 0.0f, 0.7f, 0.1f);
	CreateInstance(Concrete, TEXT("NEXUS_MI_PaintedConcrete"), ENexusArtSurface::PaintedConcrete, 0.02f, 0.72f, 0.12f);
	CreateInstance(Concrete, TEXT("NEXUS_MI_DirtyConcrete"), ENexusArtSurface::DirtyConcrete, 0.02f, 0.84f, 0.35f);
	CreateInstance(Wall, TEXT("NEXUS_MI_Sandstone"), ENexusArtSurface::Sandstone, 0.0f, 0.76f, 0.18f);
	CreateInstance(Wood, TEXT("NEXUS_MI_Wood"), ENexusArtSurface::Wood, 0.0f, 0.78f, 0.12f);
	CreateInstance(Metal, TEXT("NEXUS_MI_Metal"), ENexusArtSurface::Metal, 0.7f, 0.4f, 0.08f);
	CreateInstance(Metal, TEXT("NEXUS_MI_RustyMetal"), ENexusArtSurface::RustyMetal, 0.45f, 0.7f, 0.28f);
	CreateInstance(Glass, TEXT("NEXUS_MI_Glass"), ENexusArtSurface::Glass, 0.05f, 0.14f, 0.05f);
	CreateInstance(Fabric, TEXT("NEXUS_MI_Fabric"), ENexusArtSurface::Fabric, 0.0f, 0.88f, 0.1f);
	CreateInstance(Floor, TEXT("NEXUS_MI_Asphalt"), ENexusArtSurface::Asphalt, 0.0f, 0.9f, 0.2f);
	CreateInstance(Floor, TEXT("NEXUS_MI_RoadDust"), ENexusArtSurface::RoadDust, 0.0f, 0.85f, 0.3f);

	UE_LOG(LogTemp, Display, TEXT("NEXUS art materials created under /Game/NEXUS/Art/Materials"));
	return 0;
}
