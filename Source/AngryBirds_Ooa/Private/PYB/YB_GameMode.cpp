// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/YB_GameMode.h"

void AYB_GameMode::AddScore(int score)
{
	totalScore += score;
	OnScoreChanged.Broadcast(totalScore);
}

void AYB_GameMode::RegisterPig()
{
	remainingPigs++;
}

void AYB_GameMode::PigDestroyed()
{
	remainingPigs--;
	if (remainingPigs <= 0)
	{
		OnAllPigDestroyed.Broadcast();
	}
}
