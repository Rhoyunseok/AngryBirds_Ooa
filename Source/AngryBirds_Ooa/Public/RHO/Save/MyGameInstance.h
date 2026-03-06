// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMyGameInstance();
	
	virtual void Init() override;
	
	UPROPERTY()
	class UAngryBirdSaveGame* CurrentSaveData;
	
	FString SaveSlotName = TEXT("PlayerSaveSlot");
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame();
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame();

	// 3. 스테이지를 클리어했을 때 별점 기록하기 (GameState가 부를 함수)
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveStageClearData(FString StageName, int32 Stars);

	// 4. 스테이지 선택 창에서 특정 맵의 별점 물어보기 (UI가 부를 함수)
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	int32 GetStageClearStars(FString StageName);
	
};
