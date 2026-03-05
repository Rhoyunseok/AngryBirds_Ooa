// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/PigBlock.h"
#include "PYB/YB_LevelScriptActor.h"


void APigBlock::BeginPlay()
{
	Super::BeginPlay();
	
	// 레벨 스크립트 직접 가져와서 PigNum 계산
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
	// GameState 에 돼지 수 감소 이벤트 보내기
	UWorld* CurrentStage = GetWorld();
	if (CurrentStage)
	{
		AYB_LevelScriptActor* LSA = Cast<AYB_LevelScriptActor>(CurrentStage->GetLevelScriptActor());
		if (LSA)		{
			LSA->PigNum--;
			LSA->ShowLevelInfo();
		}
	}
	OnPigDestroyed.Broadcast();
}
