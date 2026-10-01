#include "Art/NexusMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/PrimitiveComponent.h"

static const TCHAR* GSolidColor = TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial");
static const TCHAR* GFallbackLit = TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial");

const TCHAR* UNexusMaterialLibrary::MasterPath(const TCHAR* Kind)
{
	if (FCString::Stricmp(Kind, TEXT("Wall")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Wall.NEXUS_Master_Wall");
	if (FCString::Stricmp(Kind, TEXT("Floor")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Floor.NEXUS_Master_Floor");
	if (FCString::Stricmp(Kind, TEXT("Metal")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Metal.NEXUS_Master_Metal");
	if (FCString::Stricmp(Kind, TEXT("Wood")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Wood.NEXUS_Master_Wood");
	if (FCString::Stricmp(Kind, TEXT("Glass")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Glass.NEXUS_Master_Glass");
	if (FCString::Stricmp(Kind, TEXT("Fabric")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Fabric.NEXUS_Master_Fabric");
	if (FCString::Stricmp(Kind, TEXT("Concrete")) == 0) return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Concrete.NEXUS_Master_Concrete");
	return TEXT("/Game/NEXUS/Art/Materials/NEXUS_Master_Wall.NEXUS_Master_Wall");
}

const TCHAR* UNexusMaterialLibrary::InstancePath(ENexusArtSurface Surface)
{
	switch (Surface)
	{
	case ENexusArtSurface::OldStone: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_OldStone.NEXUS_MI_OldStone");
	case ENexusArtSurface::Plaster: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Plaster.NEXUS_MI_Plaster");
	case ENexusArtSurface::PaintedConcrete: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_PaintedConcrete.NEXUS_MI_PaintedConcrete");
	case ENexusArtSurface::DirtyConcrete: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_DirtyConcrete.NEXUS_MI_DirtyConcrete");
	case ENexusArtSurface::Sandstone: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Sandstone.NEXUS_MI_Sandstone");
	case ENexusArtSurface::Wood: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Wood.NEXUS_MI_Wood");
	case ENexusArtSurface::Metal: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Metal.NEXUS_MI_Metal");
	case ENexusArtSurface::RustyMetal: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_RustyMetal.NEXUS_MI_RustyMetal");
	case ENexusArtSurface::Glass: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Glass.NEXUS_MI_Glass");
	case ENexusArtSurface::Fabric: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Fabric.NEXUS_MI_Fabric");
	case ENexusArtSurface::Asphalt: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Asphalt.NEXUS_MI_Asphalt");
	case ENexusArtSurface::RoadDust: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_RoadDust.NEXUS_MI_RoadDust");
	case ENexusArtSurface::Ceramic: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Sandstone.NEXUS_MI_Sandstone");
	case ENexusArtSurface::Pavement: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Asphalt.NEXUS_MI_Asphalt");
	case ENexusArtSurface::Foliage: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Wood.NEXUS_MI_Wood");
	case ENexusArtSurface::Water: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Glass.NEXUS_MI_Glass");
	default: return TEXT("/Game/NEXUS/Art/Materials/NEXUS_MI_Sandstone.NEXUS_MI_Sandstone");
	}
}

bool UNexusMaterialLibrary::IsDebugOrMissingMaterial(UMaterialInterface* Material)
{
	if (!Material)
	{
		return true;
	}
	const FString Name = Material->GetName();
	return Name.Contains(TEXT("DefaultMaterial"))
		|| Name.Contains(TEXT("WorldGrid"))
		|| Name.Contains(TEXT("GridMaterial"))
		|| Name.Contains(TEXT("Debug"));
}

UMaterialInterface* UNexusMaterialLibrary::SolidColorParent()
{
	if (UMaterialInterface* Solid = LoadObject<UMaterialInterface>(nullptr, GSolidColor))
	{
		return Solid;
	}
	return LoadObject<UMaterialInterface>(nullptr, GFallbackLit);
}

UMaterialInterface* UNexusMaterialLibrary::LoadMaster(const TCHAR* Kind)
{
	if (UMaterialInterface* Loaded = LoadObject<UMaterialInterface>(nullptr, MasterPath(Kind)))
	{
		if (!IsDebugOrMissingMaterial(Loaded))
		{
			return Loaded;
		}
	}
	return SolidColorParent();
}

UMaterialInterface* UNexusMaterialLibrary::LoadSurface(ENexusArtSurface Surface)
{
	if (UMaterialInterface* Loaded = LoadObject<UMaterialInterface>(nullptr, InstancePath(Surface)))
	{
		if (!IsDebugOrMissingMaterial(Loaded))
		{
			return Loaded;
		}
	}
	return SolidColorParent();
}

UMaterialInstanceDynamic* UNexusMaterialLibrary::MakeSurfaceMID(UObject* Outer, ENexusArtSurface Surface)
{
	UMaterialInterface* Parent = LoadSurface(Surface);
	if (!Parent)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Parent, Outer);
	if (!MID)
	{
		return nullptr;
	}
	const FLinearColor Color = NexusArtSurfaceColor(Surface);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	MID->SetVectorParameterValue(TEXT("BaseColor"), Color);
	MID->SetVectorParameterValue(TEXT("Tint"), Color);
	const bool bMetal = (Surface == ENexusArtSurface::Metal || Surface == ENexusArtSurface::RustyMetal);
	const bool bGlass = (Surface == ENexusArtSurface::Glass || Surface == ENexusArtSurface::Water);
	MID->SetScalarParameterValue(TEXT("Metallic"), bMetal ? 0.72f : (bGlass ? 0.15f : 0.02f));
	float Rough = 0.78f;
	if (bMetal) { Rough = 0.38f; }
	else if (bGlass) { Rough = 0.12f; }
	else if (Surface == ENexusArtSurface::Asphalt) { Rough = 0.88f; }
	else if (Surface == ENexusArtSurface::Ceramic) { Rough = 0.45f; }
	else if (Surface == ENexusArtSurface::Fabric) { Rough = 0.92f; }
	MID->SetScalarParameterValue(TEXT("Roughness"), Rough);
	MID->SetScalarParameterValue(TEXT("Specular"), bGlass ? 0.85f : 0.22f);
	MID->SetScalarParameterValue(TEXT("Dirt"),
		(Surface == ENexusArtSurface::DirtyConcrete || Surface == ENexusArtSurface::RoadDust || Surface == ENexusArtSurface::OldStone) ? 0.4f : 0.1f);
	return MID;
}

void UNexusMaterialLibrary::ApplyToPrimitive(UPrimitiveComponent* Comp, ENexusArtSurface Surface, int32 ElementIndex)
{
	if (!Comp)
	{
		return;
	}
	if (UMaterialInstanceDynamic* MID = MakeSurfaceMID(Comp, Surface))
	{
		Comp->SetMaterial(ElementIndex, MID);
	}
}

void UNexusMaterialLibrary::ApplyGameplayMaterial(UPrimitiveComponent* Comp, ENexusMaterialType Type)
{
	ApplyToPrimitive(Comp, NexusArtSurfaceFromGameplay(Type));
}
