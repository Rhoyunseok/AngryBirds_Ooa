// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Save/MyGameInstance.h"
#include "RHO/Save/AngryBirdSaveGame.h" // 본인의 SaveGame 클래스 경로에 맞게 수정!
#include "Kismet/GameplayStatics.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
    Super::Init();
    // 게임이 켜지자마자 하드디스크에서 기록을 읽어옵니다!
    LoadGame();
}

void UMyGameInstance::LoadGame()
{
    // 1. 하드디스크에 SaveSlotName("PlayerSaveSlot") 이름의 파일이 있는지 확인
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
        // 2. 있으면 읽어와서 내 바구니(CurrentSaveData)에 담기
        CurrentSaveData = Cast<UAngryBirdSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
        UE_LOG(LogTemp, Warning, TEXT("기존 세이브 파일을 성공적으로 불러왔습니다!"));
    }
    else
    {
        // 3. 없으면(처음 게임을 켰다면) 텅 빈 새 바구니 만들기
        CurrentSaveData = Cast<UAngryBirdSaveGame>(UGameplayStatics::CreateSaveGameObject(UAngryBirdSaveGame::StaticClass()));
        UE_LOG(LogTemp, Warning, TEXT("새로운 세이브 파일을 생성했습니다!"));
    }
}

void UMyGameInstance::SaveGame()
{
    if (CurrentSaveData)
    {
        // Async 대신 일반 SaveGameToSlot 사용 (즉시 저장)
        bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveData, SaveSlotName, 0);
        
        if(bSuccess) {
            UE_LOG(LogTemp, Warning, TEXT("!!! 즉시 저장 성공 !!!"));
        } else {
            UE_LOG(LogTemp, Error, TEXT("!!! 저장 실패 (슬롯 이름 확인 필요) !!!"));
        }
    }
}

void UMyGameInstance::SaveStageClearData(FString StageName, int32 Stars)
{
    if (!CurrentSaveData) return;

    UE_LOG(LogTemp, Warning, TEXT("저장 시도 이름: [%s], 별 개수: %d"), *StageName, Stars);
    // 기존 기록이 있는지 확인
    if (CurrentSaveData->StageClearRecords.Contains(StageName))
    {
        // 만약 예전에 별 1개로 깼는데, 이번에 별 3개로 깼다면 갱신! (최고 기록 유지)
        int32 OldStars = CurrentSaveData->StageClearRecords[StageName];
        if (Stars > OldStars)
        {
            CurrentSaveData->StageClearRecords[StageName] = Stars;
            SaveGame(); // 갱신했으니 하드디스크에 저장
        }
    }
    else
    {
        // 처음 깨는 스테이지라면 그냥 등록하고 저장!
        CurrentSaveData->StageClearRecords.Add(StageName, Stars);
        SaveGame();
    }
}

int32 UMyGameInstance::GetStageClearStars(FString StageName)
{
    if (!CurrentSaveData)
    {
        LoadGame();
    }

    if (CurrentSaveData && CurrentSaveData->StageClearRecords.Contains(StageName))
    {
        int32 StarCount = CurrentSaveData->StageClearRecords[StageName];
        UE_LOG(LogTemp, Warning, TEXT("====> [성공] %s 데이터 찾음: %d 개"), *StageName, StarCount);
        return StarCount;
    }

    UE_LOG(LogTemp, Error, TEXT("====> [실패] %s 데이터를 찾을 수 없음!"), *StageName);
    return 0;
}