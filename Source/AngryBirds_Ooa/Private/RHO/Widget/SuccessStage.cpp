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
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this)); // 게임 상태에서 점수와 별 개수를 가져옵니다.
	if (GameState)
	{
		int32 MyScore = GameState->GetCurrentScore();
		int32 MyStars = GameState->GetCurrentStars();

		// 텍스트 업데이트
		if (Txt_Score)
			Txt_Score->SetText(FText::Format(NSLOCTEXT("UI", "ScoreText", "SCORE : {0}"), FText::AsNumber(MyScore)));
		if (Txt_Stars)
			Txt_Stars->SetText(FText::Format(NSLOCTEXT("UI", "StarsText", "STARS : {0}"), FText::AsNumber(MyStars)));
		 if (WBP_StarWidget)
		 {
		 	// 몇초 뒤에 별이 채워지는 애니메이션이 실행되도록 타이머 설정
		 	FTimerHandle StarAnimTimerHandle;
		 	GetWorld()->GetTimerManager().SetTimer(StarAnimTimerHandle, [this, MyStars]()
		 	{
		 		WBP_StarWidget->UpdateStars(MyStars, true);
		 	}, 1.0f, false); // 1초 뒤에 실행, 반복 안함
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