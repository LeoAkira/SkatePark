// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameDisplay.generated.h"

/**
 * 
 */
UCLASS()
class SKATEPARK_API UEndGameDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowEndGame(int32 FinalScore);
};
