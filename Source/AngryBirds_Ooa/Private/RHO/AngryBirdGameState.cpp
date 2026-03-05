// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/AngryBirdGameState.h"

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
}

void AAngryBirdGameState::SetTotalBirds(int32 Count)
{
	TotalBirds = Count;
	RemainingBirds = Count;
	OnBirdsChanged.Broadcast(RemainingBirds, TotalBirds);
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