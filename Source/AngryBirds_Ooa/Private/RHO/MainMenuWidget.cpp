#include "RHO/MainMenuWidget.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 영상 재생 (안전하게 Null 체크 후 실행)
	if (MainMenuMediaPlayer && MainMenuVideoSource)
	{
		MainMenuMediaPlayer->OpenSource(MainMenuVideoSource);
		MainMenuMediaPlayer->Play();
	}

	// 2. 버튼 바인딩
	if (Btn_StageSelect) Btn_StageSelect->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStageSelectClicked);
	if (Btn_Options) Btn_Options->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
	if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
}

void UMainMenuWidget::OnStageSelectClicked() { UGameplayStatics::OpenLevel(this, FName("StageSelect")); }
void UMainMenuWidget::OnOptionsClicked() { UGameplayStatics::OpenLevel(this, FName("Options")); }
void UMainMenuWidget::OnQuitClicked() { UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true); }