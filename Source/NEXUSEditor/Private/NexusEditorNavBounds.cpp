#include "NexusEditorNavBounds.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "ActorFactories/ActorFactory.h"
#include "Builders/CubeBuilder.h"
#include "Components/BrushComponent.h"
#include "Engine/Brush.h"

void NexusEnsureNavMeshBoundsBrush(ANavMeshBoundsVolume* Volume)
{
	if (!Volume)
	{
		return;
	}

	const FVector SavedLocation = Volume->GetActorLocation();
	const FRotator SavedRotation = Volume->GetActorRotation();
	const FVector SavedScale = Volume->GetActorScale3D();

	UCubeBuilder* CubeBuilder = NewObject<UCubeBuilder>();
	CubeBuilder->X = 200.f;
	CubeBuilder->Y = 200.f;
	CubeBuilder->Z = 200.f;
	UActorFactory::CreateBrushForVolumeActor(Volume, CubeBuilder);

	Volume->SetActorLocation(SavedLocation);
	Volume->SetActorRotation(SavedRotation);
	Volume->SetActorScale3D(SavedScale);

	if (UBrushComponent* BrushComp = Volume->GetBrushComponent())
	{
		BrushComp->UpdateBounds();
	}

	const float Radius = Volume->GetRootComponent() ? Volume->GetRootComponent()->Bounds.SphereRadius : -1.f;
	UE_LOG(LogTemp, Display, TEXT("NavMeshBoundsVolume '%s' brush rebuilt. Scale=%s Bounds.SphereRadius=%.1f"),
		*Volume->GetName(), *SavedScale.ToCompactString(), Radius);
}
