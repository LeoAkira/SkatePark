// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreSubsystem.h"

void UScoreSubsystem::AddScore(const int32 Score, const FString& ScoreMessage)
{
	CurrentScore += Score;
	OnScored.Broadcast(CurrentScore, Score, ScoreMessage);
}
