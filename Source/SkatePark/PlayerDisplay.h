// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDisplay.generated.h"

/**
 * 
 */
UCLASS()
class SKATEPARK_API UPlayerDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayNewScore(int32 TotalScore, int32 NewScore, const FString& ScoreMessage);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRemainingTime(int32 Time);
};
