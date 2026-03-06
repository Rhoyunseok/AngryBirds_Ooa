// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/Stage1_1_LevelScriptActor.h"

#include "SlingShot.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"

void AStage1_1_LevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	PigNum = 2;
	StageInfo = "Tutorial Map";
	// GameState 전송하기
	
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		// 2. GameState의 함수를 호출해서 돼지 수를 전달합니다! // pi
		GameState->SetTotalPigs(PigNum);
        
		// 여기서 사용할 수 있는 새의 수도 전달하고 싶다면 똑같이 호출하면 됩니다.
		GameState->SetTotalBirds(3); 
		
		// Stage Info 
		GameState->SetStageInfo(StageInfo);
		
		// 새 
		GameState->SetBirdQueue(LevelBirds);
		
		AActor* FoundSlingshot = UGameplayStatics::GetActorOfClass(GetWorld(), ASlingShot::StaticClass());
		if (ASlingShot* Slingshot = Cast<ASlingShot>(FoundSlingshot))
		{
			Slingshot->LoadBird(); // "새총아! 내가 새 채워놨으니까 이제 장전해!"
		}

		UE_LOG(LogTemp, Warning, TEXT("GameState에 데이터 전달 성공! 돼지 수: %d"), PigNum);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameState를 찾을 수 없습니다! 월드 세팅을 확인하세요."));
	}
}

void AStage1_1_LevelScriptActor::ShowLevelInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("PigNum: %d, StageInfo: %s"), PigNum, *StageInfo);
}
