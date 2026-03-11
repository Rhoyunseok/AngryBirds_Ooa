// Fill out your copyright notice in the Description page of Project Settings.

#include "RHO/AngryBirdGameState.h"

#include "Base_Bird.h"
#include "IPlatformFilePak.h"
#include "Kismet/GameplayStatics.h"
#include "Rho/Save/MyGameInstance.h"



AAngryBirdGameState::AAngryBirdGameState()
{
	// 초기값 설정
}



void AAngryBirdGameState::AddScore(int32 Amount)
{
	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore); // 점수 변경 이벤트 브로드캐스트
}

void AAngryBirdGameState::SetTotalPigs(int32 Count)
{
	TotalPigs = Count;
	RemainingPigs = Count;
	OnPigsChanged.Broadcast(RemainingPigs, TotalPigs);
}

void AAngryBirdGameState::DecreasePigCount()
{
	RemainingPigs = FMath::Max(0, RemainingPigs - 1);
	OnPigsChanged.Broadcast(RemainingPigs, TotalPigs);
    
	// 만약 여기서 돼지가 0마리가 되면 승리 이벤트를 보낼 수도 있습니다.
	if (RemainingPigs==0)
	{
		CheckMatchState();
	}
}

void AAngryBirdGameState::SetTotalBirds(int32 Count)
{
	TotalBirds = Count;
	RemainingBirds = Count;
	OnBirdsChanged.Broadcast(RemainingBirds , TotalBirds);
}

void AAngryBirdGameState::UseBird()
{
	RemainingBirds = FMath::Max(0, RemainingBirds - 1);
	OnBirdsChanged.Broadcast(RemainingBirds, TotalBirds);
}

void AAngryBirdGameState::UpdateStars(int32 NewStars)
{
	if (CurrentStars != NewStars)
	{
		CurrentStars = NewStars;
		OnStarsChanged.Broadcast(CurrentStars);
	}
}

void AAngryBirdGameState::SetStageInfo(const FString& NewStageInfo)
{
	// 스테이지 정보가 변경되었을 때만 업데이트 및 이벤트 브로드캐스트
	if (CurrentStageInfo != NewStageInfo)
	{
		CurrentStageInfo = NewStageInfo;
		OnStageInfoChanged.Broadcast(CurrentStageInfo);
	}
}

void AAngryBirdGameState::SetBirdQueue(const TArray<TSubclassOf<class AActor>>& NewBirdQueue)
{
	// 1. 전달받은 새 종류와 순서를 GameState의 대기열(BirdQueue)에 저장
	BirdQueue = NewBirdQueue;
    
	// 2. 굳이 SetTotalBirds를 따로 부를 필요 없이, 배열의 길이(Num)가 곧 총 새 마리수!
	TotalBirds = BirdQueue.Num();
	RemainingBirds = TotalBirds;
    
	// 3. UI(위젯)에 숫자 바뀌었다고 방송 때리기
	OnBirdsChanged.Broadcast(RemainingBirds, TotalBirds);
}

TSubclassOf<class AActor> AAngryBirdGameState::GetNextBird()
{
	// 1. 만약 대기열이 텅 비었다면 줄 새가 없으므로 nullptr 반환 (게임 끝 판정용)
	if (BirdQueue.IsEmpty())
	{
		return nullptr;
	}

	// 2. 대기열의 맨 앞(0번 인덱스)에 있는 새 클래스를 기억해둠
	TSubclassOf<AActor> NextBirdClass = BirdQueue[0];
    
	// 3. 방금 꺼낸 새를 배열에서 삭제! (뒤에 있던 새들이 자동으로 앞으로 한 칸씩 당겨짐)
	BirdQueue.RemoveAt(0);

	// 4. 남은 새 마리수를 1 깎고 UI에 반영 (기존에 만들어둔 UseBird() 재활용)
	UseBird();

	// 5. 새총에게 "자, 이 새 스폰해서 장전해라!" 하고 돌려줌
	return NextBirdClass;
}

void AAngryBirdGameState::CheckMatchState()
{
	if (bIsGameOver) return; 

	// 승리 조건
	if (RemainingPigs <= 0)
	{
		
		bIsGameOver = true;

		UE_LOG(LogTemp, Warning, TEXT("모든 돼지 처치! 4초 뒤에 승리 창이 뜹니다..."));
		
		// 4초 뒤 게임 승리
		GetWorldTimerManager().SetTimer(VictoryTimerHandle, this, &AAngryBirdGameState::ProcessVictory, 4.0f, false);
	}
	
	// 패배 조건
	else if (RemainingPigs > 0 && RemainingBirds <= 0)
	{
		if (ActiveBirdsOnField == 0)
		{
			bIsGameOver = true;
			GetWorldTimerManager().SetTimer(VictoryTimerHandle, this, &AAngryBirdGameState::ProcessFailure, 4.0f, false);
			UE_LOG(LogTemp, Warning, TEXT("졌어졌어졌어졌어졌어"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("졌을때 방어 코드"));
		}
	}
}
// 4초 뒤에 타이머에 의해 자동으로 실행되는 함수
void AAngryBirdGameState::ProcessVictory()
{
	int32 EarnedStars = 1;
	// 튜토리얼 점수 산정
	if (CurrentStageInfo == TEXT("Tutorial Map"))
	{
		// 1. 별 개수 동적 계산 (예: 남은 새 마릿수를 기준으로)
		 // 기본 별 1개 (클리어 보장)

		if (CurrentScore >= 5000)
		{
			EarnedStars = 3; // 5천점 이상이면 ★★★
		}
		else if (CurrentScore >= 3000)
		{
			EarnedStars = 2; // 3천점 이상이면 ★★☆
		}
		
	} 
	else if (CurrentStageInfo == TEXT("Stage1_1"))
	{
		
		if (CurrentScore >= 8500)
		{
			EarnedStars = 3; // 3천점 이상이면 ★★★
		}
		else if (CurrentScore >= 6000)
		{
			EarnedStars = 2; // 2천점 이상이면 ★★☆
		}
	}
	UpdateStars(EarnedStars);
	// 2. 게임 인스턴스 가져와서 저장
	UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGI)
	{
		// 핵심 디버깅 포인트] 만약 스테이지 이름이 비어있다면, 저장이 꼬이므로 경고를 띄우고 임시 이름을 줍니다.
		if (CurrentStageInfo.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("경고: AngryBird GameState 문제 생김 제대로 저장 안됨"));
			CurrentStageInfo = TEXT("Tutorial Map"); 
		}

		// 저장하기 직전에 로그로 한 번 더 확인
		UE_LOG(LogTemp, Warning, TEXT("저장 시도 이름: [%s] GameState"), *CurrentStageInfo);
		MyGI->SaveStageClearData(CurrentStageInfo, EarnedStars);
	}

	// 3. UI 띄우기
	OnGameCleared.Broadcast();
}

void AAngryBirdGameState::ProcessFailure()
{
	OnGameOver.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("졌어졌어졌어졌어졌어"));
}