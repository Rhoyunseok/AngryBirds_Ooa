// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AngryBirdSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UAngryBirdSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UAngryBirdSaveGame();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	TMap<FString, int32> StageClearRecords;
	
};
