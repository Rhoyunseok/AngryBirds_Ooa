// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/StarWidget.h"
#include "Components/Image.h" // UImage를 사용하기 위해 필수 포함

void UStarWidget::UpdateStars(int32 StarCount)
{
	// StarCount는 보통 0, 1, 2, 3 중 하나입니다.
    
	// 첫 번째 별: 1개 이상일 때 켜짐
	if (Star_1)
	{
		Star_1->SetRenderOpacity(StarCount >= 1 ? 1.0f : 0.2f);
	}

	// 두 번째 별: 2개 이상일 때 켜짐
	if (Star_2)
	{
		Star_2->SetRenderOpacity(StarCount >= 2 ? 1.0f : 0.2f);
	}

	// 세 번째 별: 3개일 때 켜짐
	if (Star_3)
	{
		Star_3->SetRenderOpacity(StarCount >= 3 ? 1.0f : 0.2f);
	}
}