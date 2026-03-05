// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/PigBlock.h"

#include "PYB/YB_LevelScriptActor.h"


void APigBlock::BeginPlay()
{
	Super::BeginPlay();
	UWorld* CurrentStage = GetWorld();
	if (CurrentStage)
	{
		AYB_LevelScriptActor* LSA = Cast<AYB_LevelScriptActor>(CurrentStage->GetLevelScriptActor());
		if (LSA)
		{
			LSA->PigNum++;
			LSA->ShowLevelInfo();
		}
	}
}

void APigBlock::BeforeBlockDestory()
{
	Super::BeforeBlockDestory();
	UE_LOG(LogTemp, Warning, TEXT("PigDie"));
	OnPigDestroyed.Broadcast();
}
