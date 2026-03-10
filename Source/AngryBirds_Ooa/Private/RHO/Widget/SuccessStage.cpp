#include "RHO/Widget/SuccessStage.h"
#include "RHO/Widget/StarWidget.h" // StarWidget 헤더 포함 필수
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"

void USuccessStage::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (Btn_NextStage) Btn_NextStage->OnClicked.AddDynamic(this, &USuccessStage::OnNextStageClicked);
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &USuccessStage::OnRestartClicked);
	if (Btn_StageSelect) Btn_StageSelect->OnClicked.AddDynamic(this, &USuccessStage::OnStageSelectClicked);

	// 점수 및 별점 표시 로직
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		int32 MyScore = GameState->GetCurrentScore();
		
		// GameState에 별 개수를 계산하는 함수가 있다면 호출하세요. 
		// 여기서는 예시로 점수에 따라 별을 설정합니다.
		int32 MyStars = 0;
		if (MyScore >= 3000) MyStars = 3;
		else if (MyScore >= 1500) MyStars = 2;
		else MyStars = 1;

		// 텍스트 업데이트
		if (Txt_Score)
			Txt_Score->SetText(FText::Format(NSLOCTEXT("UI", "ScoreText", "SCORE : {0}"), FText::AsNumber(MyScore)));

		if (Txt_Stars)
			Txt_Stars->SetText(FText::Format(NSLOCTEXT("UI", "StarsText", "STARS : {0}"), FText::AsNumber(MyStars)));

		// ★ 스타 위젯 업데이트 실행 ★
		if (WBP_StarWidget)
		{
			WBP_StarWidget->UpdateStars(MyStars, true);
		}
	}
}

void USuccessStage::OnNextStageClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Stage1_1"));
}

void USuccessStage::OnRestartClicked()
{
	FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}

void USuccessStage::OnStageSelectClicked()
{
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}