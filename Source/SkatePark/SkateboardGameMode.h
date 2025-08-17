// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SkateboardGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateMatchTime, int32, NewTime);

/**
 * 
 */
UCLASS()
class SKATEPARK_API ASkateboardGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int32 GetMatchDuration() const { return MatchDuration; }

	virtual void StartMatch() override;

	FOnUpdateMatchTime OnUpdateMatchTime;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 MatchDuration = 180;
	
private:
	UPROPERTY()
	FTimerHandle TimerHandle;

	UFUNCTION()
	void UpdateMatchTimer();
	
	int32 CurrentMatchTime;
};
