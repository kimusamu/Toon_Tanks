// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTankGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTankPlayerController.h"
#include "TimerManager.h"

void AToonTankGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Tank) {
		Tank->HandleDestruction();

		if (ToonTankPlayerController) {
			ToonTankPlayerController->SetPlayerEnabledState(false);
		}

		GameOver(false);
	}

	else if (ATower* DestroyedTower = Cast<ATower>(DeadActor)) {
		DestroyedTower->HandleDestruction();
		--TargetTowers;

		if (TargetTowers == 0) {
			GameOver(true);
		}
	}
}

void AToonTankGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
}

void AToonTankGameMode::HandleGameStart()
{
	TargetTowers = GetTargetTowerCount();
	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	ToonTankPlayerController = Cast<AToonTankPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	StartGame();

	if (ToonTankPlayerController) {
		ToonTankPlayerController->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnableTimerHandle;
		FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(
			ToonTankPlayerController,
			&AToonTankPlayerController::SetPlayerEnabledState,
			true
		);

		GetWorldTimerManager().SetTimer(
			PlayerEnableTimerHandle,
			PlayerEnableTimerDelegate,
			StartDelay,
			false);
	}
}

int32 AToonTankGameMode::GetTargetTowerCount()
{
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), Towers);

	return Towers.Num();
}