#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NexusHUD.generated.h"

NEXUS_API extern int32 GNexusHUDDrawCount;

UCLASS()
class NEXUS_API ANexusHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
