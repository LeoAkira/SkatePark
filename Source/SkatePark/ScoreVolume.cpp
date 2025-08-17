// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreVolume.h"

#include "ScoreSubsystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AScoreVolume::AScoreVolume()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(BoxComponent);

	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AScoreVolume::OnBeginOverlap);
}

void AScoreVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		if (UScoreSubsystem* ScoreSubsystem = GetGameInstance()->GetSubsystem<UScoreSubsystem>())
		{
			FString Message = ScoreMessage.ToString();
			ScoreSubsystem->AddScore(Score, Message);
		}
	}
}

