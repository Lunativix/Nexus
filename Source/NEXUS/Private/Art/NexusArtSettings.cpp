#include "Art/NexusArtSettings.h"

UNexusArtSettings::UNexusArtSettings()
{
	Morning.SunRotation = FRotator(-28.f, 95.f, 0.f);
	Morning.SunColor = FLinearColor(1.f, 0.82f, 0.62f);
	Morning.SunIntensity = 6.5f;
	Morning.SkyIntensity = 0.9f;
	Morning.SkyColor = FLinearColor(0.55f, 0.62f, 0.78f);

	Day.SunRotation = FRotator(-55.f, 140.f, 0.f);
	Day.SunColor = FLinearColor(1.f, 0.95f, 0.88f);
	Day.SunIntensity = 10.f;
	Day.SkyIntensity = 1.1f;
	Day.SkyColor = FLinearColor(0.55f, 0.65f, 0.85f);

	Evening.SunRotation = FRotator(-28.f, 232.f, 0.f);
	Evening.SunColor = FLinearColor(1.f, 0.82f, 0.62f);
	Evening.SunIntensity = 5.2f;
	Evening.SkyIntensity = 0.8f;
	Evening.SkyColor = FLinearColor(0.48f, 0.52f, 0.68f);

	Night.SunRotation = FRotator(-8.f, 210.f, 0.f);
	Night.SunColor = FLinearColor(0.35f, 0.42f, 0.7f);
	Night.SunIntensity = 1.4f;
	Night.SkyIntensity = 0.35f;
	Night.SkyColor = FLinearColor(0.12f, 0.14f, 0.22f);

	Velasqo_DefaultFog.FogDensity = 0.004f;
	Velasqo_DefaultFog.FogHeightFalloff = 0.2f;
	Velasqo_DefaultFog.FogInscattering = FLinearColor(0.42f, 0.44f, 0.48f);
	Velasqo_DefaultFog.FogStartDistance = 2800.f;
	Velasqo_DefaultFog.FogMaxOpacity = 0.16f;

	Skyline_DefaultFog.FogDensity = 0.0032f;
	Skyline_DefaultFog.FogHeightFalloff = 0.12f;
	Skyline_DefaultFog.FogInscattering = FLinearColor(0.18f, 0.28f, 0.48f);
	Skyline_DefaultFog.FogStartDistance = 3200.f;
	Skyline_DefaultFog.FogMaxOpacity = 0.28f;

	Outpost_DefaultFog.FogDensity = 0.0065f;
	Outpost_DefaultFog.FogHeightFalloff = 0.16f;
	Outpost_DefaultFog.FogInscattering = FLinearColor(0.62f, 0.48f, 0.28f);
	Outpost_DefaultFog.FogStartDistance = 1800.f;
	Outpost_DefaultFog.FogMaxOpacity = 0.24f;
}

const FNexusTimeOfDayPreset& UNexusArtSettings::ActiveTimeOfDay() const
{
	switch (TimeOfDay)
	{
	case ENexusTimeOfDay::Morning: return Morning;
	case ENexusTimeOfDay::Day: return Day;
	case ENexusTimeOfDay::Night: return Night;
	default: return Evening;
	}
}
