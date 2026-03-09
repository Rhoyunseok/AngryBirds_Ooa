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
	UpdateStageStarsUI(TEXT("TutorialMap"), Tutorial_Stars);
	UpdateStageStarsUI(TEXT("Stage1_1"), Stage1_1_Stars);
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

void UStageSelectWidget::UpdateStageStarsUI(FString StageName, UTextBlock* StarTextBlock)
{
	// [탐정 1번] C++ 코드가 블루프린트 UI(텍스트 블록)를 제대로 찾았는지 검사!
	if (!StarTextBlock) 
	{
		UE_LOG(LogTemp, Error, TEXT("====> [UI 에러] %s 스테이지의 TextBlock이 끊어져 있습니다! WBP에서 이름과 'Is Variable' 체크를 확인하세요!"), *StageName);
		return;
	}
    
	UMyGameInstance* MyGI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!MyGI) return;

	// 해당 스테이지의 클리어 별 개수 가져오기 (0 ~ 3)
	int32 EarnedStars = MyGI->GetStageClearStars(StageName);

	// [탐정 2번] 게임 인스턴스가 하드디스크에서 별을 몇 개나 가져왔는지 검사!
	UE_LOG(LogTemp, Warning, TEXT("====> [UI 로드 완료] %s 스테이지에서 꺼내온 별 개수: %d 개"), *StageName, EarnedStars);

	FString StarText = TEXT("☆☆☆");
	if (EarnedStars == 1) StarText = TEXT("★☆☆");
	else if (EarnedStars == 2) StarText = TEXT("★★☆");
	else if (EarnedStars >= 3) StarText = TEXT("★★★");
    
	StarTextBlock->SetText(FText::FromString(StarText));
}
