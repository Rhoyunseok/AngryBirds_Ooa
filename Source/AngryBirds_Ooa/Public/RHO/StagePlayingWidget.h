// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "StagePlayingWidget.generated.h"

/**
 * 현재 스테이지의 정보
 * - 점수
 * - 현재 스테이지에서 사용 가능한 새의 종류 및 개수
 * - 남은 새의 수
 * - 현재 스테이지의 별 개수
 * - 스테이지에 처음 배치된 총 돼지의 수
 * - 현재 남아있는 돼지의 수
 * 
 * - 점수 위젯은 ScoreWidget으로 따로 만들어서 StagePlayingWidget에 배치하는 방식으로 구현할 예정
 * - 남은 새의 수는 BirdWidget으로 따로 만들어서 StagePlayingWidget에 배치하는 방식으로 구현할 예정
 * - 별의 개수는 StarWidget으로 따로 만들어서 StagePlayingWidget에 배치하는 방식으로 구현할 예정
 * - 돼지의 수는 PigWidget으로 따로 만들어서 StagePlayingWidget에 배치하는 방식으로 구현할 예정
 */
// GameState 에서 정보를 받는 방법은 Delegate를 활용하는 방식으로 구현할 예정, 코드 위치는 StagePlayingWidget.cpp 참고
UCLASS()
class ANGRYBIRDS_OOA_API UStagePlayingWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:
	// 하위 위젯 바인딩
	UPROPERTY(meta = (BindWidget))
	class UScoreWidget* WBP_ScoreWidget;
	
	UPROPERTY(meta = (BindWidget))
	class UBirdWidget* WBP_BirdWidget;
	
	UPROPERTY(meta = (BindWidget))
	class UStarWidget* WBP_StarWidget;
	
	UPROPERTY(meta = (BindWidget))
	class UPigWidget* WBP_PigWidget;
	
	// 현재 스테이지 정보도 표시할 예정이라면 여기에 UPROPERTY로 선언하세요! (예: UTextBlock* WBP_StageInfoText;)
	UPROPERTY(meta = (BindWidget))
	class UStageInfoWidget* WBP_StageInfoWidget;
	
	// StageSelect 화면으로 전환 버튼
	UPROPERTY(meta = (BindWidget))
	class UBackToStageSelectButton* WBP_BackToStageSelectButton;
	
	// -- 생명 주기
	virtual void NativeConstruct() override;
private:
	// GameState와 연결해서 점수, 새의 수, 별의 개수, 돼지의 수 등을 업데이트하는 함수들
	UFUNCTION()
	void UpdateAllUI(int32 Score, int32 RemainingPigs, int32 TotalPigs, int32 RemainingBirds, int32 TotalBirds, int32 Stars); // 새 관련 파라미터 2개 추가

	UFUNCTION()
	void OnScoreChanged(int32 NewScore);

	UFUNCTION()
	void OnPigsChanged(int32 RemainingPigs, int32 TotalPigs);
    
	UFUNCTION()
	void OnBirdsChanged(int32 RemainingBirds, int32 TotalBirds);
    
	UFUNCTION()
	void OnStarsChanged(int32 Stars);
	
	UFUNCTION()
	void OnStageInfoChanged(FString NewStageInfo);
	
	// StageSelect 화면으로 돌아가는 버튼 클릭 이벤트 핸들러
	UFUNCTION()
	void OnBackToStageSelectClicked();
	
	
};
