#include "RHO/StageSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
// 사운드 관련 헤더 추가
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

void UStageSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 배경 영상 재생 로직
	if (StageMediaPlayer && StageVideoSource)
	{
		StageMediaPlayer->OpenSource(StageVideoSource);
		StageMediaPlayer->Play();
	}

	// 2. 배경 음악 재생 로직 (추가)
	if (StageSelectBGM)
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(this, StageSelectBGM);
	}

	// 3. 버튼 클릭 이벤트 바인딩
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
	if (BGMComponent) BGMComponent->Stop();
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UStageSelectWidget::OnTutorialClicked()
{
	if (BGMComponent) BGMComponent->Stop();
	UGameplayStatics::OpenLevel(this, FName("TutorialMap")); 
}

void UStageSelectWidget::OnStage1_1Clicked()
{
	if (BGMComponent) BGMComponent->Stop();
	UGameplayStatics::OpenLevel(this, FName("1_1Map")); 
}