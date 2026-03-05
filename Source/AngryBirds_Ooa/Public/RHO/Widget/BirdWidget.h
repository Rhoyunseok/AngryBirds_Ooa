// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BirdWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UBirdWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BirdCountText; // 새 수를 표시할 텍스트 블록 (블루프린트에서 배치하고 이름을 BirdCountText로 맞춰야 연결됨)
	
	void UpdateBirdCount(int32 Remaining, int32 Total);
};
