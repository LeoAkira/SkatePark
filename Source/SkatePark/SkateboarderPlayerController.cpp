// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboarderPlayerController.h"

#include "GameHUD.h"

void ASkateboarderPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Cast<AGameHUD>(GetHUD())->InitializeHUD();
}