// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStartUserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameStartUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GameStartButton)
	{
		GameStartButton->OnClicked.AddDynamic(this, &UGameStartUserWidget::OnGameStartButtonClicked);
	}
}

void UGameStartUserWidget::OnGameStartButtonClicked()
{
	
	// 현재 위젯 제거 (게임이 시작되면 메인 메뉴 위젯이 사라지도록)
	this->RemoveFromParent();
	
	// 게임 시작 버튼이 클릭되면 "TestLevel"이라는 이름의 레벨로 이동합니다.
	UGameplayStatics::OpenLevel(this, FName("TestLevel"));
}