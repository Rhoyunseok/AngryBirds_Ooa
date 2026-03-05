// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/BirdWidget.h"
#include "Components/TextBlock.h" // UTextBlock을 사용하기 위해 필수 포함

void UBirdWidget::UpdateBirdCount(int32 Remaining, int32 Total)
{
	// 블루프린트의 텍스트 블록이 잘 연결되었는지 확인
	if (BirdCountText)
	{
		// FString::Printf를 사용해 "남은수 / 전체수" 형태의 문자열 생성
		FString CountString = FString::Printf(TEXT("%d / %d"), Remaining, Total);
        
		// FString을 FText로 변환하여 텍스트 블록에 세팅
		BirdCountText->SetText(FText::FromString(CountString));
	}
}