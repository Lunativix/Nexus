#include "Art/NexusAudio.h"
#include "Art/NexusArtSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/StrongObjectPtr.h"

static TArray<TStrongObjectPtr<USoundWaveProcedural>> GNexusAudioKeep;

static USoundWaveProcedural* MakeBurst(UObject* Outer, float FreqHz, float Duration, float Amp)
{
	USoundWaveProcedural* Wave = NewObject<USoundWaveProcedural>(Outer);
	const int32 SampleRate = 22050;
	Wave->SetSampleRate(SampleRate);
	Wave->NumChannels = 1;
	Wave->Duration = Duration;
	Wave->SoundGroup = SOUNDGROUP_Default;
	Wave->bLooping = false;
	const int32 Frames = FMath::Max(32, FMath::RoundToInt(Duration * SampleRate));
	TArray<uint8> PCM;
	PCM.SetNumUninitialized(Frames * 2);
	int16* Samples = reinterpret_cast<int16*>(PCM.GetData());
	for (int32 i = 0; i < Frames; ++i)
	{
		const float T = i / float(SampleRate);
		const float Env = 1.f - (i / float(Frames));
		const float S = FMath::Sin(2.f * PI * FreqHz * T) * Amp * Env;
		Samples[i] = (int16)FMath::Clamp(S * 32767.f, -32767.f, 32767.f);
	}
	Wave->QueueAudio(PCM.GetData(), PCM.Num());
	GNexusAudioKeep.Add(TStrongObjectPtr<USoundWaveProcedural>(Wave));
	if (GNexusAudioKeep.Num() > 24)
	{
		GNexusAudioKeep.RemoveAt(0);
	}
	return Wave;
}

float UNexusAudio::OccludedVolume(UWorld* World, const FVector& From, const FVector& To, float InVolume)
{
	const UNexusArtSettings* Art = GetDefault<UNexusArtSettings>();
	if (!World || !Art)
	{
		return InVolume;
	}
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NexusAudioOcc), true);
	if (World->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params))
	{
		return InVolume * FMath::Clamp(Art->OcclusionMultiplier, 0.05f, 1.f);
	}
	return InVolume;
}

void UNexusAudio::PlayFire(UObject* WorldContext, const FVector& Location)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World) { return; }
	const UNexusArtSettings* Art = GetDefault<UNexusArtSettings>();
	USoundWaveProcedural* Wave = MakeBurst(World, 220.f, 0.07f, 0.85f);
	UGameplayStatics::PlaySoundAtLocation(World, Wave, Location, Art->WeaponFireVolume, 1.f, 0.f, nullptr, nullptr);
}

void UNexusAudio::PlayReload(UObject* WorldContext, const FVector& Location)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World) { return; }
	USoundWaveProcedural* Wave = MakeBurst(World, 140.f, 0.12f, 0.5f);
	UGameplayStatics::PlaySoundAtLocation(World, Wave, Location, 0.35f);
}

void UNexusAudio::PlayFootstep(UObject* WorldContext, const FVector& Location, ENexusFootstepSurface Surface)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World) { return; }
	const UNexusArtSettings* Art = GetDefault<UNexusArtSettings>();
	float Freq = 90.f;
	switch (Surface)
	{
	case ENexusFootstepSurface::Wood: Freq = 160.f; break;
	case ENexusFootstepSurface::Metal: Freq = 240.f; break;
	case ENexusFootstepSurface::Glass: Freq = 280.f; break;
	case ENexusFootstepSurface::Sand: Freq = 70.f; break;
	case ENexusFootstepSurface::Stone: Freq = 110.f; break;
	default: Freq = 95.f; break;
	}
	APlayerController* PC = World->GetFirstPlayerController();
	const FVector Ear = PC && PC->PlayerCameraManager ? PC->PlayerCameraManager->GetCameraLocation() : Location;
	const float Vol = OccludedVolume(World, Ear, Location, Art->FootstepVolume * Art->SurfaceMultiplier);
	if (FVector::Dist(Ear, Location) > Art->FootstepRange)
	{
		return;
	}
	USoundWaveProcedural* Wave = MakeBurst(World, Freq, 0.05f, 0.7f);
	UGameplayStatics::PlaySoundAtLocation(World, Wave, Location, Vol);
}

void UNexusAudio::PlayHitConfirm(UObject* WorldContext, bool bHeadshot)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World) { return; }
	USoundWaveProcedural* Wave = MakeBurst(World, bHeadshot ? 880.f : 520.f, 0.04f, 0.45f);
	UGameplayStatics::PlaySound2D(World, Wave, 0.35f);
}

void UNexusAudio::PlayPlant(UObject* WorldContext, const FVector& Location)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World) { return; }
	USoundWaveProcedural* Wave = MakeBurst(World, 330.f, 0.18f, 0.6f);
	UGameplayStatics::PlaySoundAtLocation(World, Wave, Location, 0.5f);
}

void UNexusAudio::PlayRoundSting(UObject* WorldContext, bool bStart)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World) { return; }
	USoundWaveProcedural* Wave = MakeBurst(World, bStart ? 392.f : 196.f, 0.22f, 0.5f);
	UGameplayStatics::PlaySound2D(World, Wave, 0.4f);
}
