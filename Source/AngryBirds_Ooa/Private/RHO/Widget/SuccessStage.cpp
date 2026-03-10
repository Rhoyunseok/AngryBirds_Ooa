#include "RHO/Widget/SuccessStage.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"

void USuccessStage::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (Btn_NextStage) 
		Btn_NextStage->OnClicked.AddDynamic(this, &USuccessStage::OnNextStageClicked);
    
	if (Btn_Restart) 
		Btn_Restart->OnClicked.AddDynamic(this, &USuccessStage::OnRestartClicked);

	if (Btn_StageSelect) 
		Btn_StageSelect->OnClicked.AddDynamic(this, &USuccessStage::OnStageSelectClicked);

	// 점수 및 별점 표시 로직
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		int32 MyScore = GameState->GetCurrentScore();
		int32 MyStars = 1; // 필요시 GameState에서 별 개수 가져오는 로직 추가

		if (Txt_Score)
		{
			Txt_Score->SetText(FText::Format(NSLOCTEXT("UI", "ScoreText", "SCORE : {0}"), FText::AsNumber(MyScore)));
		}
        
		if (Txt_Stars)
		{
			Txt_Stars->SetText(FText::Format(NSLOCTEXT("UI", "StarsText", "STARS : {0}"), FText::AsNumber(MyStars)));
		}
	}
}

void USuccessStage::OnNextStageClicked()
{
	// 지정된 다음 스테이지(Stage1_1)로 이동
	//UGameplayStatics::OpenLevel(this, NextLevelName);
	UGameplayStatics::OpenLevel(this, FName("Stage1_1"));
}

void USuccessStage::OnRestartClicked()
{
	// 현재 활성화된 레벨의 이름을 가져와서 다시 엽니다 (재시작)
	FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}

void USuccessStage::OnStageSelectClicked()
{
	// 스테이지 선택 화면으로 이동
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}