// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboardGameMode.h"

void ASkateboardGameMode::StartMatch()
{
	Super::StartMatch();

	CurrentMatchTime = MatchDuration;
	UpdateMatchTimer();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASkateboardGameMode::UpdateMatchTimer, 1, true);
}

void ASkateboardGameMode::EndMatch()
{
	OnMatchFinished.Broadcast();
	Super::EndMatch();
}

void ASkateboardGameMode::UpdateMatchTimer()
{
	CurrentMatchTime--;
	OnUpdateMatchTime.Broadcast(CurrentMatchTime);
	if (CurrentMatchTime == 0)
	{
		EndMatch();
		TimerHandle.Invalidate();
	}
}
