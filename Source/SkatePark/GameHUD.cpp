// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDisplay.h"
#include "ScoreSubsystem.h"
#include "SkateboardGameMode.h"

class UScoreSubsystem;

void AGameHUD::InitializeHUD()
{
	PlayerDisplay = CreateWidget<UPlayerDisplay>(GetWorld(), PlayerDisplayClass);
	PlayerDisplay->AddToViewport();

	if (UScoreSubsystem* ScoreSubsystem = GetGameInstance()->GetSubsystem<UScoreSubsystem>())
	{
		ScoreSubsystem->OnScored.AddDynamic(this, &AGameHUD::OnScored);
	}

	if (ASkateboardGameMode* GameMode = Cast<ASkateboardGameMode>(GetWorld()->GetAuthGameMode()))
	{
		UpdateTimer(GameMode->GetMatchDuration());
		GameMode->OnUpdateMatchTime.AddDynamic(this, &AGameHUD::UpdateTimer);
	}
}

void AGameHUD::OnScored(const int32 TotalScore, const int32 NewScore, const FString& ScoreMessage)
{
	PlayerDisplay->DisplayNewScore(TotalScore, NewScore, ScoreMessage);
}

void AGameHUD::UpdateTimer(int32 NewTime)
{
	PlayerDisplay->UpdateRemainingTime(NewTime);
}
