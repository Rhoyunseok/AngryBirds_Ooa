// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/FailStage.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UFailStage::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯이 켜질 때, 버튼에 클릭 이벤트를 달아줍니다!
	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddDynamic(this, &UFailStage::OnRetryClicked);
	}
	if (Btn_StageSelect)
	{
		Btn_StageSelect->OnClicked.AddDynamic(this, &UFailStage::OnStageSelectClicked);
	}
}
void UFailStage::OnRetryClicked()
{
	// 현재 열려있는 맵 이름을 가져와서 그 맵을 다시 엽니다 (재시작!)
	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}

void UFailStage::OnStageSelectClicked()
{
	// 스테이지 선택 화면 맵 이름을 정확히 적어주세요! (예: "StageSelectMap")
	UGameplayStatics::OpenLevel(this, FName("StageSelect")); 
}