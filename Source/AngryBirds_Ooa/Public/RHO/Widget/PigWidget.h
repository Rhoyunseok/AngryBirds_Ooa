// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PigWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UPigWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 블루프린트와 연결용
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PigCountText; // 돼지 수를 표시할 텍스트 블록 (블루프린트에서 배치하고 이름을 PigCountText로 맞춰야 연결됨)
	
	void UpdatePigCount(int32 Remaining, int32 Total);
};
