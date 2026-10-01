#include "Interaction/NexusInteractable.h"
#include "NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

bool INexusInteractable::CanNexusInteract(ANexusCharacter* /*Character*/) const
{
	return true;
}

void INexusInteractable::ExecuteNexusInteract(ANexusCharacter* /*Character*/)
{
}

FVector INexusInteractable::GetNexusInteractLocation() const
{
	if (const AActor* Actor = Cast<AActor>(this))
	{
		return Actor->GetActorLocation();
	}
	return FVector::ZeroVector;
}

bool NexusInteraction::HasLineOfSight(UWorld* World, const FVector& From, AActor* Target, AActor* Instigator)
{
	if (!World || !Target)
	{
		return false;
	}
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NexusInteractLOS), true, Instigator);
	Params.AddIgnoredActor(Target);
	const FVector To = Target->GetActorLocation();
	if (World->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params))
	{
		return false;
	}
	return true;
}

AActor* NexusInteraction::FindBestInteractable(ANexusCharacter* Character, float MaxDistance)
{
	if (!Character || !Character->GetWorld())
	{
		return nullptr;
	}
	AActor* Best = nullptr;
	float BestDist = MaxDistance;
	const FVector Eyes = Character->FirstPersonCamera
		? Character->FirstPersonCamera->GetComponentLocation()
		: Character->GetActorLocation() + FVector(0, 0, 64.f);
	for (TActorIterator<AActor> It(Character->GetWorld()); It; ++It)
	{
		if (!It->GetClass()->ImplementsInterface(UNexusInteractable::StaticClass()))
		{
			continue;
		}
		const float Dist = FVector::Dist(Character->GetActorLocation(), It->GetActorLocation());
		if (Dist >= BestDist)
		{
			continue;
		}
		if (!HasLineOfSight(Character->GetWorld(), Eyes, *It, Character))
		{
			continue;
		}
		if (INexusInteractable* Interact = Cast<INexusInteractable>(*It))
		{
			if (!Interact->CanNexusInteract(Character))
			{
				continue;
			}
		}
		BestDist = Dist;
		Best = *It;
	}
	return Best;
}
