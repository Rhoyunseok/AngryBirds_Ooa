#include "RHO/Widget/SuccessStage.h"
#include "RHO/Widget/StarWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"
#include "Sound/SoundBase.h"

void USuccessStage::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯이 생성되자마자 성공 사운드 재생
	if (SuccessSound)
	{
		UGameplayStatics::PlaySound2D(this, SuccessSound);
	}
	
	// 버튼 이벤트 바인딩
	if (Btn_NextStage) Btn_NextStage->OnClicked.AddDynamic(this, &USuccessStage::OnNextStageClicked);
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &USuccessStage::OnRestartClicked);
	if (Btn_StageSelect) Btn_StageSelect->OnClicked.AddDynamic(this, &USuccessStage::OnStageSelectClicked);

	// 현재 스테이지의 점수 및 별점 가져오기
	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		int32 MyScore = GameState->GetCurrentScore();
		int32 MyStars = GameState->GetCurrentStars();

		// 텍스트 업데이트
		if (Txt_Score)
			Txt_Score->SetText(FText::Format(NSLOCTEXT("UI", "ScoreText", "{0}"), FText::AsNumber(MyScore)));
       
		if (Txt_Stars)
			Txt_Stars->SetText(FText::Format(NSLOCTEXT("UI", "StarsText", "{0}"), FText::AsNumber(MyStars)));

		if (WBP_StarWidget)
		{
			// 0.5초 뒤에 별 애니메이션 실행 (이미지 연출용)
			FTimerHandle StarAnimTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(StarAnimTimerHandle, [this, MyStars]()
			{
			   if(WBP_StarWidget) WBP_StarWidget->UpdateStars(MyStars, true);
			}, 0.5f, false);
		}
	}
}

void USuccessStage::OnNextStageClicked()
{
	if (NextLevelName.IsNone())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NextLevelName이 설정되지 않았습니다!"));
		return;
	}

	FString MapName = NextLevelName.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("이동 시도 중인 맵: %s"), *MapName));

	UGameplayStatics::OpenLevel(this, NextLevelName);
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