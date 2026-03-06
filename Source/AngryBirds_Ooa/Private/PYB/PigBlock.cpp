// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/PigBlock.h"
#include "RHO/AngryBirdGameState.h"
#include "Kismet/GameplayStatics.h"


APigBlock::APigBlock()
{
	BirdThreshold = 10.0f;
}

void APigBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

void APigBlock::BeforeBlockDestory()
{
	Super::BeforeBlockDestory();
	UE_LOG(LogTemp, Warning, TEXT("PigDie"));
    
	// 1. 현재 월드의 GameState를 가져와서 우리가 만든 AAngryBirdGameState로 캐스팅
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
    
	// 2. 캐스팅에 성공했다면?
	if (GameState)
	{
		// 3. GameState에게 돼지가 죽었다고 알림 (여기서 UI 변경 방송이 자동으로 나갑니다!)
		GameState->DecreasePigCount();
        
		// (선택) 점수도 올리고 싶다면 여기서 호출하세요!
		// GameState->AddScore(500); 
	}

	// 돼지 자체의 델리게이트 실행 (이펙트나 사운드 재생용으로 쓰기 좋습니다)
	OnPigDestroyed.Broadcast();
}