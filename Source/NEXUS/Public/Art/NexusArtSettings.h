#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Art/NexusArtTypes.h"
#include "NexusArtSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "NEXUS Art"))
class NEXUS_API UNexusArtSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UNexusArtSettings();
	virtual FName GetCategoryName() const override { return FName(TEXT("NEXUS")); }

	UPROPERTY(EditAnywhere, Config, Category = "TimeOfDay")
	ENexusTimeOfDay TimeOfDay = ENexusTimeOfDay::Evening;

	UPROPERTY(EditAnywhere, Config, Category = "TimeOfDay")
	FNexusTimeOfDayPreset Morning;

	UPROPERTY(EditAnywhere, Config, Category = "TimeOfDay")
	FNexusTimeOfDayPreset Day;

	UPROPERTY(EditAnywhere, Config, Category = "TimeOfDay")
	FNexusTimeOfDayPreset Evening;

	UPROPERTY(EditAnywhere, Config, Category = "TimeOfDay")
	FNexusTimeOfDayPreset Night;

	UPROPERTY(EditAnywhere, Config, Category = "Fog")
	FNexusFogPreset Velasqo_DefaultFog;

	UPROPERTY(EditAnywhere, Config, Category = "Fog")
	FNexusFogPreset Skyline_DefaultFog;

	UPROPERTY(EditAnywhere, Config, Category = "Fog")
	FNexusFogPreset Outpost_DefaultFog;

	UPROPERTY(EditAnywhere, Config, Category = "Camera")
	bool bEnableWeaponSway = true;

	UPROPERTY(EditAnywhere, Config, Category = "Camera")
	float WeaponSwayScale = 1.f;

	UPROPERTY(EditAnywhere, Config, Category = "Camera")
	bool bEnableHeadBob = true;

	UPROPERTY(EditAnywhere, Config, Category = "Camera")
	float HeadBobScale = 0.55f;

	UPROPERTY(EditAnywhere, Config, Category = "Camera")
	float RecoilCameraScale = 0.4f;

	UPROPERTY(EditAnywhere, Config, Category = "Audio")
	float FootstepVolume = 0.4f;

	UPROPERTY(EditAnywhere, Config, Category = "Audio")
	float FootstepRange = 1800.f;

	UPROPERTY(EditAnywhere, Config, Category = "Audio")
	float OcclusionMultiplier = 0.35f;

	UPROPERTY(EditAnywhere, Config, Category = "Audio")
	float SurfaceMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Config, Category = "Audio")
	float WeaponFireVolume = 0.55f;

	const FNexusTimeOfDayPreset& ActiveTimeOfDay() const;
};
