#include "NexusPlayerController.h"
#include "NexusGameMode.h"
#include "NexusPlayerState.h"
#include "NexusCharacter.h"
#include "NexusGameState.h"
#include "Art/NexusAudio.h"
#include "Debug/NexusLevelValidator.h"
#include "Debug/NexusLevelDebug.h"
#include "Debug/NexusGameplayTests.h"
#include "Debug/NexusNavConnectivity.h"
#include "Debug/NexusVelasqoValidation.h"
#include "Maps/NexusMapCatalog.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "NEXUS.h"

ANexusPlayerController::ANexusPlayerController()
{
	bShowMouseCursor = false;
	PrimaryActorTick.bCanEverTick = true;
}

void ANexusPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void ANexusPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Scoreboard"), IE_Pressed, this, &ANexusPlayerController::OnScoreboardPressed);
		InputComponent->BindAction(TEXT("Scoreboard"), IE_Released, this, &ANexusPlayerController::OnScoreboardReleased);
		InputComponent->BindAction(TEXT("BuyMenu"), IE_Pressed, this, &ANexusPlayerController::OnBuyMenu);
		InputComponent->BindAction(TEXT("SpecNext"), IE_Pressed, this, &ANexusPlayerController::OnSpecNext);
		InputComponent->BindAction(TEXT("SpecPrev"), IE_Pressed, this, &ANexusPlayerController::OnSpecPrev);
	}
}

void ANexusPlayerController::ShowCallout(const FText& Callout)
{
	LastCallout = Callout;
	CalloutTimeRemaining = 2.0f;
}

void ANexusPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (CalloutTimeRemaining > 0.f)
	{
		CalloutTimeRemaining -= DeltaTime;
	}
	if (HitMarkerTime > 0.f)
	{
		HitMarkerTime -= DeltaTime;
	}
}

void ANexusPlayerController::ClientHitFeedback_Implementation(bool bHeadshot)
{
	HitMarkerTime = 0.18f;
	bHitMarkerHead = bHeadshot;
	UNexusAudio::PlayHitConfirm(this, bHeadshot);
}

void ANexusPlayerController::OnScoreboardPressed() { bShowScoreboard = true; }
void ANexusPlayerController::OnScoreboardReleased() { bShowScoreboard = false; }
void ANexusPlayerController::OnBuyMenu() { bShowBuyMenu = !bShowBuyMenu; }
void ANexusPlayerController::OnSpecNext() { CycleSpectator(1); }
void ANexusPlayerController::OnSpecPrev() { CycleSpectator(-1); }

void ANexusPlayerController::EnterSpectator()
{
	bSpectating = true;
	CycleSpectator(1);
}

void ANexusPlayerController::CycleSpectator(int32 Dir)
{
	ANexusPlayerState* SelfPS = GetPlayerState<ANexusPlayerState>();
	if (!SelfPS)
	{
		return;
	}
	TArray<ANexusCharacter*> Teammates;
	for (TActorIterator<ANexusCharacter> It(GetWorld()); It; ++It)
	{
		if (It->Health > 0.f && It->GetNexusPS() && It->GetNexusPS()->Faction == SelfPS->Faction)
		{
			Teammates.Add(*It);
		}
	}
	if (Teammates.Num() == 0)
	{
		return;
	}
	int32 Idx = 0;
	if (AActor* View = GetViewTarget())
	{
		Idx = Teammates.IndexOfByPredicate([&](ANexusCharacter* C) { return C == View; });
	}
	Idx = (Idx + Dir + Teammates.Num()) % Teammates.Num();
	SetViewTarget(Teammates[Idx]);
}

void ANexusPlayerController::ServerBuyItem_Implementation(FName ItemId)
{
	if (ANexusGameMode* GM = GetWorld()->GetAuthGameMode<ANexusGameMode>())
	{
		GM->ServerBuy(GetPlayerState<ANexusPlayerState>(), ItemId);
	}
}

void ANexusPlayerController::ServerPickOperator_Implementation(uint8 OperatorId)
{
	if (ANexusGameMode* GM = GetWorld()->GetAuthGameMode<ANexusGameMode>())
	{
		GM->ServerSelectOperator(GetPlayerState<ANexusPlayerState>(), static_cast<ENexusOperatorId>(OperatorId));
	}
}

void ANexusPlayerController::Buy(const FString& ItemId)
{
	ServerBuyItem(FName(*ItemId));
}

void ANexusPlayerController::NexusValidateLevel() { FNexusLevelValidator::Run(GetWorld()); }
void ANexusPlayerController::NexusLevelDebug(int32 Mask) { ANexusLevelDebug::Toggle(GetWorld(), Mask); }
void ANexusPlayerController::NexusTestPlant(const FString& Site) { FNexusGameplayTests::TestPlant(GetWorld(), Site); }
void ANexusPlayerController::NexusTestDefuse() { FNexusGameplayTests::TestDefuse(GetWorld()); }
void ANexusPlayerController::NexusTestWallbang() { FNexusGameplayTests::TestWallbang(GetWorld()); }
void ANexusPlayerController::NexusTestRound() { FNexusGameplayTests::TestRound(GetWorld()); }
void ANexusPlayerController::NexusTestBots() { FNexusGameplayTests::TestBots(GetWorld()); }
void ANexusPlayerController::NexusTestNavigation() { FNexusGameplayTests::TestNavigation(GetWorld()); }
void ANexusPlayerController::NexusTestMatch() { FNexusGameplayTests::TestMatch(GetWorld()); }

void ANexusPlayerController::NexusForceScore(int32 VanguardRounds, int32 SentinelRounds)
{
	if (ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr)
	{
		GM->DebugForceScore(VanguardRounds, SentinelRounds);
	}
}

void ANexusPlayerController::NexusForceOvertime()
{
	if (ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr)
	{
		GM->DebugForceOvertimeFromTie();
	}
}

void ANexusPlayerController::NexusNavConnectivity()
{
	FNexusNavConnectivity::Run(GetWorld());
}

void ANexusPlayerController::NexusRematch()
{
#if !UE_BUILD_SHIPPING
	if (ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr)
	{
		GM->Rematch();
	}
#endif
}

void ANexusPlayerController::NexusGiveCredits(int32 Amount)
{
#if !UE_BUILD_SHIPPING
	if (ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr)
	{
		GM->AwardCredits(GetPlayerState<ANexusPlayerState>(), Amount);
	}
#endif
}

void ANexusPlayerController::NexusShowEconomy()
{
#if !UE_BUILD_SHIPPING
	FNexusGameplayTests::TestEconomyPanel(GetWorld());
#endif
}

void ANexusPlayerController::NexusNetSim(int32 Milliseconds)
{
#if !UE_BUILD_SHIPPING
	if (IConsoleVariable* Lag = IConsoleManager::Get().FindConsoleVariable(TEXT("NetPktLag")))
	{
		Lag->Set(FMath::Max(0, Milliseconds), ECVF_SetByConsole);
	}
	UE_LOG(LogNexusNetwork, Log, TEXT("NetSim lag=%dms"), Milliseconds);
#endif
}

void ANexusPlayerController::NexusTestOperators()
{
#if !UE_BUILD_SHIPPING
	FNexusGameplayTests::TestOperators(GetWorld());
#endif
}

void ANexusPlayerController::NexusValidateSpawns()
{
#if !UE_BUILD_SHIPPING
	FNexusGameplayTests::TestSpawns(GetWorld());
#endif
}

void ANexusPlayerController::NexusNetworkTest()
{
#if !UE_BUILD_SHIPPING
	FNexusGameplayTests::TestNetwork(GetWorld());
#endif
}

void ANexusPlayerController::NexusStartBotMatch()
{
#if !UE_BUILD_SHIPPING
	if (ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr)
	{
		GM->FillBotsToFiveVFive();
		GM->StartMatchFlow();
		UE_LOG(LogNexusMatch, Log, TEXT("StartBotMatch 5v5"));
	}
#endif
}

void ANexusPlayerController::NexusAutoMatchTest()
{
#if !UE_BUILD_SHIPPING
	if (ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr)
	{
		GM->FillBotsToFiveVFive();
		UE_LOG(LogNexusMatch, Log, TEXT("AutoMatchTest filled 5v5 — full 13-round play is NOT VERIFIED in this command (starts match, does not simulate 24 rounds)"));
		GM->StartMatchFlow();
	}
#endif
}

void ANexusPlayerController::NexusValidateVelasqo()
{
#if !UE_BUILD_SHIPPING
	FNexusVelasqoValidation::ValidateMap(GetWorld());
#endif
}

void ANexusPlayerController::NexusVelasqoWalkTest()
{
#if !UE_BUILD_SHIPPING
	FNexusVelasqoValidation::WalkTest(GetWorld());
#endif
}

void ANexusPlayerController::NexusVelasqoTeleport(const FString& Landmark)
{
#if !UE_BUILD_SHIPPING
	FNexusVelasqoValidation::TeleportToLandmark(GetWorld(), Landmark);
#endif
}

void ANexusPlayerController::NexusVelasqoFullTest()
{
#if !UE_BUILD_SHIPPING
	FNexusVelasqoValidation::FullTest(GetWorld());
#endif
}

void ANexusPlayerController::NexusOpenMap(const FString& MapName)
{
	ENexusMapId Id = ENexusMapId::Velasqo;
	if (MapName.Contains(TEXT("Sky"), ESearchCase::IgnoreCase) || MapName.Contains(TEXT("2")))
	{
		Id = ENexusMapId::Skyline;
	}
	else if (MapName.Contains(TEXT("Out"), ESearchCase::IgnoreCase) || MapName.Contains(TEXT("3")))
	{
		Id = ENexusMapId::Outpost;
	}
	const FString Path = FNexusMapCatalog::PackagePath(Id);
	UE_LOG(LogNEXUS, Log, TEXT("OpenMap %s -> %s"), *MapName, *Path);
	UGameplayStatics::OpenLevel(this, FName(*Path));
}
