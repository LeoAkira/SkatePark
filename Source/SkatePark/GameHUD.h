// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UEndGameDisplay;
class UPlayerDisplay;
/**
 * 
 */
UCLASS()
class SKATEPARK_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitializeHUD();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerDisplay> PlayerDisplayClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEndGameDisplay> EndGameDisplayClass;
private:
	
	UPROPERTY()
	UPlayerDisplay* PlayerDisplay;

	UPROPERTY()
	UEndGameDisplay* EndGameDisplay;
	
	UFUNCTION()
	void OnScored(const int32 TotalScore, const int32 NewScore, const FString& ScoreMessage);

	UFUNCTION()
	void UpdateTimer(int32 NewTime);

	UFUNCTION()
	void OnMatchFinished();
};
