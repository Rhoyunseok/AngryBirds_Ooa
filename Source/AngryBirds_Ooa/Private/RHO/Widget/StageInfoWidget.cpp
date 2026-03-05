// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/StageInfoWidget.h"
#include "Components/TextBlock.h" // UTextBlock을 사용하기 위해 필수 포함

void UStageInfoWidget::UpdateStageInfo(const FString& NewStageInfo)
{
	// 블루프린트의 텍스트 블록이 잘 연결되었는지 확인
	if (StageInfoText)
	{
		// FString을 FText로 변환하여 텍스트 블록에 세팅
		StageInfoText->SetText(FText::FromString(NewStageInfo));
	}
}