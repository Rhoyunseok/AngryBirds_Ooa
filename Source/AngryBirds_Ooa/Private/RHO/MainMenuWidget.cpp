#include "RHO/MainMenuWidget.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 영상 재생
	if (MainMenuMediaPlayer && MainMenuVideoSource)
	{
		MainMenuMediaPlayer->OpenSource(MainMenuVideoSource);
		MainMenuMediaPlayer->Play();
	}

	// 2. 배경음악(BGM) 재생
	if (MainMenuBGM)
	{
		// SpawnSound2D를 사용하여 사운드를 생성하고 제어권을 가져옵니다.
		BGMComponent = UGameplayStatics::SpawnSound2D(this, MainMenuBGM);
	}

	// 3. 버튼 바인딩
	if (Btn_StageSelect) Btn_StageSelect->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStageSelectClicked);
	if (Btn_Options) Btn_Options->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
	if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
}

void UMainMenuWidget::OnStageSelectClicked() 
{ 
	StopMainMenuBGM();
	UGameplayStatics::OpenLevel(this, FName("StageSelect")); 
}

void UMainMenuWidget::OnOptionsClicked() 
{ 
	// 옵션 메뉴로 이동할 때 음악을 유지할지 끌지 선택할 수 있습니다.
	// 여기서는 일단 유지하는 방향으로 두거나 필요시 Stop을 호출하세요.
	UGameplayStatics::OpenLevel(this, FName("Options")); 
}

void UMainMenuWidget::OnQuitClicked() 
{ 
	StopMainMenuBGM();
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true); 
}

void UMainMenuWidget::StopMainMenuBGM()
{
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->Stop();
	}
}