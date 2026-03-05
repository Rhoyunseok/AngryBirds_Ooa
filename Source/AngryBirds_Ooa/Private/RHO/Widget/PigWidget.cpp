// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/PigWidget.h"
#include "Components/TextBlock.h"

void UPigWidget::UpdatePigCount(int32 Remaining, int32 Total)
{
	if (PigCountText)
	{
		// FString::Printf를 사용해 "남은수 / 전체수" 형태의 문자열(FString)을 만듭니다.
		FString CountString = FString::Printf(TEXT("%d / %d"), Remaining, Total);
        
		// 만들어진 FString을 언리얼 UI가 쓰는 FText로 변환해서 텍스트 블록에 세팅합니다.
		PigCountText->SetText(FText::FromString(CountString));
	}
}