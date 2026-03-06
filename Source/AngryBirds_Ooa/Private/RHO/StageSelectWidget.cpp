#include "RHO/StageSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
// 미디어 기능을 사용하기 위한 헤더 추가
#include "MediaPlayer.h"
#include "MediaSource.h"

void UStageSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 배경 영상 재생 로직 (추가)
	if (StageMediaPlayer && StageVideoSource)
	{
		StageMediaPlayer->OpenSource(StageVideoSource);
		StageMediaPlayer->Play();
	}

	// 2. 버튼 클릭 이벤트 바인딩
	if (Btn_BackToMainMenu)
	{
		Btn_BackToMainMenu->OnClicked.AddDynamic(this, &UStageSelectWidget::OnBackToMainMenuClicked);
	}
    
	if (Btn_Tutorial)
	{
		Btn_Tutorial->OnClicked.AddDynamic(this, &UStageSelectWidget::OnTutorialClicked);
	}

	if (Btn_Stage1_1)
	{
		Btn_Stage1_1->OnClicked.AddDynamic(this, &UStageSelectWidget::OnStage1_1Clicked);
	}
}

void UStageSelectWidget::OnBackToMainMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UStageSelectWidget::OnTutorialClicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestLevel")); 
}

void UStageSelectWidget::OnStage1_1Clicked()
{
	UGameplayStatics::OpenLevel(this, FName("TestLevel")); 
}