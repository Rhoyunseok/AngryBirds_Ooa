#include "RHO/StageSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
// 사운드 관련 헤더 추가
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "RHO/Save/MyGameInstance.h"
#include "Components/TextBlock.h"
#include "RHO/Widget/StarWidget.h"

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
	UpdateStageStarsUI(TEXT("Tutorial Map"), Tutorial_StarWidget);
	UpdateStageStarsUI(TEXT("Stage1_1"), Stage1_1_StarWidget);
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

void UStageSelectWidget::UpdateStageStarsUI(FString StageName, UStarWidget* StarWidget)
{
	// [탐정 1번] 별 위젯이 제대로 바인딩 되었는지 검사
	if (!StarWidget) 
	{
		UE_LOG(LogTemp, Error, TEXT("====> [UI 에러] %s 스테이지의 StarWidget이 연결되지 않았습니다!"), *StageName);
		return;
	}
    
	UMyGameInstance* MyGI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!MyGI) return;

	// 해당 스테이지의 클리어 별 개수 가져오기 (0 ~ 3)
	int32 EarnedStars = MyGI->GetStageClearStars(StageName);

	// [탐정 2번] 로그 확인
	UE_LOG(LogTemp, Warning, TEXT("====> [별 로드] %s : %d 개"), *StageName, EarnedStars);

	// [핵심 변경] 텍스트 대신 별 위젯의 이미지/애니메이션 로직 호출
	// 스테이지 선택창이므로 애니메이션 없이(false) 즉시 채워진 상태로 보여줍니다.
	StarWidget->UpdateStars(EarnedStars, false);
}



