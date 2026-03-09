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
        // 바구니의 내용을 하드디스크에 물리적으로 덮어씌워서 저장!
        UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveData, SaveSlotName, 0); // 비동기 형식으로 저장
        UE_LOG(LogTemp, Warning, TEXT("게임 진행 상황이 비동기로 저장되었습니다!"));
    }
}

void UMyGameInstance::SaveStageClearData(FString StageName, int32 Stars)
{
    if (!CurrentSaveData) return;

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
    if (CurrentSaveData && CurrentSaveData->StageClearRecords.Contains(StageName))
    {
        // 기록이 있으면 그 별 개수를 돌려줌
        return CurrentSaveData->StageClearRecords[StageName];
    }
    // 한 번도 깬 적 없는 맵이면 별 0개!
    return 0;
}