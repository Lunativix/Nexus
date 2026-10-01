#pragma once

#include "CoreMinimal.h"

struct NEXUS_API FNexusGameplayTests
{
	static void TestPlant(UWorld* World, const FString& SiteArg);
	static void TestDefuse(UWorld* World);
	static void TestWallbang(UWorld* World);
	static void TestRound(UWorld* World);
	static void TestBots(UWorld* World);
	static void TestNavigation(UWorld* World);
	static void TestInput(UWorld* World);
	static void TestListenServer(UWorld* World);
	static void TestReplication(UWorld* World);
	static void TestFiveVFive(UWorld* World);
	static void TestMatch(UWorld* World);
	static void TestLiveDamage(UWorld* World);
	static void TestGadgets(UWorld* World);
	static void TestOvertimeForce(UWorld* World);
	static void TestPlantInterrupt(UWorld* World);
	static void TestPlantComplete(UWorld* World);
	static void TestDefuseComplete(UWorld* World);
	static void TestHUD(UWorld* World);
	static void TestOperators(UWorld* World);
	static void TestUltimates(UWorld* World);
	static void TestSpawns(UWorld* World);
	static void TestNetwork(UWorld* World);
	static void TestEconomyPanel(UWorld* World);
	static void TestVelasqo(UWorld* World);
};
