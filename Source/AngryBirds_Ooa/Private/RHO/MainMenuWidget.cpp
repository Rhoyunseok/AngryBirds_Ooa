// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" // 게임 종료를 위한 함수가 있는 헤더

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼이 눌렸을 때 실행할 함수들을 연결
	if (Btn_StageSelect)
	{
		Btn_StageSelect->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStageSelectClicked);
	}
    
	if (Btn_Options)
	{
		Btn_Options->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
	}
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}
}
void UMainMenuWidget::OnStageSelectClicked()
{
	// "StageSelect" 라는 이름의 맵(레벨)으로 이동! (맵 이름 스펠링 주의)
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}

void UMainMenuWidget::OnOptionsClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Options")); // 옵션 맵 이름
}

void UMainMenuWidget::OnQuitClicked()
{
	// 플레이어 컨트롤러 가져오기 (게임 종료 함수에 필요)
	APlayerController* PC = GetOwningPlayer();
	// 게임 종료
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}
