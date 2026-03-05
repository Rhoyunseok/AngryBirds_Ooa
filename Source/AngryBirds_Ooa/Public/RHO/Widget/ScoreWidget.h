// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

/**
 * gamestate 에서 점수 정보를 받아서 화면에 표시하는 위젯
 * - 점수는 게임 플레이 중에 계속 업데이트되어야 하므로, GameState에서 점수가 변경될 때마다 ScoreWidget에 업데이트를 전달하는 방식으로 구현할 예정
 */
UCLASS()
class ANGRYBIRDS_OOA_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 점수 텍스트를 바인딩할 변수 (블루프린트에서 TextBlock을 배치하고 이름을 ScoreText로 맞춰야 연결됨)
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Score;
	
	

	
	void UpdateScore(int32 NewScore);
};
