// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/Stage1_1_LevelScriptActor.h"

#include "SlingShot.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"
#include "Blueprint/UserWidget.h"

void AStage1_1_LevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	PigNum = 4;
	StageInfo = "Stage1_1";
	// GameState 전송하기
	
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		// 2. GameState의 함수를 호출해서 돼지 수를 전달합니다! // pi
		GameState->SetTotalPigs(PigNum);
        
		// 여기서 사용할 수 있는 새의 수도 전달하고 싶다면 똑같이 호출하면 됩니다.
		GameState->SetTotalBirds(4); 
		
		// Stage Info 
		GameState->SetStageInfo(StageInfo);
		
		// 새 
		GameState->SetBirdQueue(LevelBirds);
		GameState->OnGameCleared.AddDynamic(this, &AStage1_1_LevelScriptActor::OnGameSuccess);
		GameState->OnGameOver.AddDynamic(this, &AStage1_1_LevelScriptActor::OnGameFail);
		
		
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

void AStage1_1_LevelScriptActor::OnGameSuccess()
{
	if (SuccessWidgetClass)
	{
		// 1. 위젯 생성
		UUserWidget* SuccessWidget = CreateWidget<UUserWidget>(GetWorld(), SuccessWidgetClass);
		if (SuccessWidget)
		{
			// 2. 화면에 띄우기
			SuccessWidget->AddToViewport();

			// 3. (중요) 마우스 커서를 보이게 하고, 마우스가 UI만 누르도록 설정!
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly()); // 게임 조작(새총 당기기 등) 막기
			}
		}
	}
}

void AStage1_1_LevelScriptActor::OnGameFail()
{
	if (FailWidgetClass)
	{
		UUserWidget* FailWidget = CreateWidget<UUserWidget>(GetWorld(), FailWidgetClass);
		if (FailWidget)
		{
			FailWidget->AddToViewport();

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly()); 
			}
		}
	}
}
