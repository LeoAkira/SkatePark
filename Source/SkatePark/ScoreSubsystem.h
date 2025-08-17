// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ScoreSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnScored, const int32, TotalScore, const int32, NewScore, const FString&, ScoreMessage);

/**
 * 
 */
UCLASS()
class SKATEPARK_API UScoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddScore(const int32 Score, const FString& ScoreMessage);

	UFUNCTION(BlueprintCallable)
	int32 GetScore() const { return CurrentScore; }
	
	UPROPERTY()
	FOnScored OnScored;
private:
	int32 CurrentScore;
};
