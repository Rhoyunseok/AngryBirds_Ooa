// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/StageSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UStageSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_BackToMainMenu)
	{
		Btn_BackToMainMenu->OnClicked.AddDynamic(this, &UStageSelectWidget::OnBackToMainMenuClicked);
	}
	
	if (Btn_Tutorial)
	{
		Btn_Tutorial->OnClicked.AddDynamic(this, &UStageSelectWidget::OnTutorialClicked);
	}
	if (Btn_Stage1_1)
	{
		Btn_Stage1_1->OnClicked.AddDynamic(this, &UStageSelectWidget::OnStage1_1Clicked);
	}
	
}

void UStageSelectWidget::OnBackToMainMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UStageSelectWidget::OnTutorialClicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestLevel")); // 튜토리얼 맵 이름
}

void UStageSelectWidget::OnStage1_1Clicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestLevel")); // 스테이지 1-1 맵 이름
}