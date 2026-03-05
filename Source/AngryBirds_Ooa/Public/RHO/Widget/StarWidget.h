// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UStarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 블루프린트에서 만들 Image 컴포넌트의 이름과 완벽히 일치해야 합니다.
	UPROPERTY(meta = (BindWidget))
	class UImage* Star_1;

	UPROPERTY(meta = (BindWidget))
	class UImage* Star_2;

	UPROPERTY(meta = (BindWidget))
	class UImage* Star_3;
	
	// StagePlayingWidget이 호출해서 0~3 사이의 별 개수를 전달해 줄 함수
	void UpdateStars(int32 StarCount);
};
