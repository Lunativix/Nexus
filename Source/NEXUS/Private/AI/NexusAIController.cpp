#include "AI/NexusAIController.h"
#include "NexusCharacter.h"
#include "NexusPlayerState.h"
#include "NexusGameState.h"
#include "NexusGameMode.h"
#include "NexusGameplaySettings.h"
#include "Maps/NexusRouteSpline.h"
#include "Objectives/NexusBombSite.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/SplineComponent.h"
#include "EngineUtils.h"
#include "NEXUS.h"

ANexusAIController::ANexusAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2f;
	bWantsPlayerState = true;
}

float ANexusAIController::AimError() const
{
	switch (GetDefault<UNexusGameplaySettings>()->Bots.Difficulty)
	{
	case ENexusBotDifficulty::Easy: return 80.f;
	case ENexusBotDifficulty::Hard: return 18.f;
	default: return 40.f;
	}
}

float ANexusAIController::ReactionDelay() const
{
	switch (GetDefault<UNexusGameplaySettings>()->Bots.Difficulty)
	{
	case ENexusBotDifficulty::Easy: return 0.55f;
	case ENexusBotDifficulty::Hard: return 0.12f;
	default: return 0.28f;
	}
}

void ANexusAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ANexusPlayerState* PS = GetPlayerState<ANexusPlayerState>())
	{
		const int32 Slot = PS->GetUniqueId().IsValid() ? GetTypeHash(PS->GetPlayerName()) : FMath::Rand();
		const int32 R = FMath::Abs(Slot) % 3;
		BotRole = (R == 0) ? ENexusBotRole::Anchor : ((R == 1) ? ENexusBotRole::Roamer : ENexusBotRole::Support);
	}
	PickGoal();
}

void ANexusAIController::PickGoal()
{
	ANexusCharacter* PawnChar = Cast<ANexusCharacter>(GetPawn());
	if (!PawnChar)
	{
		return;
	}

	const ENexusTeam Team = PawnChar->GetTeam();
	ENexusRouteId Wanted = ENexusRouteId::RouteA;
	if (Team == ENexusTeam::Defenders)
	{
		BotState = ENexusBotState::Defend;
		if (BotRole == ENexusBotRole::Anchor)
		{
			PlannedSite = ENexusSiteId::A;
			Wanted = ENexusRouteId::RotateShortAB;
		}
		else if (BotRole == ENexusBotRole::Roamer)
		{
			PlannedSite = ENexusSiteId::B;
			Wanted = ENexusRouteId::RotateShortBA;
		}
		else
		{
			Wanted = ENexusRouteId::RotateShortAB;
		}
	}
	else
	{
		BotState = ENexusBotState::Attack;
		PlannedSite = (BotRole == ENexusBotRole::Anchor || FMath::RandRange(0, 9) < 6) ? ENexusSiteId::A : ENexusSiteId::B;
		if (PlannedSite == ENexusSiteId::A)
		{
			Wanted = (BotRole == ENexusBotRole::Roamer) ? ENexusRouteId::RouteC : ENexusRouteId::RouteA;
		}
		else
		{
			Wanted = ENexusRouteId::RouteB;
		}
	}

	CurrentRoute = nullptr;
	for (TActorIterator<ANexusRouteSpline> It(GetWorld()); It; ++It)
	{
		if (It->RouteId == Wanted)
		{
			CurrentRoute = *It;
			break;
		}
	}
	RoutePoint = 1;
	if (CurrentRoute && CurrentRoute->Spline)
	{
		MoveToLocation(CurrentRoute->Spline->GetLocationAtSplinePoint(RoutePoint, ESplineCoordinateSpace::World), 80.f);
	}
	UE_LOG(LogNexusBot, Log, TEXT("Bot %s role=%s site=%d route=%s"),
		*PawnChar->GetName(), *UEnum::GetValueAsString(BotRole), static_cast<int32>(PlannedSite), *UEnum::GetValueAsString(Wanted));
}

void ANexusAIController::TryShoot()
{
	ANexusCharacter* Self = Cast<ANexusCharacter>(GetPawn());
	ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	if (!Self || Self->Health <= 0.f || !GS || GS->IsBuyPhase() || GS->Phase == ENexusRoundPhase::PostRound)
	{
		return;
	}
	ThinkTimer += GetWorld()->GetDeltaSeconds();
	if (ThinkTimer < ReactionDelay())
	{
		return;
	}
	ThinkTimer = 0.f;

	ANexusCharacter* Target = nullptr;
	float Best = 2800.f;
	for (TActorIterator<ANexusCharacter> It(GetWorld()); It; ++It)
	{
		if (*It == Self || It->Health <= 0.f || It->GetTeam() == Self->GetTeam())
		{
			continue;
		}
		const float Dist = FVector::Dist(Self->GetActorLocation(), It->GetActorLocation());
		if (Dist < Best)
		{
			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(AILos), true, Self);
			if (GetWorld()->LineTraceSingleByChannel(Hit, Self->GetPawnViewLocation(), It->GetActorLocation() + FVector(0, 0, 60), ECC_Visibility, Params))
			{
				if (Hit.GetActor() == *It)
				{
					Best = Dist;
					Target = *It;
				}
			}
		}
	}
	if (Target)
	{
		BotState = ENexusBotState::Engage;
		const FVector Err = FMath::VRand() * AimError();
		SetFocalPoint(Target->GetActorLocation() + Err);
		Self->ServerStartFire();
	}
	else
	{
		if (BotState == ENexusBotState::Engage) { BotState = ENexusBotState::Search; }
		Self->ServerStopFire();
	}
}

void ANexusAIController::TryBuy()
{
	ANexusPlayerState* PS = GetPlayerState<ANexusPlayerState>();
	ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	ANexusGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANexusGameMode>() : nullptr;
	if (!PS || !GS || !GM || !GS->IsBuyPhase())
	{
		return;
	}
	BotState = ENexusBotState::Buy;
	if (PS->ArmorType == ENexusArmorType::None && PS->Credits >= 500)
	{
		GM->ServerBuy(PS, TEXT("LightArmor"));
	}
	if (PS->PrimaryWeapon.IsNone() && PS->Credits >= 1200)
	{
		GM->ServerBuy(PS, (PS->Credits >= 2700) ? FName(TEXT("AK47")) : FName(TEXT("UMP")));
	}
}

void ANexusAIController::TryObjective()
{
	ANexusCharacter* Self = Cast<ANexusCharacter>(GetPawn());
	ANexusGameState* GS = GetWorld()->GetGameState<ANexusGameState>();
	if (!Self || !GS || GS->IsBuyPhase())
	{
		return;
	}
	if (Self->GetTeam() == ENexusTeam::Attackers && Self->HasBomb() && !GS->bBombPlanted)
	{
		BotState = ENexusBotState::Plant;
		bool bInZone = false;
		for (TActorIterator<ANexusBombSite> It(GetWorld()); It; ++It)
		{
			if (It->SiteID == PlannedSite || PlannedSite == ENexusSiteId::None)
			{
				if (It->IsInPlantZone(Self->GetActorLocation()))
				{
					Self->ServerInteractPressed();
					bInZone = true;
					break;
				}
				MoveToLocation(It->GetActorLocation(), 90.f);
			}
		}
		if (!bInZone)
		{
			return;
		}
	}
	if (Self->GetTeam() == ENexusTeam::Defenders && GS->bBombPlanted)
	{
		BotState = ENexusBotState::Defuse;
		if (FVector::Dist2D(Self->GetActorLocation(), GS->BombWorldLocation) < 180.f)
		{
			Self->ServerInteractPressed();
		}
		else
		{
			BotState = ENexusBotState::Retake;
			MoveToLocation(GS->BombWorldLocation, 80.f);
		}
	}
}

void ANexusAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ANexusCharacter* Self = Cast<ANexusCharacter>(GetPawn());
	if (!Self)
	{
		return;
	}
	if (Self->Health <= 0.f)
	{
		BotState = ENexusBotState::Dead;
		return;
	}

	ANexusGameState* GS = GetWorld() ? GetWorld()->GetGameState<ANexusGameState>() : nullptr;
	TryBuy();
	if (GS && GS->IsBuyPhase())
	{
		return;
	}
	TryShoot();
	TryObjective();

	if (GS && GS->bBombPlanted && Self->GetTeam() == ENexusTeam::Attackers && BotState != ENexusBotState::Engage)
	{
		BotState = ENexusBotState::PostPlant;
		MoveToLocation(GS->BombWorldLocation, 220.f);
	}

	if (CurrentRoute && CurrentRoute->Spline && BotState != ENexusBotState::Plant && BotState != ENexusBotState::Defuse && BotState != ENexusBotState::Retake)
	{
		const FVector Goal = CurrentRoute->Spline->GetLocationAtSplinePoint(RoutePoint, ESplineCoordinateSpace::World);
		if (FVector::Dist2D(Self->GetActorLocation(), Goal) < 160.f)
		{
			RoutePoint = FMath::Min(RoutePoint + 1, CurrentRoute->Spline->GetNumberOfSplinePoints() - 1);
			MoveToLocation(CurrentRoute->Spline->GetLocationAtSplinePoint(RoutePoint, ESplineCoordinateSpace::World), 80.f);
		}
		else if (GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			MoveToLocation(Goal, 80.f);
		}
	}
}
