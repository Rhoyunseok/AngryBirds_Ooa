#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

// 전방 선언 (컴파일 속도 향상)
class UButton;
class UMediaPlayer;
class UMediaSource;
class USoundBase;
class UAudioComponent;

UCLASS()
class ANGRYBIRDS_OOA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// --- UI 구성 요소 ---
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;
    
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Options;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Quit;

	// --- 미디어 관련 (영상) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	UMediaPlayer* MainMenuMediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	UMediaSource* MainMenuVideoSource;

	// --- 오디오 관련 (추가됨) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* MainMenuBGM;

	// 재생 중인 사운드를 관리하기 위한 컴포넌트
	UPROPERTY()
	UAudioComponent* BGMComponent;

	virtual void NativeConstruct() override;

private:
	// 버튼 클릭 이벤트 핸들러
	UFUNCTION() void OnStageSelectClicked();
	UFUNCTION() void OnOptionsClicked();
	UFUNCTION() void OnQuitClicked();
    
	// 사운드 정지 헬퍼 함수
	void StopMainMenuBGM();
};