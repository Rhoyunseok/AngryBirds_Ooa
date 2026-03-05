// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "YB_GameMode.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, float);
DECLARE_MULTICAST_DELEGATE(FOnAllPigDestroyedSignature);

UCLASS()
class ANGRYBIRDS_OOA_API AYB_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FOnScoreChangedSignature OnScoreChanged;
	FOnAllPigDestroyedSignature OnAllPigDestroyed;
	
	void AddScore(int score);
	void RegisterPig();
	void PigDestroyed();
	
	float totalScore = 0;
	int remainingPigs = 0;
};
