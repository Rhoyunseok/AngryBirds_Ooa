// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/BackToStageSelectButton.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UBackToStageSelectButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼이 눌렸을 때 실행할 함수들을 연결
	if (UButton* BackToStageSelectButton = Cast<UButton>(GetWidgetFromName(TEXT("WBP_BackToStageSelectButton"))))
	{
		BackToStageSelectButton->OnClicked.AddDynamic(this, &UBackToStageSelectButton::OnBackToStageSelectClicked);
	}
}

void UBackToStageSelectButton::OnBackToStageSelectClicked()
{
	// "StageSelect" 라는 이름의 맵(레벨)으로 이동! (맵 이름 스펠링 주의)
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}

void UBackToStageSelectButton::OnQuitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}
