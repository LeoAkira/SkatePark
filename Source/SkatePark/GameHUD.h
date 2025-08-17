// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

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
private:
	
	UPROPERTY()
	UPlayerDisplay* PlayerDisplay;
	
	UFUNCTION()
	void OnScored(const int32 TotalScore, const int32 NewScore, const FString& ScoreMessage);

	UFUNCTION()
	void UpdateTimer(int32 NewTime);
};
