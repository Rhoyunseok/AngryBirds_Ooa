#include "RHO/MainMenuWidget.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/WidgetAnimation.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 배경 영상 재생
	if (MainMenuMediaPlayer && MainMenuVideoSource)
	{
		// 미디어 소스를 열면 자동으로 재생이 시작됩니다.
		MainMenuMediaPlayer->OpenSource(MainMenuVideoSource);
	}

	// 2. UI 등장 애니메이션 재생
	if (IntroAnim)
	{
		PlayAnimation(IntroAnim);
	}

	// 3. 버튼 이벤트 바인딩
	if (Btn_StageSelect)
	{
		Btn_StageSelect->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStageSelectClicked);
	}
    
	if (Btn_Options)
	{
		Btn_Options->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
	}

	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}
}

void UMainMenuWidget::OnStageSelectClicked()
{
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}

void UMainMenuWidget::OnOptionsClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Options"));
}

void UMainMenuWidget::OnQuitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}